# 🔐 Secure CAN Communication using STM32 TrustZone + ChaCha20-Poly1305

This project demonstrates a secure and lightweight **CAN communication framework** for embedded automotive systems using **STM32H5 (Cortex-M33)** with **ARM TrustZone** and **ChaCha20-Poly1305** authenticated encryption.

It includes simulation of various CAN-based attacks and showcases how encryption and secure-world isolation can protect against them in real-time.

---

## 🚀 Features

- 🔐 **ChaCha20-Poly1305** encryption and authentication for CAN data
- 🔒 **TrustZone-based isolation** of encryption keys and secure logic
- 🔁 **Periodic Nonce Update** protocol with encrypted sync messages
- 🧪 **Attack Simulation**: Message Injection, Replay, Fuzzing, Spoofing
- 📟 **Data UART** to PC for decrypted message logging
- 🐞 **Debug UART** for internal status and failure logs

---

## 🧩 System Architecture

- **Sensor Node (TX Board)**:
  - Encrypts and sends data over CAN
  - Initiates nonce update and simulates attacks via User Button

- **ECU Node (RX Board)**:
  - Receives and decrypts CAN data in the secure world
  - Validates nonce, MAC, and payload authenticity

- **PC Host**:
  - Reads plain output via UART
  - Displays data and logs for testing/verification

---

## 🛠 Hardware Requirements

- 2× [NUCLEO-H563ZI](https://www.st.com/en/evaluation-tools/nucleo-h563zi.html)
- 2× CAN Transceivers (e.g., MCP2551, TJA1040)
- USB-to-UART adapter (for Data UART)
- Optional: Logic analyzer or CAN analyzer for bus monitoring

---

