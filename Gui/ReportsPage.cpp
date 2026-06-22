#include "ReportsPage.h"
#include "ui_ReportsPage.h"
#include "../Attendance.h"
#include "../CsvUtils.h"
#include <QStandardItemModel>
#include <QHeaderView>
#include <QFileDialog>
#include <QMessageBox>
#include <QTextStream>
#include <QPrinter>
#include <QPrintDialog>
#include <QPainter>
ReportsPage::ReportsPage(AttendanceManager*a,QWidget*p):QWidget(p),ui(new Ui::ReportsPage),attendance_(a),model_(new QStandardItemModel(this)){ui->setupUi(this);model_->setHorizontalHeaderLabels({"Date","Time","Reg No","Name","Status"});ui->reportTable->setModel(model_);ui->reportTable->horizontalHeader()->setStretchLastSection(true);ui->fromDate->setDate(QDate::currentDate().addMonths(-1));ui->toDate->setDate(QDate::currentDate());connect(ui->filterButton,&QPushButton::clicked,this,&ReportsPage::refresh);connect(ui->csvButton,&QPushButton::clicked,this,&ReportsPage::exportCsv);connect(ui->pdfButton,&QPushButton::clicked,this,&ReportsPage::exportPdf);connect(ui->printButton,&QPushButton::clicked,this,&ReportsPage::printReport);refresh();}
ReportsPage::~ReportsPage(){delete ui;}void ReportsPage::refresh(){model_->removeRows(0,model_->rowCount());for(const auto&r:attendance_->getAllRecords()){const QDate d=QDate::fromString(QString::fromStdString(r.date),"dd-MM-yyyy");if(d.isValid()&&(d<ui->fromDate->date()||d>ui->toDate->date()))continue;model_->appendRow({new QStandardItem(QString::fromStdString(r.date)),new QStandardItem(QString::fromStdString(r.time)),new QStandardItem(QString::fromStdString(r.regNo)),new QStandardItem(QString::fromStdString(r.name)),new QStandardItem(QString::fromStdString(r.status))});}}
void ReportsPage::exportCsv(){const QString path=QFileDialog::getSaveFileName(this,"Export attendance","attendance_report.csv","CSV files (*.csv)");if(path.isEmpty())return;QFile f(path);if(!f.open(QIODevice::WriteOnly|QIODevice::Text)){QMessageBox::critical(this,"Export failed",f.errorString());return;}QTextStream out(&f);out<<"Date,Time,RegNo,Name,Status\n";for(int r=0;r<model_->rowCount();++r){QStringList row;for(int c=0;c<model_->columnCount();++c){QString v=model_->item(r,c)->text();v.replace('"',"\"\"");row<<'"'+v+'"';}out<<row.join(',')<<'\n';}QMessageBox::information(this,"Export complete","CSV report saved successfully.");}
void ReportsPage::render(QPrinter* printer){QPainter painter(printer);const QRect page=printer->pageLayout().paintRectPixels(printer->resolution());int y=80;painter.setFont(QFont("Segoe UI",18,QFont::Bold));painter.drawText(60,y,"Smart Attendance Report");y+=50;painter.setFont(QFont("Segoe UI",9,QFont::Bold));const int widths[]={110,80,140,260,100};int x=60;for(int c=0;c<5;++c){painter.drawText(x,y,model_->headerData(c,Qt::Horizontal).toString());x+=widths[c];}y+=25;painter.setFont(QFont("Segoe UI",8));for(int r=0;r<model_->rowCount();++r){if(y>page.height()-70){printer->newPage();y=70;}x=60;for(int c=0;c<5;++c){painter.drawText(QRect(x,y-15,widths[c]-5,22),Qt::AlignLeft|Qt::AlignVCenter,model_->item(r,c)->text());x+=widths[c];}y+=24;}}
void ReportsPage::exportPdf(){const QString path=QFileDialog::getSaveFileName(this,"Export PDF","attendance_report.pdf","PDF files (*.pdf)");if(path.isEmpty())return;QPrinter printer(QPrinter::HighResolution);printer.setOutputFormat(QPrinter::PdfFormat);printer.setOutputFileName(path);render(&printer);QMessageBox::information(this,"Export complete","PDF report saved successfully.");}
void ReportsPage::printReport(){QPrinter printer(QPrinter::HighResolution);QPrintDialog dlg(&printer,this);if(dlg.exec()==QDialog::Accepted)render(&printer);}
