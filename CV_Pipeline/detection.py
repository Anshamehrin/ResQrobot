"""
ResQRobot - Computer Vision Pipeline
------------------------------------
Reads the live video stream from the ESP32-CAM and performs:
  1. Human detection      -> pretrained YOLOv8-nano model
  2. Motion detection     -> background subtraction (MOG2)
  3. Fire/Smoke detection -> HSV color-space thresholding

Detected hazards are printed as JSON and can be forwarded to the
Flutter app / server over HTTP or MQTT (see send_alert() below —
plug in your actual networking code there).

Requirements (see requirements.txt):
    pip install ultralytics opencv-python numpy requests
"""

import cv2
import numpy as np
import json
import time
from ultralytics import YOLO

# ---------------------------------------------------------------------------
# CONFIG - update these for your setup
# ---------------------------------------------------------------------------
ESP32_CAM_STREAM_URL = "http://192.168.1.50:81/stream"  # replace with your ESP32-CAM IP
SERVER_ALERT_ENDPOINT = "http://192.168.1.100:5000/alert"  # your Flask/Node server or MQTT bridge
FRAME_RESIZE_WIDTH = 640  # resize for faster inference

# Fire/smoke HSV ranges (tune these based on real test footage/lighting)
FIRE_LOWER = np.array([18, 50, 50])
FIRE_UPPER = np.array([35, 255, 255])
FIRE_PIXEL_THRESHOLD = 3000   # min number of "fire-colored" pixels to trigger alert

MOTION_MIN_AREA = 800  # min contour area to count as motion (filters out noise)

# ---------------------------------------------------------------------------
# SETUP
# ---------------------------------------------------------------------------
print("[INFO] Loading YOLOv8-nano model (pretrained)...")
model = YOLO("yolov8n.pt")   # auto-downloads on first run

bg_subtractor = cv2.createBackgroundSubtractorMOG2(history=500, varThreshold=40, detectShadows=False)


def send_alert(alert_type: str, details: dict):
    """
    Forward a detected hazard to the app/server.
    Replace this with a real HTTP POST / MQTT publish call.
    """
    payload = {
        "type": alert_type,
        "timestamp": time.time(),
        **details
    }
    print(f"[ALERT] {json.dumps(payload)}")
    # Example HTTP version (uncomment and adjust once your server is ready):
    #
    # import requests
    # try:
    #     requests.post(SERVER_ALERT_ENDPOINT, json=payload, timeout=1)
    # except requests.exceptions.RequestException as e:
    #     print(f"[WARN] Could not send alert: {e}")


def detect_humans(frame):
    """Run YOLOv8 and return True if a person is detected, plus annotated frame."""
    results = model(frame, classes=[0], verbose=False)  # class 0 = 'person' in COCO
    person_detected = False
    for r in results:
        if len(r.boxes) > 0:
            person_detected = True
        frame = r.plot()  # draws bounding boxes on the frame
    return person_detected, frame


def detect_motion(frame):
    """Return True if significant motion is detected in the frame."""
    fg_mask = bg_subtractor.apply(frame)
    _, thresh = cv2.threshold(fg_mask, 200, 255, cv2.THRESH_BINARY)
    contours, _ = cv2.findContours(thresh, cv2.RETR_EXTERNAL, cv2.CHAIN_APPROX_SIMPLE)
    motion_detected = any(cv2.contourArea(c) > MOTION_MIN_AREA for c in contours)
    return motion_detected


def detect_fire_smoke(frame):
    """Return True if fire/smoke-colored regions exceed the pixel threshold."""
    hsv = cv2.cvtColor(frame, cv2.COLOR_BGR2HSV)
    mask = cv2.inRange(hsv, FIRE_LOWER, FIRE_UPPER)
    fire_pixel_count = cv2.countNonZero(mask)
    return fire_pixel_count > FIRE_PIXEL_THRESHOLD


def main():
    print(f"[INFO] Connecting to ESP32-CAM stream: {ESP32_CAM_STREAM_URL}")
    cap = cv2.VideoCapture(ESP32_CAM_STREAM_URL)

    if not cap.isOpened():
        print("[ERROR] Could not open ESP32-CAM stream. Check the IP/URL and network connection.")
        return

    last_human_alert = 0
    last_fire_alert = 0
    ALERT_COOLDOWN = 5  # seconds, avoid spamming repeated alerts

    while True:
        ret, frame = cap.read()
        if not ret:
            print("[WARN] Failed to read frame, retrying...")
            time.sleep(0.5)
            continue

        # Resize for faster processing
        h, w = frame.shape[:2]
        scale = FRAME_RESIZE_WIDTH / w
        frame = cv2.resize(frame, (FRAME_RESIZE_WIDTH, int(h * scale)))

        # --- Human detection ---
        person_detected, annotated_frame = detect_humans(frame.copy())
        if person_detected and (time.time() - last_human_alert > ALERT_COOLDOWN):
            send_alert("human_detected", {"confidence": "see_boxes"})
            last_human_alert = time.time()

        # --- Motion detection ---
        motion = detect_motion(frame)
        if motion:
            send_alert("motion_detected", {})

        # --- Fire/smoke detection ---
        fire = detect_fire_smoke(frame)
        if fire and (time.time() - last_fire_alert > ALERT_COOLDOWN):
            send_alert("fire_smoke_detected", {})
            last_fire_alert = time.time()

        # --- Display (for local testing/debugging; remove for headless deployment) ---
        cv2.imshow("ResQRobot - CV Pipeline", annotated_frame)
        if cv2.waitKey(1) & 0xFF == ord('q'):
            break

    cap.release()
    cv2.destroyAllWindows()


if __name__ == "__main__":
    main()
