# Smart Attendance Management System

## Team Members

- Deepak R (192511008)
- Emad Mohammed Abdullah Farooqui (192511019)

---

## My Contributions (Deepak R)

I was responsible for the complete development and implementation of **Module 2 – Face Dataset Collection Module** and jointly contributed to **Module 3 – Face Recognition Model Training (LBPH)**.

### Key Contributions

#### Module 2 – Face Dataset Collection Module (100% Contribution)

- Designed and developed the face dataset collection workflow.
- Implemented camera integration using OpenCV.
- Developed automatic face detection and image capture functionality.
- Created student-wise dataset organization and storage mechanisms.
- Implemented image preprocessing and grayscale conversion.
- Developed progress tracking and dataset collection monitoring.

#### Module 3 – Face Recognition Model Training (Shared Contribution)

- Assisted in implementing the LBPH Face Recognition algorithm.
- Contributed to dataset preparation and model training processes.
- Participated in training validation and accuracy evaluation.
- Assisted in integrating the trained model into the attendance system.
- Contributed to testing and optimization of recognition performance.

---

## Technologies Used

- C++
- OpenCV
- Qt Framework
- CSV Data Management
- LBPH Face Recognition
- CMake
- Visual Studio

---

## Features

- Secure User Authentication
- Student Management System
- Face Dataset Collection
- LBPH Face Recognition Training
- Real-Time Attendance Recognition
- Attendance Analytics Dashboard
- Attendance Report Generation
- User Management
- Application Settings
- CSV-Based Data Storage

---

## Project Overview

Smart Attendance Management System is an AI-powered attendance solution that automates student attendance using facial recognition technology.

The system captures student face datasets, trains an LBPH face recognition model, and performs real-time attendance marking through webcam-based recognition. The application also provides attendance analytics, reporting tools, and administrative controls through an integrated graphical user interface.

---

## Screenshots

### Splash Screen
![Splash Screen](smart_attendance_splash_screen.png)

### Secure Login Interface
![Login Screen](secure_login_interface.png)

### Attendance Dashboard
![Dashboard](attendance_dashboard_overview.png)

### Student Management Module
![Student Management](student_management_module.png)

### Face Dataset Collection Module
![Face Dataset](face_dataset_collection_module.png)

### LBPH Model Training Module
![Model Training](lbph_model_training_module.png)

### Real-Time Attendance Recognition
![Attendance Recognition](real_time_attendance_recognition.png)

### Attendance Analytics Dashboard
![Analytics](attendance_analytics_dashboard.png)

### Attendance Reports Module
![Reports](attendance_reports_module.png)

### User Management Module
![User Management](user_management_module.png)

### Application Settings Panel
![Settings](application_settings_panel.png)

---

## How to Run

### Prerequisites

- Visual Studio 2022
- OpenCV 4.x
- Qt Framework
- CMake

### Build the Project

Open the solution file:

```bash
SmartAttendanceSystem.sln
```

Build and run using Visual Studio.

---

## Expected Output

The application provides:

- Secure login authentication
- Student registration and management
- Face dataset collection using webcam
- LBPH model training
- Real-time face recognition
- Automatic attendance marking
- Attendance analytics and visualization
- Attendance report generation
- User and system management

---

## Project Structure

```text
SmartAttendanceSystem/
│
├── Auth/
├── Dashboard/
├── Gui/
├── Managers/
├── Recognition/
├── Services/
├── attendance/
├── data/
├── trainer/
├── ui/
│
├── Analytics.cpp
├── Attendance.cpp
├── CaptureFaces.cpp
├── RecognizeFace.cpp
├── TrainModel.cpp
├── SmartAttendanceSystem.cpp
│
├── students.csv
├── attendance.csv
├── README.md
└── SmartAttendanceSystem.sln
```

---

## Face Recognition Workflow

1. Register Student Information
2. Capture Face Dataset Images
3. Store Images in Dataset Folder
4. Train LBPH Recognition Model
5. Start Real-Time Recognition
6. Identify Student
7. Mark Attendance Automatically
8. Generate Reports and Analytics

---

## Academic Purpose

This project was developed as part of an academic capstone project to demonstrate the practical application of Computer Vision, Face Recognition, and Automated Attendance Management using C++, OpenCV, and Qt Framework.

---

## License

This project is intended for educational and academic purposes.
