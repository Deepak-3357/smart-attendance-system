#include "DashboardPage.h"
#include "ui_DashboardPage.h"
#include "../Student.h"
#include "../Attendance.h"

#include <QPushButton>
#include <QSettings>

DashboardPage::DashboardPage(StudentManager* students, AttendanceManager* attendance, QWidget* parent)
    : QWidget(parent), ui(new Ui::DashboardPage), students_(students), attendance_(attendance) {
    ui->setupUi(this);
    ui->card->setObjectName("card");
    ui->attendanceCard->setObjectName("card");
    ui->accuracyCard->setObjectName("card");
    ui->defaultersCard->setObjectName("card");
    ui->activityCard->setObjectName("card");
    ui->sessionCard->setObjectName("card");

    connect(ui->logoutButton, &QPushButton::clicked, this, &DashboardPage::logoutRequested);
    refresh();
}

DashboardPage::~DashboardPage() {
    delete ui;
}

void DashboardPage::setCurrentUser(const CurrentUser& user) {
    ui->loggedUserValue->setText(user.fullName);
    ui->roleValue->setText(user.role);
}

void DashboardPage::refresh() {
    const auto students = students_->getAllStudents();
    const auto records = attendance_->getAllRecords();

    ui->studentsValue->setText(QString::number(students.size()));
    ui->attendanceValue->setText(QString::number(attendance_->countPresentToday()));

    int defaulters = 0;
    const int workingDays = attendance_->countWorkingDays();
    for (const auto& student : students) {
        const double percentage = workingDays ? 100.0 * attendance_->countPresentDays(student.regNo) / workingDays : 100.0;
        if (percentage < 75.0) {
            ++defaulters;
        }
    }

    ui->defaultersValue->setText(QString::number(defaulters));
    const QVariant accuracy = QSettings().value("recognition/lastAccuracy");
    ui->accuracyValue->setText(accuracy.isValid() ? QString("%1%").arg(accuracy.toInt()) : "-");

    if (!records.empty()) {
        const auto& record = records.back();
        ui->activityValue->setText(QString::fromStdString(record.name + " (" + record.regNo + ") - " + record.date + " " + record.time));
    } else {
        ui->activityValue->setText("No attendance activity yet.");
    }
}
