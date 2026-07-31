/****************************************************
      SEMI-AUTONOMOUS SEARCH & RESCUE ROBOT
                BLOCK 1 - LIBRARIES
****************************************************/

#include <WiFi.h>           // Connect ESP32 to Wi-Fi
#include <WebServer.h>      // Create web server
#include <FS.h>
#include <SPIFFS.h>  
#include <TinyGPS++.h>
#include <HardwareSerial.h>
 // ESP32-CAM Library

/****************************************************
            BLOCK 2 - PIN DEFINITIONS
****************************************************/

//========== MQ-2 GAS SENSOR ==========
#define MQ2_PIN 34          // Analog Output (AO)

//========== HC-SR04 ULTRASONIC SENSOR ==========
#define TRIG_PIN 13
#define ECHO_PIN 12

//========== BUZZER ==========
#define BUZZER_PIN 2

//========== L298N MOTOR DRIVER ==========
//========== GPS MODULE ==========
#define GPS_RX 26
#define GPS_TX 27
#define IN1 14
#define IN2 15
#define IN3 16
#define IN4 17

// ENA and ENB are connected to jumper caps
// Motors will run at full speed

/****************************************************
          BLOCK 3 - Wi-Fi CREDENTIALS
****************************************************/

// Wi-Fi Hotspot Details
const char* ssid = "Galaxy S24";
const char* password = "11102006";

// Create Web Server on Port 80
WebServer server(80);
TinyGPSPlus gps;
HardwareSerial GPS_Serial(2);

/****************************************************
          BLOCK 4 - GLOBAL VARIABLES
****************************************************/

// MQ-2 Gas Sensor Reading
int gasValue = 0;

// Ultrasonic Sensor Reading
long distance = 0;

// Robot Movement Status
String robotStatus = "STOPPED";

// Obstacle Detection
bool obstacleDetected = false;

// Gas Detection
bool gasDetected = false;
// Auto Mode Flag
bool autoMode = false;

// Manual Buzzer Control
bool manualBuzzer = false;

float latitude = 0.0;
float longitude = 0.0;


/****************************************************
          BLOCK 5 - MOTOR FUNCTIONS
****************************************************/

// Move Robot Forward
void forwardRobot()
{
    digitalWrite(IN1, HIGH);
    digitalWrite(IN2, LOW);

    digitalWrite(IN3, HIGH);
    digitalWrite(IN4, LOW);

    robotStatus = "FORWARD";
}

// Move Robot Backward
void backwardRobot()
{
    digitalWrite(IN1, LOW);
    digitalWrite(IN2, HIGH);

    digitalWrite(IN3, LOW);
    digitalWrite(IN4, HIGH);

    robotStatus = "BACKWARD";
}

// Turn Robot Left
void leftRobot()
{
    digitalWrite(IN1, LOW);
    digitalWrite(IN2, HIGH);

    digitalWrite(IN3, HIGH);
    digitalWrite(IN4, LOW);

    robotStatus = "LEFT";
}

// Turn Robot Right
void rightRobot()
{
    digitalWrite(IN1, HIGH);
    digitalWrite(IN2, LOW);

    digitalWrite(IN3, LOW);
    digitalWrite(IN4, HIGH);

    robotStatus = "RIGHT";
}

// Stop Robot
void stopRobot()
{
    digitalWrite(IN1, LOW);
    digitalWrite(IN2, LOW);

    digitalWrite(IN3, LOW);
    digitalWrite(IN4, LOW);

    robotStatus = "STOPPED";
}

/****************************************************
      BLOCK 6 - ULTRASONIC SENSOR FUNCTIONS
****************************************************/

// Function to Measure Distance
long readDistance()
{
    // Clear the TRIG pin
    digitalWrite(TRIG_PIN, LOW);
    delayMicroseconds(2);

    // Send a 10 microsecond pulse
    digitalWrite(TRIG_PIN, HIGH);
    delayMicroseconds(10);
    digitalWrite(TRIG_PIN, LOW);

    // Measure the pulse duration
    long duration = pulseIn(ECHO_PIN, HIGH, 30000);

if (duration == 0)
{
    return 999;
}

    // Calculate distance in centimeters
    long distanceCM = duration * 0.0343 / 2;

    return distanceCM;
}

