# ESP32-C6 Network Monitor

## 📌 Introduction

In modern connected systems, **network speed and stability** play a crucial role in ensuring **reliable communication** for IoT devices, smart home automation, industrial applications, and real-time data processing.  
Fluctuations in **RSSI (signal strength), latency, jitter, and throughput** can lead to **packet loss, increased response times, and poor performance** in Wi-Fi-dependent applications.

This project implements a **network monitoring tool** for the **ESP32-C6** microcontroller, measuring:
- **Wi-Fi signal strength (RSSI)**
- **Latency & Jitter (via ICMP ping)**
- **Throughput**
- **Wi-Fi 6-specific features: OFDMA & TWT (Target Wake Time)**

The project provides **real-time logging** to track **network performance variations**, which can be useful for:
✔ Debugging **Wi-Fi reliability issues**  
✔ Evaluating **network stability over time**  
✔ Optimizing **ESP32-C6 power usage and connectivity**

---

## 📌 Why Use the **ESP32-C6**?

The **ESP32-C6** is a **Wi-Fi 6** capable microcontroller with **enhanced connectivity features** that improve **network performance and power efficiency**. Key benefits include:

### ✅ **Wi-Fi 6 Advantages**
- **Faster speeds**: Supports **Wi-Fi 6 (802.11ax)**, improving performance over **Wi-Fi 4 (802.11n)** used in previous ESP32 models.
- **Lower power consumption**: Uses **Target Wake Time (TWT)** to reduce power consumption when idle.
- **Improved network efficiency**: Supports **OFDMA** (Orthogonal Frequency-Division Multiple Access) for handling multiple devices efficiently.

### ✅ **ESP32-C6 Features**
- **Supports both 2.4 GHz and 5 GHz bands** for flexible network testing.
- **Low-power RISC-V processor**, making it suitable for battery-powered applications.
- **Enhanced security features**, including WPA3 and hardware encryption.
- **BLE 5.0 & Thread/Zigbee support**, making it ideal for IoT devices.

The ESP32-C6 is **an excellent choice for real-time Wi-Fi analysis** while maintaining **low power consumption** in IoT environments.

---

## 📌 Project Setup

This project follows the **standard ESP-IDF setup** for ESP32 devices.

### **1️⃣ Prerequisites**
Ensure you have the **ESP-IDF development environment** set up. If not, follow the official **[ESP-IDF Installation Guide](https://docs.espressif.com/projects/esp-idf/en/latest/esp32/get-started/index.html).**

```sh
# Clone the repository
git clone https://github.com/yourusername/esp32c6-network-monitor.git
cd esp32c6-network-monitor

# Set up ESP-IDF environment
. $HOME/esp/esp-idf/export.sh

# Configure project by setting Wi-Fi SSID and password
idf.py menuconfig

```

### **2️⃣ Build and Flash**

```sh
# Build the project
idf.py build

# Flash the firmware
idf.py -p PORT flash monitor
```

### **3️⃣ Monitor Network Performance**

Open the **ESP32-C6 serial monitor** to view real-time network performance metrics. You can observe the **Wi-Fi RSSI, latency, jitter, throughput, OFDMA, and TWT** values.

## 📌 Example Log Output

After flashing, the ESP32-C6 will monitor the network every 5 seconds and print the results in the terminal:

```sh

I (104461) NetworkMonitor: RSSI: -65 dBm
W (104461) NetworkMonitor: Failed to get OFDMA status
I (104461) NetworkMonitor: TWT Enabled: Yes
I (104461) NetworkMonitor: Old ping session stopped and deleted.
I (104471) NetworkMonitor: Latency: 6 ms
I (105461) NetworkMonitor: Latency: 7 ms
I (106471) NetworkMonitor: Latency: 9 ms
I (107481) NetworkMonitor: Latency: 22 ms
I (108461) PING: 4 packets transmitted, 4 received, time 44 ms

```

### Explanation of Output
* ```RSSI: -65 dBm``` → Current Wi-Fi signal strength.
* ```Failed to get OFDMA status``` → OFDMA is not supported or the AP does not report it.
* ```TWT Enabled: Yes``` → Target Wake Time (TWT) is active.
* ```Latency: X ms``` → Time taken for ICMP ping response.
* ```PING: 4 packets transmitted, 4 received, time 44 ms``` → Final ping statistics.

## 📌 Future Improvements
🔹 Add sending received network parameter to central server via HTTP requests.
🔹 Implement power-saving optimizations based on TWT intervals.
🔹 Enhance network diagnostics with packet loss and retransmission analysis.

## 📌 License
This project is licensed under the MIT License. You are free to use and modify it for personal or commercial applications.
