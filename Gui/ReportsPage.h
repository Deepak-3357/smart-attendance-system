#pragma once
#include <QWidget>
class AttendanceManager;class QStandardItemModel;namespace Ui{class ReportsPage;}
class ReportsPage:public QWidget{
 Q_OBJECT
public:explicit ReportsPage(AttendanceManager*,QWidget* parent=nullptr);~ReportsPage()override;
public slots:void refresh();
private slots:void exportCsv();void exportPdf();void printReport();
private:void render(class QPrinter*);Ui::ReportsPage* ui;AttendanceManager* attendance_;QStandardItemModel* model_;};