// Function to Detect Obstacles
void obstacleDetection()
{
    // Read the current distance
    distance = readDistance();

    // If obstacle is within 20 cm
    if (distance <= 20)
    {
        obstacleDetected = true;

        // Stop the robot immediately
        stopRobot();
    }
    else
    {
        obstacleDetected = false;
    }
}

/****************************************************
        BLOCK 7 - MQ-2 GAS SENSOR FUNCTIONS
****************************************************/

// Function to Read MQ-2 Gas Sensor
int readGas()
{
    // Read the analog value from MQ-2
    gasValue = analogRead(MQ2_PIN);

    // Return the sensor value
    return gasValue;
}

// Function to Detect Gas
void gasDetection()
{
    // Read current gas value
    gasValue = readGas();

    // Check if gas value exceeds threshold
    if (gasValue >= 600)
    {
        gasDetected = true;
    }
    else
    {
        gasDetected = false;
    }
}
/****************************************************
            BLOCK 8 - BUZZER FUNCTIONS
****************************************************/

// Turn ON the buzzer
void buzzerON()
{
    digitalWrite(BUZZER_PIN, HIGH);
}

// Turn OFF the buzzer
void buzzerOFF()
{
    digitalWrite(BUZZER_PIN, LOW);
}

// Check gas status and control buzzer
void buzzerControl()
{
    if (manualBuzzer)
    {
        buzzerON();
        return;
    }

    if (gasDetected)
    {
        buzzerON();
    }
    else
    {
        buzzerOFF();
    }
}
/****************************************************
          BLOCK 9 - Wi-Fi CONNECTION
****************************************************/

void connectWiFi()
{
    Serial.println();
    Serial.println("Connecting to Wi-Fi...");

    WiFi.begin(ssid, password);

    while (WiFi.status() != WL_CONNECTED)
    {
        delay(500);
        Serial.print(".");
    }

    Serial.println();
    Serial.println("Wi-Fi Connected!");
    Serial.print("IP Address: ");
    Serial.println(WiFi.localIP());
}

/****************************************************
      BLOCK 10 - SPIFFS INITIALIZATION
****************************************************/

void startSPIFFS()
{
    if (!SPIFFS.begin(true))
    {
        Serial.println("SPIFFS Mount Failed");
        return;
    }

    Serial.println("SPIFFS Mounted Successfully");
}
/****************************************************
          BLOCK 11 - WEB SERVER ROUTES
****************************************************/

// Send Home Page
void handleRoot()
{
    File file = SPIFFS.open("/index.html", "r");

    if (!file)
    {
        server.send(404, "text/plain", "index.html not found");
        return;
    }

    server.streamFile(file, "text/html");
    file.close();
}

// Send CSS File
void handleCSS()
{
    File file = SPIFFS.open("/style.css", "r");

    if (!file)
    {
        server.send(404, "text/plain", "style.css not found");
        return;
    }

    server.streamFile(file, "text/css");
    file.close();
}

// Send JavaScript File
void handleJS()
{
    File file = SPIFFS.open("/script.js", "r");

    if (!file)
    {
        server.send(404, "text/plain", "script.js not found");
        return;
    }

    server.streamFile(file, "application/javascript");
    file.close();
}
/****************************************************
        BLOCK 12 - ROBOT CONTROL ROUTE
****************************************************/

void handleMove()
{
    // Check if "dir" parameter exists
    if (!server.hasArg("dir"))
    {
        server.send(400, "text/plain", "Direction Missing");
        return;
    }

    // Get the direction from URL
    String direction = server.arg("dir");

    if (direction == "forward")
    {
        forwardRobot();
    }
    else if (direction == "backward")
    {
        backwardRobot();
    }
    else if (direction == "left")
    {
        leftRobot();
    }
    else if (direction == "right")
    {
        rightRobot();
    }
    else if (direction == "stop")
    {
        stopRobot();
    }

    server.send(200, "text/plain", "OK");
}
/****************************************************
        BLOCK 13 - SENSOR DATA API
****************************************************/

// Robot Mode
String robotMode = "MANUAL";

// Dummy Battery Percentage
int batteryLevel = 100;

