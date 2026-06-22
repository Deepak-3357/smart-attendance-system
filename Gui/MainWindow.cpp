#include "MainWindow.h"
#include "ui_MainWindow.h"
#include "DashboardPage.h"
#include "StudentManagementPage.h"
#include "DatasetCollectionPage.h"
#include "TrainingPage.h"
#include "AttendancePage.h"
#include "AnalyticsPage.h"
#include "ReportsPage.h"
#include "SettingsPage.h"
#include "ThemeManager.h"
#include "UserManagementPage.h"
#include "../Auth/UserManager.h"
#include "../Student.h"
#include "../Attendance.h"
#include "../Managers/CameraManager.h"
#include "../Managers/FaceRecognitionManager.h"
#include "../Recognition/RecognitionUtils.h"

#include <QApplication>
#include <QCloseEvent>
#include <QDateTime>
#include <QDir>
#include <QFile>
#include <QListWidgetItem>
#include <QMessageBox>
#include <QSettings>
#include <QTimer>

namespace {
const QStringList kAllRoles = {"Administrator", "Faculty", "Viewer"};
const QStringList kAdminOnly = {"Administrator"};
const QStringList kFacultyAndAdmin = {"Administrator", "Faculty"};
const QStringList kReadOnlyRoles = {"Administrator", "Faculty", "Viewer"};
}

MainWindow::MainWindow(const CurrentUser& user, QWidget* parent)
    : QMainWindow(parent),
      ui(new Ui::MainWindow),
      currentUser_(user),
      students_(std::make_unique<StudentManager>("students.csv")),
      attendance_(std::make_unique<AttendanceManager>("attendance.csv", "reports")),
      authUsers_(std::make_unique<UserManager>()) {
    ui->setupUi(this);

    themes_ = new ThemeManager(qApp);
    recognition_ = new FaceRecognitionManager(this);
    recognition_->loadModel();
    camera_ = new CameraManager(students_.get(), attendance_.get(), recognition_, this);
    RecognitionUtils::setConfidenceThreshold(QSettings().value("recognition/threshold", 45.0).toDouble());

    dashboard_ = new DashboardPage(students_.get(), attendance_.get());
    studentPage_ = new StudentManagementPage(students_.get());
    datasetPage_ = new DatasetCollectionPage(students_.get(), camera_);
    trainingPage_ = new TrainingPage(recognition_);
    attendancePage_ = new AttendancePage(camera_, recognition_);
    analyticsPage_ = new AnalyticsPage(students_.get(), attendance_.get());
    reportsPage_ = new ReportsPage(attendance_.get());
    userManagementPage_ = new UserManagementPage(authUsers_.get());
    settingsPage_ = new SettingsPage(themes_);

    dashboard_->setCurrentUser(currentUser_);

    const QStringList items = {
        "⌂  Dashboard",
        "♟  Students",
        "▣  Face Dataset",
        "⚙  Training",
        "◎  Attendance / Recognition",
        "◫  Analytics",
        "▤  Reports",
        "👥  User Management",
        "⚙  Settings"
    };
    const QList<QWidget*> pageWidgets = {
        dashboard_,
        studentPage_,
        datasetPage_,
        trainingPage_,
        attendancePage_,
        analyticsPage_,
        reportsPage_,
        userManagementPage_,
        settingsPage_
    };
    allowedRolesByRow_ = {
        kAllRoles,
        kAdminOnly,
        kAdminOnly,
        kAdminOnly,
        kFacultyAndAdmin,
        kReadOnlyRoles,
        kReadOnlyRoles,
        kAdminOnly,
        kAdminOnly
    };

    ui->navigation->addItems(items);
    ui->navigation->setSpacing(5);
    for (QWidget* page : pageWidgets) {
        ui->pages->addWidget(page);
    }

    for (int row = 0; row < allowedRolesByRow_.size(); ++row) {
        if (auto* item = ui->navigation->item(row); item && !isRoleAllowed(row)) {
            item->setHidden(true);
        }
    }

    connect(ui->navigation, &QListWidget::currentRowChanged, this, &MainWindow::navigate);
    connect(ui->logoutButton, &QPushButton::clicked, this, &MainWindow::logout);
    connect(dashboard_, &DashboardPage::logoutRequested, this, &MainWindow::logout);
    connect(studentPage_, &StudentManagementPage::studentsChanged, this, &MainWindow::refreshData);
    connect(studentPage_, &StudentManagementPage::studentsChanged, datasetPage_, &DatasetCollectionPage::refreshStudents);
    connect(attendancePage_, &AttendancePage::attendanceChanged, this, &MainWindow::refreshData);
    connect(userManagementPage_, &UserManagementPage::usersChanged, userManagementPage_, &UserManagementPage::refresh);

    QTimer* clock = new QTimer(this);
    connect(clock, &QTimer::timeout, this, &MainWindow::updateClock);
    clock->start(1000);

    ui->navigation->setCurrentRow(0);
    updateClock();
}

MainWindow::~MainWindow() {
    delete ui;
}

void MainWindow::navigate(int row) {
    if (row < 0) {
        return;
    }
    if (!isRoleAllowed(row)) {
        QMessageBox::warning(this, "Access denied", "Your role does not have permission to open this module.");
        ui->navigation->setCurrentRow(0);
        return;
    }
    if (camera_->isRunning()) {
        camera_->stop();
    }
    ui->pages->setCurrentIndex(row);
    refreshData();
}

void MainWindow::updateClock() {
    statusBar()->showMessage(QString("%1 (%2) session   •   %3")
                                 .arg(currentUser_.fullName, currentUser_.role,
                                      QDateTime::currentDateTime().toString("dddd, dd MMMM yyyy  •  hh:mm:ss AP")));
}

void MainWindow::refreshData() {
    dashboard_->refresh();
    analyticsPage_->refresh();
    reportsPage_->refresh();
}

void MainWindow::logout() {
    if (camera_->isRunning()) {
        camera_->stop();
    }
    CurrentUser::clearCurrent();
    emit signedOut();
    close();
}

bool MainWindow::isRoleAllowed(int row) const {
    if (row < 0 || row >= allowedRolesByRow_.size()) {
        return false;
    }
    return allowedRolesByRow_[row].contains(currentUser_.role);
}

void MainWindow::closeEvent(QCloseEvent* event) {
    camera_->stop();
    QSettings settings;
    if (settings.value("backup/automatic", false).toBool()) {
        QDir target(settings.value("backup/folder").toString());
        if (target.exists()) {
            const QString stamp = QDateTime::currentDateTime().toString("yyyyMMdd_hhmmss");
            QFile::copy("students.csv", target.filePath("students_" + stamp + ".csv"));
            QFile::copy("attendance.csv", target.filePath("attendance_" + stamp + ".csv"));
        }
    }
    event->accept();
}
