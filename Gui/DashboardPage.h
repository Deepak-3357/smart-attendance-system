#pragma once

#include <QWidget>
#include "../Auth/CurrentUser.h"

class StudentManager;
class AttendanceManager;
namespace Ui { class DashboardPage; }

class DashboardPage : public QWidget {
    Q_OBJECT

public:
    DashboardPage(StudentManager* students, AttendanceManager* attendance, QWidget* parent = nullptr);
    ~DashboardPage() override;

    void setCurrentUser(const CurrentUser& user);

public slots:
    void refresh();

signals:
    void logoutRequested();

private:
    Ui::DashboardPage* ui;
    StudentManager* students_;
    AttendanceManager* attendance_;
};
