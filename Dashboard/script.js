//======================================
// Rescue Robot Dashboard JavaScript
//======================================

// ESP32 DevKit
const ESP32_IP = "http://10.168.129.159";

// ESP32-CAM
const CAMERA_IP = "http://10.168.129.47";

// Camera Stream
window.onload = function ()
{
    document.getElementById("cameraFeed").src =
        CAMERA_IP + ":81/stream";
};

//======================================
// Robot Movement
//======================================

function move(direction) {

    document.getElementById("status").innerHTML =
        direction.toUpperCase();

    fetch(`${ESP32_IP}/move?dir=${direction}`)
        .then(response => response.text())
        .then(data => console.log(data))
        .catch(error => console.log(error));

}

//======================================
// Robot Mode
//======================================

function setMode(mode) {

    document.getElementById("mode").innerHTML = mode;

    fetch(`${ESP32_IP}/mode?value=${mode}`)
        .then(response => response.text())
        .then(data => console.log(data))
        .catch(error => console.log(error));

}

//======================================
// Buzzer
//======================================

let buzzerState = false;

function toggleBuzzer()
{
    buzzerState = !buzzerState;

    let value = buzzerState ? "ON" : "OFF";

    fetch(`${ESP32_IP}/buzzer?value=${value}`)
        .then(response => response.text())
        .then(data => console.log(data))
        .catch(error => console.log(error));
}

//======================================
// Dashboard Update
//======================================

function updateDashboard() {

    fetch(`${ESP32_IP}/data`)
        .then(response => response.json())
        .then(data => {


            document.getElementById("gas").innerHTML =
                data.gas;

            document.getElementById("distance").innerHTML =
                data.distance + " cm";

            document.getElementById("battery").innerHTML =
                data.battery + " %";

            document.getElementById("status").innerHTML =
                data.status;

            document.getElementById("mode").innerHTML =
                data.mode;

            document.getElementById("gps").innerHTML =
                "Latitude : " + data.latitude +
                "<br>Longitude : " + data.longitude;

            updateAlerts(data);

            updateGasColor(data.gas);

        })

        .catch(error => {

            console.log("ESP32 Offline");

        });

}

//======================================
// Alerts
//======================================

function updateAlerts(data) {

    let alerts = document.getElementById("alertList");

    alerts.innerHTML = "";

    if (data.gas == "DANGER") {

        alerts.innerHTML +=
            "<li>🚨 Dangerous Gas Detected</li>";

    }

    else if (data.gas == "WARNING") {

        alerts.innerHTML +=
            "<li>⚠ Gas Level Increasing</li>";

    }

    if (data.distance < 20) {

        alerts.innerHTML +=
            "<li>🚧 Obstacle Ahead</li>";

    }



    if (data.battery < 20) {

        alerts.innerHTML +=
            "<li>🔋 Battery Low</li>";

    }

    if (alerts.innerHTML == "") {

        alerts.innerHTML =
            "<li>✅ No Alerts</li>";

    }

}

//======================================
// Gas Color
//======================================

function updateGasColor(level) {

    let gas = document.getElementById("gas");

    gas.classList.remove("safe");
    gas.classList.remove("warning");
    gas.classList.remove("danger");

    if (level == "SAFE") {

        gas.classList.add("safe");

    }

    else if (level == "WARNING") {

        gas.classList.add("warning");

    }

    else {

        gas.classList.add("danger");

    }

}

//======================================
// Refresh Dashboard
//======================================

setInterval(updateDashboard, 1000);

// First Update

updateDashboard();
