//======================================
// ResQ Robot Dashboard JavaScript
//======================================

// ESP32 DevKit IP
const ESP32_IP = "http://10.174.61.159";

// ESP32-CAM IP
const CAMERA_IP = "http://10.174.61.47";

//======================================
// Camera Stream
//======================================

window.onload = function () {

    document.getElementById("cameraFeed").src =
        CAMERA_IP + ":81/stream";

    document.getElementById("wifi").innerHTML = "CONNECTED";
    document.getElementById("cameraStatus").innerHTML = "ONLINE";
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
// Buzzer
//======================================

let buzzerState = false;

function toggleBuzzer() {

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

            // Gas Status
            document.getElementById("gas").innerHTML =
                data.gas;

            // Distance
            document.getElementById("distance").innerHTML =
                data.distance + " cm";

            // Robot Status
            document.getElementById("status").innerHTML =
                data.status;

            // Wi-Fi & Camera
            document.getElementById("wifi").innerHTML =
                "CONNECTED";

            document.getElementById("cameraStatus").innerHTML =
                "ONLINE";

            // Alert Card
            let alertStatus = "NONE";

            if (data.gas == "WARNING")
                alertStatus = "WARNING";

            if (data.gas == "DANGER")
                alertStatus = "DANGER";

            if (data.distance < 20)
                alertStatus = "OBSTACLE";

            document.getElementById("alertStatus").innerHTML =
                alertStatus;

            updateAlerts(data);

            updateGasColor(data.gas);

        })

        .catch(error => {

            console.log("ESP32 Offline");

            document.getElementById("wifi").innerHTML =
                "OFFLINE";

        });

}

//======================================
// Alerts
//======================================

function updateAlerts(data) {

    let alerts =
        document.getElementById("alertList");

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

    if (alerts.innerHTML == "") {

        alerts.innerHTML =
            "<li>✅ No Alerts</li>";

    }

}

//======================================
// Gas Color
//======================================

function updateGasColor(level) {

    let gas =
        document.getElementById("gas");

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

updateDashboard();