// Send Sensor Data as JSON
void handleData()
{
    // Update sensor values
    gasDetection();
    obstacleDetection();

    // Determine gas status
    String gasStatus;

    if (gasValue < 400)
    {
        gasStatus = "SAFE";
    }
    else if (gasValue < 600)
    {
        gasStatus = "WARNING";
    }
    else
    {
        gasStatus = "DANGER";
    }

    // Create JSON response
    String json = "{";

    json += "\"gas\":\"" + gasStatus + "\",";
    json += "\"gasValue\":" + String(gasValue) + ",";
    json += "\"distance\":" + String(distance) + ",";
    json += "\"status\":\"" + robotStatus + "\",";
    json += "\"mode\":\"" + robotMode + "\",";
    json += "\"battery\":" + String(batteryLevel) + ",";
    json += "\"latitude\":" + String(latitude, 6) + ",";
    json += "\"longitude\":" + String(longitude, 6);
    json += "}";

    server.send(200, "application/json", json);
}
/****************************************************
        BLOCK 14 - MODE & BUZZER ROUTES
****************************************************/



//================ AUTO / MANUAL MODE ================

void handleMode()
{
    if (!server.hasArg("value"))
    {
        server.send(400, "text/plain", "Mode Missing");
        return;
    }

    String mode = server.arg("value");

    if (mode == "AUTO")
    {
        autoMode = true;
        robotMode = "AUTO";
    }
    else
    {
        autoMode = false;
        robotMode = "MANUAL";
        stopRobot();
    }

    server.send(200, "text/plain", "OK");
}


//================ BUZZER CONTROL ====================

void handleBuzzer()
{
    if (!server.hasArg("value"))
    {
        server.send(400, "text/plain", "Value Missing");
        return;
    }

    String value = server.arg("value");

    if (value == "ON")
    {
        manualBuzzer = true;
        buzzerON();
    }
    else
    {
        manualBuzzer = false;
        buzzerOFF();
    }

    server.send(200, "text/plain", "OK");
}
/****************************************************
                BLOCK 15 - SETUP
****************************************************/
/****************************************************
                GPS FUNCTIONS
****************************************************/

void readGPS()
{
    while (GPS_Serial.available())
    {
        gps.encode(GPS_Serial.read());
    }

    if (gps.location.isUpdated())
{
    latitude = gps.location.lat();
    longitude = gps.location.lng();
}
}
void setup()
{
    // Start Serial Monitor
    Serial.begin(115200);
    GPS_Serial.begin(9600, SERIAL_8N1, GPS_RX, GPS_TX);
    Serial.println("GPS Started...");

    //========== Configure Motor Pins ==========
    pinMode(IN1, OUTPUT);
    pinMode(IN2, OUTPUT);
    pinMode(IN3, OUTPUT);
    pinMode(IN4, OUTPUT);

    //========== Configure Ultrasonic ==========
    pinMode(TRIG_PIN, OUTPUT);
    pinMode(ECHO_PIN, INPUT);

    //========== Configure MQ-2 ==========
    pinMode(MQ2_PIN, INPUT);

    //========== Configure Buzzer ==========
    pinMode(BUZZER_PIN, OUTPUT);

    // Keep robot stopped initially
    stopRobot();

    // Turn buzzer OFF initially
    buzzerOFF();

    // Connect to Wi-Fi
    connectWiFi();

    // Start SPIFFS
    startSPIFFS();

    //========== Register Web Routes ==========
    server.on("/", handleRoot);

    server.on("/style.css", handleCSS);

    server.on("/script.js", handleJS);

    server.on("/move", handleMove);

    server.on("/data", handleData);

    server.on("/mode", handleMode);

    server.on("/buzzer", handleBuzzer);

    // Start Web Server
    server.begin();

    Serial.println("--------------------------------");
    Serial.println("Web Server Started Successfully");
    Serial.println("--------------------------------");
}
/****************************************************
                BLOCK 16 - LOOP
****************************************************/

void loop()
{
    // Handle all web requests
    server.handleClient();
    readGPS();

    // Read sensors
    gasDetection();
    obstacleDetection();

    // Control buzzer
    buzzerControl();

    //========== AUTO MODE ==========
    if (autoMode)
    {
        // If obstacle detected
        if (obstacleDetected)
        {
            stopRobot();
        }
        else
        {
            forwardRobot();
        }
    }

    delay(100);
}

