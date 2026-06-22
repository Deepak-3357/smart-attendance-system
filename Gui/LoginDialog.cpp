#include "LoginDialog.h"
#include "RegisterUserDialog.h"
#include "ui_LoginDialog.h"
#include <QMessageBox>

LoginDialog::LoginDialog(QWidget* p)
    : QDialog(p), ui(new Ui::LoginDialog), userManager_(), auth_(&userManager_) {
    ui->setupUi(this);
    ui->usernameEdit->setText("admin");
    connect(ui->loginButton, &QPushButton::clicked, this, &LoginDialog::attemptLogin);
    connect(ui->registerButton, &QPushButton::clicked, this, &LoginDialog::openRegistration);
    connect(ui->passwordEdit, &QLineEdit::returnPressed, this, &LoginDialog::attemptLogin);
}

LoginDialog::~LoginDialog(){delete ui;}

void LoginDialog::attemptLogin(){
    QString error;
    if(auth_.login(ui->usernameEdit->text(), ui->passwordEdit->text(), &currentUser_, &error)){
        accept();
        return;
    }

    ui->errorLabel->setText(error);
    QMessageBox::warning(this, "Login failed", error);
}

void LoginDialog::openRegistration(){
    RegisterUserDialog dialog(&userManager_, this);
    if(dialog.exec() == QDialog::Accepted && !dialog.createdUsername().isEmpty()){
        ui->usernameEdit->setText(dialog.createdUsername());
        ui->passwordEdit->clear();
        ui->passwordEdit->setFocus();
        ui->errorLabel->setText("Account created. Sign in with the new credentials.");
    }
}
