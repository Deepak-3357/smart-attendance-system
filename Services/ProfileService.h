#pragma once

#include "../Attendance.h"
#include "../Student.h"

class ProfileService
{
public:
    ProfileService(const StudentManager& studentManager, const AttendanceManager& attendanceManager);

    void viewStudentProfile() const;

private:
    const StudentManager& studentManager_;
    const AttendanceManager& attendanceManager_;
};
