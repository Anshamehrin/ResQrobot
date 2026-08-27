# CV Pipeline - ResQRobot

Computer vision module that processes the live ESP32-CAM video feed to detect:

- **Humans** — using a pretrained YOLOv8-nano model (via [Ultralytics](https://github.com/ultralytics/ultralytics))
- **Motion** — using OpenCV background subtraction (MOG2)
- **Fire/Smoke** — using HSV color-space thresholding

Detected hazards are printed as JSON alerts and are meant to be forwarded to the Flutter app / server (see the `send_alert()` function in `detection.py` — plug in your actual HTTP/MQTT call there).

## Setup

```bash
pip install -r requirements.txt
```

The YOLOv8-nano weights (`yolov8n.pt`) will auto-download the first time you run the script.

## Configuration

Open `detection.py` and update:

- `ESP32_CAM_STREAM_URL` — your ESP32-CAM's stream URL (e.g. `http://<esp32-cam-ip>:81/stream`)
- `SERVER_ALERT_ENDPOINT` — the server/endpoint your Flutter app listens on for alerts
- `FIRE_LOWER` / `FIRE_UPPER` — HSV thresholds for fire/smoke detection; tune these based on real test footage and lighting conditions
- `MOTION_MIN_AREA` — minimum contour area to count as motion (filters out sensor noise)

## Run

```bash
python detection.py
```

A window will show the live feed with YOLO bounding boxes drawn on detected people. Alerts are printed to the console; press `q` to quit.

## Status

- [x] Human detection (pretrained YOLOv8n)
- [x] Motion detection (background subtraction)
- [x] Fire/smoke detection (HSV thresholding)
- [ ] Integration with Flutter app (alert forwarding via HTTP/MQTT)
- [ ] Threshold tuning based on real hardware/lighting tests
- [ ] Custom-trained fire/smoke model (future improvement)
