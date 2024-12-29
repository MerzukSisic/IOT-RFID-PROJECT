# IoT RFID Employee Tracking System with Firebase

This project implements an IoT-based employee tracking system using RFID technology, a NodeMCU microcontroller, Firebase Realtime Database, and a web application for management. 

## Key Features
- **RFID Authentication**
- **Visual and Audio Indicators**
- **Firebase Integration**
- **Web Application**
- **SOS Mode**

## Technologies Used
- **Hardware:** NodeMCU ESP8266, RFID RC522, buzzer, LED indicators
- **Backend:** Firebase Realtime Database
- **Frontend:** HTML, CSS, JavaScript (Firebase SDK)
- **Communication:** Wi-Fi

## How It Works
1. NodeMCU scans an employee's RFID card and sends the data to Firebase.
2. The web application displays employee details in real-time.
3. LEDs and the buzzer provide feedback for successful or failed scans.
4. Employee data, including check-ins and check-outs, are logged in Firebase.

## Live Demo
The web application is live and accessible at: [IoT RFID Project](https://iot-project-f5585.firebaseapp.com/)

## Motivation
The project aims to provide a simple and efficient way to manage employee working hours and track their activities in workplaces.

## Installation
1. Clone this repository.
2. Configure Firebase settings in the code.
3. Deploy the web application to Firebase Hosting.
4. Set up hardware components as described in the documentation.

## License
This project is open-source and available under the [MIT License](LICENSE).
