#pragma once
#include <QWidget>
class StudentManager;class QStandardItemModel;namespace Ui{class StudentManagementPage;}
class StudentManagementPage:public QWidget{
 Q_OBJECT
public:explicit StudentManagementPage(StudentManager* manager,QWidget* parent=nullptr);~StudentManagementPage()override;
signals:void studentsChanged();
public slots:void refresh();
private slots:void addStudent();void saveStudent();void deleteStudent();void filter(const QString& text);void selectionChanged();
private:void populate(const QString& query={});void showPhoto(const QString& regNo);Ui::StudentManagementPage* ui;StudentManager* manager_;QStandardItemModel* model_;QString selectedReg_;};
