# ResQRobot - Smart Rescue Robot using ESP32

A smart IoT-based rescue robot designed to assist in emergency and disaster situations. The robot can detect gas leaks, avoid obstacles, provide live location updates, stream video through an ESP32-CAM, and display all information on a web dashboard for remote monitoring.

## Features

*   **Live Camera Streaming** using ESP32-CAM
*   **Gas Leakage Detection** using MQ-2 Gas Sensor
*   **Obstacle Detection** using HC-SR04 Ultrasonic Sensor
*   **Buzzer Alert** for gas leaks and obstacles
*   **Robot Movement Control** (Forward, Backward, Left, Right, Stop)
*   **GPS Tracking** for real-time location
*   **Web Dashboard** for monitoring robot status
*   **Wi-Fi Connectivity** for remote access


##  Hardware Components

* ESP32 Dev Module
* ESP32-CAM
* HC-SR04 Ultrasonic Sensor
* MQ-2 Gas Sensor
* GPS Module (NEO-6M)
* L298N Motor Driver
* 2 × DC Gear Motors
* Buzzer
* Robot Chassis
* Wheels
* Battery Pack
* Jumper Wires

##  Software & Tools

* Arduino IDE
* ESP32 Board Package
* Wokwi Simulator (for initial testing)
* HTML
* CSS
* JavaScript
* Git & GitHub

##  Working Principle

1. The ESP32 continuously reads data from the ultrasonic sensor and gas sensor.
2. The ultrasonic sensor measures the distance to nearby obstacles.
3. If an obstacle is detected within the safety range, the robot stops to prevent collision.
4. The MQ-2 gas sensor monitors the environment for harmful gas.
5. If gas concentration exceeds the threshold, the buzzer is activated and an alert is generated.
6. The GPS module provides the robot's live location.
7. The ESP32-CAM streams live video to the dashboard.
8. All sensor readings, GPS location, and camera feed are displayed on the web dashboard in real time.

##  Dashboard

The dashboard displays:

* Live camera feed
* Gas sensor value
* Obstacle distance
* Robot status
* GPS location
* Alert notifications

##  Applications

* Disaster response
* Search and rescue operations
* Hazardous gas monitoring
* Industrial inspection
* Mine safety
* Fire emergency support
* Remote surveillance

##  Future Improvements

* AI-based human detection
* Fire detection using computer vision
* Autonomous path planning
* Voice control
* Mobile application
* Cloud data logging
* Machine learning-based obstacle avoidance

##  License

This project is licensed under the MIT License.

##  Team Members

### Ansha Mehrin M N

* System design and project integration
* ESP32 firmware development
* ESP32-CAM programming
* Web dashboard development
* Sensor integration and testing
* Software debugging and optimization
* GitHub repository management
* Project documentation

### Aavanthika M Nair & Arsha Karinkallayi

* Hardware assembly
* Wiring and component integration
* Mechanical setup
* Robot chassis setup
* Assistance with hardware testing
 

Electronics and Communication Engineering
Model Engineering College , Thrikkakara
Kerala, India 
