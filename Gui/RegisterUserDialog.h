#pragma once

#include <QDialog>

class UserManager;
namespace Ui { class RegisterUserDialog; }

class RegisterUserDialog : public QDialog {
    Q_OBJECT

public:
    explicit RegisterUserDialog(UserManager* userManager, QWidget* parent = nullptr);
    ~RegisterUserDialog() override;

    QString createdUsername() const { return createdUsername_; }

private slots:
    void createAccount();

private:
    Ui::RegisterUserDialog* ui;
    UserManager* userManager_;
    QString createdUsername_;
};
