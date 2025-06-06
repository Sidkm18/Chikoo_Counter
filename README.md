
# 🍈 Chikoo Counter — Smart Counting System using ESP32 & Arduino Uno

A dual-microcontroller system that automatically counts *chikoos* (sapodillas) using sensors and controls a servo-powered gate based on a configurable target. The project uses an **ESP32-CAM** for detection and sends signals to an **Arduino Uno**, which handles counting, target-based logic, LCD output, and keypad input.

## 🎥 Demo Video / Playlist

Watch the full build process and demo on YouTube:  
👉 [Chikoo Counter Playlist](https://youtube.com/playlist?list=PL1rhOocHJL5jO05mjCsfs9AhGH4bDs13j&feature=shared)

## 🛠️ Features

- 🚀 **ESP32-CAM based Chikoo detection**
- 🧠 **Arduino Uno-based logic controller**
  - IR sensor filtering
  - LCD display (I2C)
  - Keypad menu for selecting modes
  - Servo motor control (for door/gate)
  - Buzzer alerts
- 📟 **User Interface via 16x2 LCD + Keypad**
- 🎯 **Target Mode**: Set a target count to auto-close the gate
- 📊 **Live Count Mode**: Keep counting till the max capacity is achieved then close the trapdoor

## 📡 Communication

- **ESP32 → Arduino Uno** via Serial
  - `'D'` = Detected
- **Arduino Uno → ESP32**
  - `'C'` = Count confirmed

## 🔁 Modes

| Mode         | Description                                    |
|--------------|------------------------------------------------|
| `MAIN MENU`  | Choose between count or target mode            |
| `COUNTING`   | Real-time counting with IR sensor filtering    |
| `TARGET`     | Set a target count; closes gate when reached   |

## 🎮 Controls (Keypad)

| Key   | Action                            |
|-------|-----------------------------------|
| `A`   | Start **Counting Mode**           |
| `B`   | Start **Target Mode**             |
| `*`   | Confirm or stop counting          |
| `D`   | Return to main menu               |
| `0-9` | Enter numbers for target count    |
| `C`   | Backspace (Target Mode)           |
| `#`   | Cancel/stop in Target Mode        |


## 🔌 Hardware Used

- ESP32-CAM
- Arduino Uno
- IR Sensor
- Servo Motor
- Buzzer
- 16x2 LCD with I2C module
- 4x4 Matrix Keypad
- Jumper wires, breadboard, power supply

## 🧠 Code Structure

- **ESP32-CAM Code**: Detects chikoos using image or sensor and sends `'D'` to Uno via UART
- **Arduino Uno Code**: Handles the UI, servo control, IR sensor debounce, count verification, and logic

## 📷 Future Improvements

- Improve Object Detection Speed and Accuracy
- Web dashboard for remote monitoring
- Battery backup & solar integration
- Simpler Hardware Design

## 💡 Inspiration

Built as part of a smart agricultural automation concept to help farmers streamline produce handling with minimal human supervision.
