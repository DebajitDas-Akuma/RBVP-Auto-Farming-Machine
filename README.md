# 🌱 RBVP Auto Farming Machine

An IoT-based smart farming system built using ESP32 for automated plant monitoring and environmental management.

## 🚀 Overview

RBVP Auto Farming Machine is a smart agriculture project designed to monitor plant health and automate farming-related tasks using sensors, actuators, and a web-based dashboard.

The system can:

* Monitor soil moisture
* Detect light intensity
* Measure temperature & humidity
* Detect nearby obstacles using radar scanning
* Analyze plant leaf color
* Automatically control devices based on environmental conditions

---

# 🛠️ Hardware Components

* ESP32
* Soil Moisture Sensor
* DHT Sensor (Temperature & Humidity)
* BH1750 Light Sensor
* Ultrasonic Sensor
* Servo Motors
* RGB LED
* TCS3200 Color Sensor
* Relay Modules / Output Devices

---

# ⚡ Features

## 🌡️ Environmental Monitoring

* Real-time temperature and humidity tracking
* Light intensity monitoring
* Soil moisture analysis

## 💧 Smart Irrigation

* Detects dry soil conditions
* Indicates watering requirement automatically

## 🌈 Plant Health Detection

* Uses TCS3200 color sensor
* Detects leaf color changes for plant condition analysis

## 📡 Radar Detection System

* 180° radar scanning using ultrasonic sensor + servo
* Displays detected objects on web UI
* Activates safety devices when objects are within 30 cm

## 💡 RGB LED Indicators

| Condition    | LED Color |
| ------------ | --------- |
| Needs Water  | Blue      |
| Needs Light  | Yellow    |
| Needs Shadow | Purple    |

## 🌐 Web Dashboard

* Live sensor data
* Radar visualization
* Moisture control interface
* Plant requirement notifications

---

# 📷 Project Preview

Add screenshots or images here.

Example:
```
/images/dashboard.png
/images/hardware.jpg
```

---

# 🔌 Pin Configuration

| Component   | ESP32 Pin |
| ----------- | --------- |
| Soil Servo  | GPIO 18   |
| Radar Servo | GPIO 19   |
| Device A    | GPIO 2    |
| Device B    | GPIO 4    |

---

# 📦 Libraries Used

Install these libraries in Arduino IDE:

* WiFi.h
* WebServer.h
* ESP32Servo.h
* DHT.h
* Wire.h
* BH1750.h

---

# ⚙️ Setup Instructions

## 1️⃣ Clone Repository

```bash
git clone https://github.com/YOUR_USERNAME/YOUR_REPOSITORY.git
```

## 2️⃣ Open in Arduino IDE

Open the `.ino` file in Arduino IDE.

## 3️⃣ Install Required Libraries

Install all required libraries from Library Manager.

## 4️⃣ Select Board

Choose:
```
ESP32 Dev Module
```

## 5️⃣ Upload Code

Connect ESP32 and upload the code.

---

# 📡 WiFi Configuration

Edit the following in code:

```cpp
const char* ssid = "YOUR_WIFI_NAME";
const char* password = "YOUR_WIFI_PASSWORD";
```

---

# 🎯 Future Improvements

* AI-based plant disease detection
* Mobile app integration
* Cloud data logging
* Automated fertilizer system
* Solar-powered operation

---

# 👨‍💻 Author

Debajit Das

GitHub:
https://github.com/DebajitDas-Akuma

---

# 📜 License

This project is open-source and available under the MIT License.
