#pragma once

#include "../Auth/CurrentUser.h"

#include <QMainWindow>
#include <QStringList>
#include <QVector>
#include <memory>

class StudentManager;
class AttendanceManager;
class FaceRecognitionManager;
class CameraManager;
class ThemeManager;
class DashboardPage;
class StudentManagementPage;
class DatasetCollectionPage;
class TrainingPage;
class AttendancePage;
class AnalyticsPage;
class ReportsPage;
class SettingsPage;
class UserManagementPage;
class UserManager;
namespace Ui { class MainWindow; }

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit MainWindow(const CurrentUser& user, QWidget* parent = nullptr);
    ~MainWindow() override;

signals:
    void signedOut();

protected:
    void closeEvent(QCloseEvent*) override;

private slots:
    void navigate(int row);
    void updateClock();
    void refreshData();
    void logout();

private:
    bool isRoleAllowed(int row) const;

    Ui::MainWindow* ui;
    CurrentUser currentUser_;
    std::unique_ptr<StudentManager> students_;
    std::unique_ptr<AttendanceManager> attendance_;
    std::unique_ptr<UserManager> authUsers_;
    FaceRecognitionManager* recognition_;
    CameraManager* camera_;
    ThemeManager* themes_;
    DashboardPage* dashboard_;
    StudentManagementPage* studentPage_;
    DatasetCollectionPage* datasetPage_;
    TrainingPage* trainingPage_;
    AttendancePage* attendancePage_;
    AnalyticsPage* analyticsPage_;
    ReportsPage* reportsPage_;
    UserManagementPage* userManagementPage_;
    SettingsPage* settingsPage_;
    QVector<QStringList> allowedRolesByRow_;
};
