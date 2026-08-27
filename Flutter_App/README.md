# Flutter App - ResQRobot Dashboard

Mobile application for remotely monitoring and controlling ResQRobot. Displays live video, sensor data, GPS location, and hazard alerts, and lets the operator drive the robot remotely.

## Planned Features

- **Live Video Feed** — displays the ESP32-CAM stream
- **Robot Movement Controls** — forward, backward, left, right, stop
- **Sensor Dashboard** — real-time gas level and obstacle distance readouts
- **GPS Map View** — shows the robot's live location on a map
- **Hazard Alerts** — human detected, motion detected, fire/smoke detected, gas leak, obstacle-stop — pushed from the CV pipeline and ESP32 over Wi-Fi
- **Connection Status** — indicates whether the robot/CV pipeline is currently reachable

## Planned Tech Stack

- Flutter & Dart
- `http` or `mqtt_client` package for receiving sensor data/alerts
- `google_maps_flutter` or `flutter_map` for GPS visualization
- A simple state management approach (Provider or Riverpod)

## Data Flow

```
[ESP32] --Wi-Fi--> [Server] --Wi-Fi--> [Flutter App]
                       ^
              [CV Pipeline alerts]
```

The app subscribes to the same server/broker that the ESP32 firmware and CV pipeline publish sensor data and alerts to (see `CV_Pipeline/detection.py`'s `send_alert()` function and the Arduino code's Wi-Fi data push).

## Status

- [ ] Project scaffolding (`flutter create`)
- [ ] UI layout for dashboard screen
- [ ] Live video feed integration
- [ ] Sensor data integration (gas, distance, GPS)
- [ ] Map view for GPS location
- [ ] Hazard alert notifications
- [ ] Robot movement controls
- [ ] End-to-end testing with hardware

This module is currently under active development. Code will be added here as it's built out.
