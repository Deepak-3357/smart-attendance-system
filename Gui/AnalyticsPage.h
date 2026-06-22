#pragma once
#include <QWidget>
class StudentManager;class AttendanceManager;class QStandardItemModel;class QChartView;namespace Ui{class AnalyticsPage;}
class AnalyticsPage:public QWidget{
 Q_OBJECT
public:AnalyticsPage(StudentManager*,AttendanceManager*,QWidget* parent=nullptr);~AnalyticsPage()override;
public slots:void refresh();
private:void applyFilter();Ui::AnalyticsPage* ui;StudentManager* students_;AttendanceManager* attendance_;QStandardItemModel* model_;QChartView* chartView_;};
