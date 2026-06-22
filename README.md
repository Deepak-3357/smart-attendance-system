# AI Powered Smart Attendance Management System

Industry-style C++ Smart Attendance Management System using OpenCV 4.12.0, OpenCV Contrib, Haar Cascade face detection, and LBPH face recognition.

## Features

- Student registration with duplicate prevention.
- CSV based student and attendance storage.
- Webcam face dataset capture with 100 samples per student.
- LBPH model training from the `faces` folder.
- Real-time webcam recognition with unknown face thresholding.
- Dynamic confidence threshold tuning and camera selection.
- Blink-based liveness verification using Haar eye cascade.
- Unknown face logging in the `unknown` folder.
- Automatic attendance marking with same-day duplicate prevention.
- Daily report export in `reports/DD-MM-YYYY.csv`.
- Attendance analytics with present days, absent days, and percentage.
- CSV export to `attendance_report.csv`.
- Email draft generation in `outbox`.
- Student profile preview with latest dataset image.
- Student search by register number or name.

## Expected OpenCV Paths

The Visual Studio project is configured for:

- Include: `C:\opencv_build\install\include`
- Library: `C:\opencv_build\lib\Release`
- Libraries:
  - `opencv_core4120.lib`
  - `opencv_imgproc4120.lib`
  - `opencv_highgui4120.lib`
  - `opencv_videoio4120.lib`
  - `opencv_objdetect4120.lib`
  - `opencv_face4120.lib`
  - `opencv_imgcodecs4120.lib`

At runtime, make sure the matching OpenCV DLL folder is available in `PATH`.

## Required Runtime File

The project includes:

- `haarcascade_frontalface_default.xml`
- `haarcascade_eye.xml`

Keep both files in the project working directory. Visual Studio is configured with `$(ProjectDir)` as the debugger working directory.

## Admin Login

Default login:

```text
Username: admin
Password: admin123
```

On first run, the app creates `admin_credentials.dat` and stores a salted password hash instead of plain text credentials.

## Dataset Format

Captured images are saved as:

```text
faces/192511008.1.jpg
faces/192511008.2.jpg
...
faces/192511008.100.jpg
```

The numeric part before the first dot is used as the LBPH label, so register numbers must be numeric and fit within a 32-bit integer.

## CSV Formats

`students.csv`

```csv
RegNo,Name
192511008,Deepak
```

`attendance.csv`

```csv
Date,Time,RegNo,Name,Status
07-06-2026,09:30,192511008,Deepak,Present
```

## Visual Studio Build

1. Open `SmartAttendanceSystem.sln`.
2. Select `x64` and `Release`.
3. Confirm OpenCV include/library paths match your machine.
4. Build and run.

## Qt 6 Desktop Application

The Qt Widgets desktop application is implemented in `Gui/`, `Managers/`, and
`ui/`. It reuses the existing CSV files, dataset folder, trained LBPH model,
Haar cascades, and liveness logic without changing the data formats.

Build with Qt 6.11.1, OpenCV 4.12.0, CMake, Ninja, and the MSVC developer
environment:

```powershell
cmake -S . -B build-gui-ninja -G Ninja `
  -DCMAKE_BUILD_TYPE=Release `
  -DCMAKE_MAKE_PROGRAM=C:\Qt\Tools\Ninja\ninja.exe `
  -DCMAKE_PREFIX_PATH=C:\Qt\6.11.1\msvc2022_64 `
  -DOpenCV_DIR=C:\opencv_build
cmake --build build-gui-ninja --parallel
cmake --install build-gui-ninja --prefix "C:\Users\deepa\OneDrive\Documents\SmartAttendanceSystem\dist-gui"
```

Run `build-gui-ninja/SmartAttendanceGUI.exe` for development, or run the
deployed copy from `dist-gui`. The default credentials remain `admin` /
`admin123`. Choose Administrator for full access or User for a limited session
after signing in.

## Menu

```text
1. Register Student
2. Capture Face Dataset
3. Train Model
4. Recognize Face
5. View Students
6. View Attendance
7. Analytics
8. Search Student
9. View Date Wise Reports
10. Export Attendance Report
11. Real-Time Dashboard
12. View Student Profile
13. Email Attendance Report
14. Tune Recognition Threshold
15. Exit
```

## Email Reports

The app creates `email_config.csv` on first run. Replace `change-me@example.com` with the target email address. The email service generates a `.eml` draft in `outbox` containing the exported attendance report, ready for a mail client or SMTP gateway.
