#pragma once

#include "Attendance.h"
#include "Student.h"

bool captureFaceDataset(const StudentManager& studentManager);
bool trainModel();
bool recognizeFaces(const StudentManager& studentManager, AttendanceManager& attendanceManager);
