# 📷 ESP32-Wrover-Module with Camera

Welcome to the ESP32-Wrover-Module with Camera project! This project is designed to capture images using an ESP32 device equipped with a camera and send them to a server for analysis. The results are displayed on an LCD screen connected to the ESP32. This repository is private and intended for collaboration among classmates and viewing by teachers.

## 🚀 Features

- **Image Capture**: Utilizes the ESP32 camera module to capture images upon button press.
- **WiFi Connectivity**: Connects to a specified WiFi network to upload images.
- **Image Upload**: Sends captured images to a remote server for analysis.
- **LCD Display**: Displays the analysis results on a connected LCD screen.
- **JSON Response Handling**: Processes JSON responses from the server to categorize objects.

## 📂 Project Structure

- **Fetch.ino**: Main Arduino sketch for the ESP32, handling camera initialization, image capture, and server communication.
- **camera_pins.h**: Configuration file for camera pin definitions.
- **.gitignore**: Specifies files and directories to be ignored by git.

## 🛠️ Setup Instructions

1. **Hardware Requirements**:
   - ESP32-Wrover-Module with Camera
   - LCD Screen (I2C compatible)
   - Push Button
   - WiFi Network

2. **Clone the Repository**:
   ```bash
   git clone https://github.com/youpv/esp32-waste-sorter.git
   cd esp32-waste-sorter
   ```

3. **Configure WiFi Credentials**:
   - Open `Fetch.ino` and update the `ssid_Router` and `password_Router` variables with your WiFi credentials.

4. **Upload Code to ESP32**:
   - Use the Arduino IDE to upload `Fetch.ino` to your ESP32 device.

5. **Connect Hardware**:
   - Connect the camera module and LCD screen to the ESP32 as per the pin configuration in `camera_pins.h`.
   - Connect the push button to the specified GPIO pin.

6. **Run the Project**:
   - Power the ESP32 and press the button to capture and upload images.

7. **Working Model Pic**:
   ![WhatsApp Image 2024-11-15 at 13 32 41_4b8c0801](https://github.com/user-attachments/assets/97130a8c-a12e-4090-b89c-7852f851e4cf)


## 📡 API Endpoints

- **POST /api/upload**: Endpoint for uploading images. The ESP32 sends a multipart/form-data request with the image.

## 🤝 Contributing

- **Classmates**: Feel free to fork the repository and submit pull requests for improvements or bug fixes.
- **Teachers**: You can view the project and provide feedback.

## 📜 License

This project is for educational purposes and is not intended for commercial use.

## 📧 Contact

For any questions or issues, please contact Youp Verkooijen at [github.com/youpv](https://github.com/youpv).

---

Thank you for being a part of this project! 🎉
