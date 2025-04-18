#ifndef CHARTWINDOW_H
#define CHARTWINDOW_H

#include <QMainWindow>
#include<QString>
#include    <QtCharts>

namespace Ui {
class ChartWindow;
}

typedef struct Pcontent
{
    int process_name;
    int appear_time;
    int sevice_time;
    int priority;
}Pcontent;
class ChartWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit ChartWindow(int pnum,QWidget *parent = nullptr);
    void displaydata();
    ~ChartWindow();
    void set_chart(int cur);
    void reinit_chart();
    Pcontent* pcontent;
private slots:
    void on_fcfs_clicked();
    void on_spf_clicked();
    void on_srtn_clicked();

    void on_hrrn_clicked();

    void on_rr_clicked();

    void on_non_preemptive_priority_clicked();

    void on_preemptive_priority_clicked();

    void on_mfq_clicked();

private:
    int pnumber;
    QChart  *chart=nullptr;        //当前的图表
    QValueAxis *axisX=nullptr;
    QValueAxis *axisY=nullptr;
    QLineSeries *series=nullptr;    //当前序列
private:
    Ui::ChartWindow *ui;
};

#endif // CHARTWINDOW_H
