#pragma once

#include <QWidget>

class UserManager;
namespace Ui { class UserManagementPage; }

class UserManagementPage : public QWidget {
    Q_OBJECT

public:
    explicit UserManagementPage(UserManager* userManager, QWidget* parent = nullptr);
    ~UserManagementPage() override;

public slots:
    void refresh();

signals:
    void usersChanged();

private slots:
    void addUser();
    void editUser();
    void deleteUser();
    void resetPassword();

private:
    QString selectedUsername() const;
    void showWarning(const QString& message);

    Ui::UserManagementPage* ui;
    UserManager* userManager_;
};
