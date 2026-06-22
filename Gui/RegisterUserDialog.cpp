#include "RegisterUserDialog.h"
#include "ui_RegisterUserDialog.h"
#include "../Auth/UserManager.h"

#include <QMessageBox>

RegisterUserDialog::RegisterUserDialog(UserManager* userManager, QWidget* parent)
    : QDialog(parent), ui(new Ui::RegisterUserDialog), userManager_(userManager) {
    ui->setupUi(this);
    ui->roleCombo->addItems(UserManager::availableRoles());

    connect(ui->createButton, &QPushButton::clicked, this, &RegisterUserDialog::createAccount);
    connect(ui->cancelButton, &QPushButton::clicked, this, &QDialog::reject);
    connect(ui->confirmPasswordEdit, &QLineEdit::returnPressed, this, &RegisterUserDialog::createAccount);
}

RegisterUserDialog::~RegisterUserDialog() {
    delete ui;
}

void RegisterUserDialog::createAccount() {
    if (ui->passwordEdit->text() != ui->confirmPasswordEdit->text()) {
        QMessageBox::warning(this, "Password mismatch", "Passwords must match.");
        return;
    }

    QString error;
    if (!userManager_->createUser(ui->fullNameEdit->text(),
                                  ui->usernameEdit->text(),
                                  ui->passwordEdit->text(),
                                  ui->roleCombo->currentText(),
                                  &error)) {
        QMessageBox::warning(this, "Account not created", error);
        return;
    }

    createdUsername_ = ui->usernameEdit->text().trimmed().toLower();
    QMessageBox::information(this, "Account created", "The new user account was created successfully.");
    accept();
}
