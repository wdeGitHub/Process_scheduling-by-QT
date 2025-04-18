#include "mainwindow.h"
#include "ui_mainwindow.h"
#include<opencv2/opencv.hpp>
#include<QFileDialog>
#include<QDebug>
using namespace cv;
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    table_Init();
}
void MainWindow::table_Init()
{
    ui->tableInfo->clear();
    ui->tableInfo->clearContents();
    QStringList headerText;
    headerText<<"进程名"<<"产生时间"<<"要求服务时间"<<"优先级";
    //    ui->tableInfo->setHorizontalHeaderLabels(headerText); //只设置标题
    ui->tableInfo->setColumnCount(headerText.size());      //设置表格列数
    for (int i=0;i<ui->tableInfo->columnCount();i++)
    {
        QTableWidgetItem *headerItem=new QTableWidgetItem(headerText.at(i));
        QFont font=headerItem->font();   //获取原有字体设置
        font.setBold(true);              //设置为粗体
        font.setPointSize(11);           //字体大小
        headerItem->setForeground(QBrush(Qt::green));  //设置文字颜色
        headerItem->setFont(font);       //设置字体
        ui->tableInfo->setHorizontalHeaderItem(i,headerItem);    //设置表头单元格的item
    }
     //ui->tableInfo->setRowCount(4);
    setprocess_name();
     //ui->tableInfo->horizontalHeader()->setStretchLastSection(true);
    ui->tableInfo->setFrameShape(QFrame::NoFrame);
    //ui->tableInfo->horizontalHeader()->setResizeMode(QHeaderView::Stretch);
    // ui->tableInfo->resizeRowsToContents();
    //ui->tableInfo->resizeColumnsToContents();
    //ui->tableInfo->setColumnWidth(3,10);
    ui->lineEdit->hide();
}
MainWindow::~MainWindow()
{
    delete chartwindow;
    delete ui;
}

void MainWindow::on_setprocess_num_clicked()
{
    ui->tableInfo->setRowCount(ui->spinRowcount->value());
    setprocess_name();
    //ui->tableInfo->resizeRowsToContents();
    //ui->tableInfo->resizeColumnsToContents();
}

void MainWindow::setprocess_name()
{
    for(int i=0;i<ui->tableInfo->rowCount();++i)
    {
        QString processName=QString("P%1").arg(i+1);
        createItemsARow(i,processName);
    }
}
void MainWindow::createItemsARow(int norow,QString processName)
{
    QTableWidgetItem *item=new  QTableWidgetItem(processName);
    ui->tableInfo->setItem(norow,0,item);
    item->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
    //Qt::ItemFlags flags=Qt::ItemIsSelectable |Qt::ItemIsEnabled;
}

void MainWindow::on_add_process_clicked()
{
    int curRow=ui->tableInfo->rowCount();
    ui->tableInfo->setRowCount(curRow+1);
    createItemsARow(curRow,QString("P%1").arg(curRow+1));
    //ui->tableInfo->resizeRowsToContents();
    //ui->tableInfo->resizeColumnsToContents();
}


void MainWindow::on_sub_process_clicked()
{
    int curRow=ui->tableInfo->rowCount();
    ui->tableInfo->setRowCount(curRow-1);
    //ui->tableInfo->resizeRowsToContents();
    //ui->tableInfo->resizeColumnsToContents();
}


void MainWindow::on_read_excel_clicked()
{
     this->chartwindow=new ChartWindow(ui->tableInfo->rowCount());
    //chartwindow chartwindow(ui->tableInfo->rowCount());
     //还要加一个读入管控
     //qDebug()<<ui->tableInfo->item(0,1)->text().isNull();
    int flag1=0,flag2=0;
    for(int i=0;i<ui->tableInfo->rowCount();++i)
    {
        chartwindow->pcontent[i].process_name=i+1;
        if(ui->tableInfo->item(i,1)==nullptr
            ||ui->tableInfo->item(i,2)==nullptr
            ||ui->tableInfo->item(i,1)->text().isEmpty()
            ||ui->tableInfo->item(i,2)->text().isEmpty())
        {
            flag1=1;
            break;
        }else
        {
            chartwindow->pcontent[i].appear_time=ui->tableInfo->item(i,1)->text().toInt();
            chartwindow->pcontent[i].sevice_time=ui->tableInfo->item(i,2)->text().toInt();
        }
        if(ui->tableInfo->item(i,3)!=nullptr)
        chartwindow->pcontent[i].priority=ui->tableInfo->item(i,3)->text().toInt();
        else
        {
            flag2=1;
        }
    }
    if(flag1==1)
    {
        ui->lineEdit->show();
        ui->lineEdit->clear();
        ui->lineEdit->insert("WARNING!Your data isn't fully populated");
       // qDebug()<<"warning!";
    }
    else
    {
        if(flag2==1)
        {
            ui->lineEdit->show();
            ui->lineEdit->clear();
            ui->lineEdit->insert("Missing built-in priorities might make some features unavailable.");
        }
    chartwindow->displaydata();
    }

}

void MainWindow::on_clear_num_clicked()
{
    ui->tableInfo->clear();
    table_Init();
}

