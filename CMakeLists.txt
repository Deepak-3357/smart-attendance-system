cmake_minimum_required(VERSION 3.21)
project(SmartAttendanceSystem VERSION 2.0 LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC_SEARCH_PATHS "${CMAKE_CURRENT_SOURCE_DIR}/ui")
find_package(Qt6 6.5 REQUIRED COMPONENTS Widgets Charts PrintSupport Concurrent)
find_package(OpenCV 4 REQUIRED COMPONENTS core imgproc imgcodecs videoio objdetect face)

qt_add_executable(SmartAttendanceGUI WIN32
 Gui/main.cpp Gui/MainWindow.cpp Gui/MainWindow.h ui/MainWindow.ui
 Gui/LoginDialog.cpp Gui/LoginDialog.h ui/LoginDialog.ui
 Gui/RegisterUserDialog.cpp Gui/RegisterUserDialog.h ui/RegisterUserDialog.ui
 Gui/DashboardPage.cpp Gui/DashboardPage.h ui/DashboardPage.ui
 Gui/StudentManagementPage.cpp Gui/StudentManagementPage.h ui/StudentManagementPage.ui
 Gui/DatasetCollectionPage.cpp Gui/DatasetCollectionPage.h ui/DatasetCollectionPage.ui
 Gui/TrainingPage.cpp Gui/TrainingPage.h ui/TrainingPage.ui
 Gui/AttendancePage.cpp Gui/AttendancePage.h ui/AttendancePage.ui
 Gui/AnalyticsPage.cpp Gui/AnalyticsPage.h ui/AnalyticsPage.ui
 Gui/ReportsPage.cpp Gui/ReportsPage.h ui/ReportsPage.ui
 Gui/UserManagementPage.cpp Gui/UserManagementPage.h ui/UserManagementPage.ui
 Gui/SettingsPage.cpp Gui/SettingsPage.h ui/SettingsPage.ui
 Gui/ThemeManager.cpp Gui/ThemeManager.h Gui/NotificationWidget.cpp Gui/NotificationWidget.h
 Managers/CameraManager.cpp Managers/CameraManager.h
 Managers/FaceRecognitionManager.cpp Managers/FaceRecognitionManager.h Managers/AttendanceManager.h
 Auth/UserManager.cpp Auth/UserManager.h Auth/CurrentUser.h
 Student.cpp Student.h CsvUtils.h Attendance.cpp Attendance.h
 Recognition/LivenessDetector.cpp Recognition/LivenessDetector.h
 Recognition/RecognitionUtils.cpp Recognition/RecognitionUtils.h
 Services/AuthenticationService.cpp Services/AuthenticationService.h
 Services/AuditLogger.cpp Services/AuditLogger.h)
target_include_directories(SmartAttendanceGUI PRIVATE ${CMAKE_CURRENT_SOURCE_DIR} ${OpenCV_INCLUDE_DIRS})
target_link_libraries(SmartAttendanceGUI PRIVATE Qt6::Widgets Qt6::Charts Qt6::PrintSupport Qt6::Concurrent ${OpenCV_LIBS})
target_compile_definitions(SmartAttendanceGUI PRIVATE NOMINMAX _CRT_SECURE_NO_WARNINGS SMART_ATTENDANCE_DATA_DIR="${CMAKE_CURRENT_SOURCE_DIR}")
add_custom_command(TARGET SmartAttendanceGUI POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy_if_different
 "${CMAKE_CURRENT_SOURCE_DIR}/haarcascade_frontalface_default.xml"
 "${CMAKE_CURRENT_SOURCE_DIR}/haarcascade_eye.xml" "$<TARGET_FILE_DIR:SmartAttendanceGUI>")
install(TARGETS SmartAttendanceGUI RUNTIME DESTINATION .)
install(FILES haarcascade_frontalface_default.xml haarcascade_eye.xml DESTINATION .)
file(GLOB OPENCV_RUNTIME_DLLS "C:/opencv_build/bin/Release/opencv_*.dll")
foreach(OPENCV_DLL IN LISTS OPENCV_RUNTIME_DLLS)
 add_custom_command(TARGET SmartAttendanceGUI POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy_if_different "${OPENCV_DLL}" "$<TARGET_FILE_DIR:SmartAttendanceGUI>")
endforeach()
qt_generate_deploy_app_script(TARGET SmartAttendanceGUI OUTPUT_SCRIPT qt_deploy_script NO_UNSUPPORTED_PLATFORM_ERROR)
install(SCRIPT ${qt_deploy_script})
