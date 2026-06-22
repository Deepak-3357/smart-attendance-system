#include "AnalyticsPage.h"
#include "ui_AnalyticsPage.h"
#include "../Student.h"
#include "../Attendance.h"
#include <QStandardItemModel>
#include <QHeaderView>
#include <QtCharts/QChartView>
#include <QtCharts/QBarSeries>
#include <QtCharts/QBarSet>
#include <QtCharts/QBarCategoryAxis>
#include <QtCharts/QValueAxis>
#include <map>
#include <set>
AnalyticsPage::AnalyticsPage(StudentManager*s,AttendanceManager*a,QWidget*p):QWidget(p),ui(new Ui::AnalyticsPage),students_(s),attendance_(a),model_(new QStandardItemModel(this)),chartView_(new QChartView(this)){ui->setupUi(this);model_->setHorizontalHeaderLabels({"Reg No","Student","Present","Absent","Attendance %","Status"});ui->analyticsTable->setModel(model_);ui->analyticsTable->horizontalHeader()->setStretchLastSection(true);ui->chartLayout->addWidget(chartView_);chartView_->setRenderHint(QPainter::Antialiasing);connect(ui->refreshButton,&QPushButton::clicked,this,&AnalyticsPage::refresh);connect(ui->searchEdit,&QLineEdit::textChanged,this,[this]{applyFilter();});connect(ui->statusFilter,&QComboBox::currentIndexChanged,this,[this]{applyFilter();});refresh();}
AnalyticsPage::~AnalyticsPage(){delete ui;}void AnalyticsPage::refresh(){model_->removeRows(0,model_->rowCount());const int working=attendance_->countWorkingDays();for(const auto&s:students_->getAllStudents()){int present=attendance_->countPresentDays(s.regNo),absent=working>present?working-present:0;double pct=working?100.0*present/working:100.0;model_->appendRow({new QStandardItem(QString::fromStdString(s.regNo)),new QStandardItem(QString::fromStdString(s.name)),new QStandardItem(QString::number(present)),new QStandardItem(QString::number(absent)),new QStandardItem(QString::number(pct,'f',1)),new QStandardItem(pct<75?"Defaulter":"Good Standing")});}
 std::map<std::string,std::set<std::string>> byDate;for(const auto&r:attendance_->getAllRecords())if(r.status=="Present")byDate[r.date].insert(r.regNo);auto*set=new QBarSet("Present");QStringList cats;int skip=std::max(0,static_cast<int>(byDate.size())-8),i=0;for(const auto&[date,ids]:byDate){if(i++<skip)continue;*set<<ids.size();cats<<QString::fromStdString(date);}auto*series=new QBarSeries;series->append(set);auto*chart=new QChart;chart->addSeries(series);chart->setTitle("Monthly / Recent Attendance Summary");auto*axisX=new QBarCategoryAxis;axisX->append(cats);chart->addAxis(axisX,Qt::AlignBottom);series->attachAxis(axisX);auto*axisY=new QValueAxis;axisY->setLabelFormat("%d");chart->addAxis(axisY,Qt::AlignLeft);series->attachAxis(axisY);chart->legend()->setVisible(false);chartView_->setChart(chart);applyFilter();}
void AnalyticsPage::applyFilter(){const QString q=ui->searchEdit->text();const int status=ui->statusFilter->currentIndex();for(int r=0;r<model_->rowCount();++r){const bool text=model_->item(r,0)->text().contains(q,Qt::CaseInsensitive)||model_->item(r,1)->text().contains(q,Qt::CaseInsensitive);const QString s=model_->item(r,5)->text();const bool st=status==0||(status==1&&s=="Defaulter")||(status==2&&s=="Good Standing");ui->analyticsTable->setRowHidden(r,!(text&&st));}}
