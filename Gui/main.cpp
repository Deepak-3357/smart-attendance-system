#include <QApplication>
#include <QCoreApplication>
#include <QDir>
#include <QMessageBox>
#include <QPainter>
#include <QPixmap>
#include <QSplashScreen>

#include "LoginDialog.h"
#include "MainWindow.h"

int main(int argc, char* argv[]) {
    QApplication app(argc, argv);
    QCoreApplication::setOrganizationName("SmartAttendance");
    QCoreApplication::setApplicationName("Smart Attendance System");
    QDir::setCurrent(QString::fromUtf8(SMART_ATTENDANCE_DATA_DIR));

    for (const char* directory : {"data", "faces", "trainer", "reports", "unknown", "outbox", "logs"}) {
        QDir().mkpath(directory);
    }

    QPixmap image(620, 330);
    image.fill(QColor("#0f172a"));
    QPainter painter(&image);
    painter.setPen(QColor("#60a5fa"));
    painter.setFont(QFont("Segoe UI", 28, QFont::Bold));
    painter.drawText(image.rect().adjusted(0, -25, 0, 0), Qt::AlignCenter, "SMART ATTENDANCE");
    painter.setPen(QColor("#cbd5e1"));
    painter.setFont(QFont("Segoe UI", 11));
    painter.drawText(image.rect().adjusted(0, 55, 0, 0), Qt::AlignCenter, "Secure • Intelligent • Academic");

    QSplashScreen splash(image);
    splash.show();
    splash.showMessage("Loading attendance services…", Qt::AlignBottom | Qt::AlignHCenter, QColor("#94a3b8"));
    app.processEvents();

    bool showSplash = true;
    while (true) {
        LoginDialog login;
        if (showSplash) {
            splash.finish(&login);
            showSplash = false;
        }

        if (login.exec() != QDialog::Accepted) {
            return 0;
        }

        bool signedOut = false;
        MainWindow window(login.currentUser());
        QObject::connect(&window, &MainWindow::signedOut, &app, [&]() {
            signedOut = true;
            app.quit();
        });
        window.show();

        const int exitCode = app.exec();
        if (!signedOut) {
            return exitCode;
        }
    }
}
