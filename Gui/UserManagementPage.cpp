#include "UserManagementPage.h"
#include "RegisterUserDialog.h"
#include "ui_UserManagementPage.h"
#include "../Auth/UserManager.h"

#include <QComboBox>
#include <QDialog>
#include <QDialogButtonBox>
#include <QFormLayout>
#include <QHeaderView>
#include <QInputDialog>
#include <QLineEdit>
#include <QMessageBox>
#include <QPushButton>

UserManagementPage::UserManagementPage(UserManager* userManager, QWidget* parent)
    : QWidget(parent), ui(new Ui::UserManagementPage), userManager_(userManager) {
    ui->setupUi(this);
    ui->userTable->setColumnCount(3);
    ui->userTable->setHorizontalHeaderLabels({"Full Name", "Username", "Role"});
    ui->userTable->horizontalHeader()->setStretchLastSection(true);
    ui->userTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->userTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->userTable->setSelectionMode(QAbstractItemView::SingleSelection);
    ui->userTable->setEditTriggers(QAbstractItemView::NoEditTriggers);

    connect(ui->addButton, &QPushButton::clicked, this, &UserManagementPage::addUser);
    connect(ui->editButton, &QPushButton::clicked, this, &UserManagementPage::editUser);
    connect(ui->deleteButton, &QPushButton::clicked, this, &UserManagementPage::deleteUser);
    connect(ui->resetPasswordButton, &QPushButton::clicked, this, &UserManagementPage::resetPassword);

    refresh();
}

UserManagementPage::~UserManagementPage() {
    delete ui;
}

void UserManagementPage::refresh() {
    ui->userTable->setRowCount(0);
    const auto users = userManager_->allUsers();
    ui->userTable->setRowCount(static_cast<int>(users.size()));
    for (int row = 0; row < static_cast<int>(users.size()); ++row) {
        const auto& user = users[static_cast<size_t>(row)];
        ui->userTable->setItem(row, 0, new QTableWidgetItem(user.fullName));
        ui->userTable->setItem(row, 1, new QTableWidgetItem(user.username));
        ui->userTable->setItem(row, 2, new QTableWidgetItem(user.role));
    }
}

void UserManagementPage::addUser() {
    RegisterUserDialog dialog(userManager_, this);
    if (dialog.exec() == QDialog::Accepted) {
        refresh();
        emit usersChanged();
    }
}

void UserManagementPage::editUser() {
    const QString username = selectedUsername();
    if (username.isEmpty()) {
        showWarning("Please select a user to edit.");
        return;
    }

    const auto user = userManager_->findUser(username);
    if (!user.has_value()) {
        showWarning("Selected user was not found.");
        refresh();
        return;
    }

    QDialog dialog(this);
    dialog.setWindowTitle("Edit User");
    QFormLayout layout(&dialog);
    QLineEdit fullNameEdit(user->fullName, &dialog);
    QLineEdit usernameEdit(user->username, &dialog);
    usernameEdit.setReadOnly(true);
    QComboBox roleCombo(&dialog);
    roleCombo.addItems(UserManager::availableRoles());
    roleCombo.setCurrentText(user->role);
    QDialogButtonBox buttons(QDialogButtonBox::Save | QDialogButtonBox::Cancel, &dialog);

    layout.addRow("Full Name", &fullNameEdit);
    layout.addRow("Username", &usernameEdit);
    layout.addRow("Role", &roleCombo);
    layout.addWidget(&buttons);

    connect(&buttons, &QDialogButtonBox::accepted, &dialog, &QDialog::accept);
    connect(&buttons, &QDialogButtonBox::rejected, &dialog, &QDialog::reject);

    if (dialog.exec() != QDialog::Accepted) {
        return;
    }

    QString error;
    if (!userManager_->updateUser(username, fullNameEdit.text(), roleCombo.currentText(), &error)) {
        showWarning(error);
        return;
    }

    refresh();
    emit usersChanged();
}

void UserManagementPage::deleteUser() {
    const QString username = selectedUsername();
    if (username.isEmpty()) {
        showWarning("Please select a user to delete.");
        return;
    }

    if (QMessageBox::question(this,
                              "Delete user",
                              QString("Delete user \"%1\"? This cannot be undone.").arg(username))
        != QMessageBox::Yes) {
        return;
    }

    QString error;
    if (!userManager_->deleteUser(username, &error)) {
        showWarning(error);
        return;
    }

    refresh();
    emit usersChanged();
}

void UserManagementPage::resetPassword() {
    const QString username = selectedUsername();
    if (username.isEmpty()) {
        showWarning("Please select a user to reset.");
        return;
    }

    bool ok = false;
    const QString newPassword = QInputDialog::getText(this,
                                                      "Reset Password",
                                                      QString("New password for %1:").arg(username),
                                                      QLineEdit::Password,
                                                      QString(),
                                                      &ok);
    if (!ok) {
        return;
    }

    QString error;
    if (!userManager_->resetPassword(username, newPassword, &error)) {
        showWarning(error);
        return;
    }

    QMessageBox::information(this, "Password reset", "The password was reset successfully.");
    emit usersChanged();
}

QString UserManagementPage::selectedUsername() const {
    const auto selectedRows = ui->userTable->selectionModel()->selectedRows();
    if (selectedRows.isEmpty()) {
        return {};
    }
    const int row = selectedRows.first().row();
    const auto* usernameItem = ui->userTable->item(row, 1);
    return usernameItem ? usernameItem->text() : QString{};
}

void UserManagementPage::showWarning(const QString& message) {
    QMessageBox::warning(this, "User Management", message);
}
