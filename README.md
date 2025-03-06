# DSF to WAV Converter

## 🎯 Overview
The **DSF to WAV Converter** is a C++ utility that converts **DSD (Direct Stream Digital) audio** stored in **DSF files** into **WAV format**. It supports:
- **Standard PCM WAV output**
- **DSD over PCM (DoP) encoding**
- **Multiple PCM bit depths & sample rates**
- **Multi-channel support (Stereo, 5.1, 7.1)**

---

## 🚀 Features
✅ **Converts DSF to WAV** (PCM or DoP)  
✅ **Supports DSD64, DSD128, DSD256, DSD512, DSD1024**  
✅ **Manually implemented WAV writer (no dependencies)**  
✅ **Named command-line arguments for flexibility**  
✅ **Validates output configurations against DoP standards**  

---

## 🔧 **Installation & Compilation**
### **🔹 Linux / macOS**
```sh
g++ -std=c++17 -o dsf_to_wav DsdToWavConverter.cpp -Wall
```
### **🔹 Android (NDK)**
Use the provided `Android.bp` to integrate into AOSP:
```sh
mm -j$(nproc)
```

---

## 📌 **Usage**
```sh
./dsf_to_wav <input.dsf> <output.wav> <bitwidth> <samplerate> <channels> [dop]
```

### **Example 1: Convert DSF to 24-bit PCM WAV (Stereo)**
```sh
./dsf_to_wav song.dsf song.wav 24 176400 2 0
```

### **Example 2: Convert DSF to DoP WAV for DSD-capable DAC**
```sh
./dsf_to_wav song.dsf song_dop.wav 24 176400 2 1
```

---

## 🎵 **What is DoP (DSD over PCM)?**
**DoP (DSD over PCM)** is a method of encapsulating **DSD audio** inside **PCM frames** for transport via standard audio interfaces like **USB, SPDIF, and AES/EBU**.

### **DoP Rules:**
- **Only 24-bit PCM is supported for DoP (as per the standard).**
- **DoP uses marker bytes (`0x05FA`) to signal DSD data inside PCM.**
- **Used when a DAC supports DSD only through PCM interfaces.**

💡 **Use DoP only if your DAC requires it!**

---

## 🔍 **Supported Output Configurations**
The converter follows standard **PCM wrapping rules** for **DSD over PCM**.

| **DSD Type** | **DSD Sampling Rate** | **PCM Sampling Rate** | **Channels** | **Bit Width** | **DoP Supported?** |
|-------------|----------------|----------------|-----------|------------|--------------|
| **DSD64**   | **2.8224 MHz**  | **176.4 kHz**  | **2**     | **24-bit** | ✅ Yes |
| **DSD128**  | **5.6448 MHz**  | **352.8 kHz**  | **2**     | **24-bit** | ✅ Yes |
| **DSD256**  | **11.2896 MHz** | **705.6 kHz**  | **2**     | **24-bit** | ✅ Yes |
| **DSD512**  | **22.5792 MHz** | **1.4112 MHz** | **2**     | **24-bit** | ✅ Yes |
| **DSD1024** | **45.1584 MHz** | **2.8224 MHz** | **2**     | **24-bit** | ✅ Yes |

📌 **Multichannel DSD (5.1, 7.1) is supported at 24-bit PCM but requires high bandwidth.**

---

## 🔧 **Troubleshooting**
### 🔹 **Getting a "Invalid output configuration" error?**
Ensure your selected **bit-width, sample rate, and channels** are **valid** as per the table above.

### 🔹 **Why does DoP require 24-bit PCM?**
The **DoP standard** reserves **8 bits** for **DoP markers**, leaving **16 bits for DSD data**. **32-bit PCM is not part of the official standard.**

---

## 📜 **License**
This project is licensed under **MIT License**.

---

## 📩 **Contributions & Support**
- **Report issues** via GitHub.
- **Feature requests & PRs are welcome!** 🎉
