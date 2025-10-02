# Gap Dipole antenna tuner firmware

The Gap Dipole is an antenna and remote tuner that allows both changing the length of the dipole in real time, via switched relays, and remote tuning.   The tuner hardware sits at the antenna feed point and is controlled over WiFi.   The ESP32 in the tuner provides a web server which supplies the control page for the tuner.  The control web page (accessed via phone,table, computer, etc.), provides a UI to operate the tuner functions.

*A unique feature of the Gap dipole is that it is characterized once with a VNA, and that characterization is uploaded to the tuner over WiFi.  After that, when the user switches frequency that freqency is given to the tuner and it tunes the antenna to the best possible match based on the VNA characterizaton, without the traditional need to transmit a signal while the tuner tries various tuning network combinations.*

We are currently using the ESP32-S3-DevKitC-1U-N8 from Espressif (with the ipex antenna connector), as we have experienced reliability issues with some clones.

# 📡 GAPTuner Wi-Fi Setup Guide

This guide will help you connect your **GAPTuner** to your local Wi-Fi network using a browser-based setup. Once connected, you'll be able to access the control interface from any device on your network.

---

## 🛠️ Getting Started

When powered on for the first time, GAPTuner creates its own Wi-Fi network so you can configure it.

---

## ✅ Step 1: Power On the GAPTuner

- Plug in your GAPTuner.
- It will boot into **Access Point (AP) mode**, broadcasting its own Wi-Fi network.

---

## ✅ Step 2: Connect to the GAPTuner's Wi-Fi

- Open the **Wi-Fi settings** on your phone, tablet, or computer.
- Look for a network named:  GAP Tuner AP - XXXX
(where `XXXX` is a unique ID for your device)

- Connect to it.  
- **Password:** _(if applicable – depends on your device's configuration)_

---

## ✅ Step 3: Open the Setup Page

- Your device may automatically open a configuration page.  
- If not, open a browser and go to: http://192.168.4.1


---

## ✅ Step 4: Enter Your Wi-Fi Information

- Select your **home Wi-Fi network** from the list.
- Enter your **Wi-Fi password**.
- Click **Connect**.

---

## ✅ Step 5: Wait for Reboot

- GAPTuner will save your Wi-Fi credentials and reboot.
- It will now attempt to connect to your Wi-Fi network as a regular device using DHCP.


## ✅ Step 6: Reconnect Your Device to Wi-Fi

- Return to your device's Wi-Fi settings.
- Reconnect to your **normal home Wi-Fi network**.


## ✅ Step 7: Access GAPTuner on Your Network

Once the GAPTuner connects to your network, you can access its control interface.

- Open a browser and go to: http://gaptuner.local


> 💡 **Note:** This uses **multicast DNS (mDNS)**. It works on:
> - macOS and iOS (built-in)
> - Windows 10+ (modern versions)
> - Android (with certain apps)
> - Linux with `avahi-daemon`

If this doesn’t work, use a network scanner app to find the device’s IP address.


## 🔄 What If Something Goes Wrong?

If GAPTuner fails to connect to your Wi-Fi (wrong password, signal issue, etc.):

- It will **automatically return to AP mode**.
- Just repeat the setup process from **Step 2**.


Current Web UI image:

![Image](https://github.com/user-attachments/assets/aa330623-e645-4571-9226-5e76ad633da7)










