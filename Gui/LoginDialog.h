#pragma once
#include <QDialog>
#include "../Auth/CurrentUser.h"
#include "../Auth/UserManager.h"
#include "../Services/AuthenticationService.h"

namespace Ui{class LoginDialog;}
class LoginDialog:public QDialog{
 Q_OBJECT
public:
 explicit LoginDialog(QWidget* parent=nullptr);
 ~LoginDialog()override;
 CurrentUser currentUser()const{return currentUser_;}
private slots:
 void attemptLogin();
 void openRegistration();
private:
 Ui::LoginDialog* ui;
 UserManager userManager_;
 AuthenticationService auth_;
 CurrentUser currentUser_;
};
