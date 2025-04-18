#include "chartwindow.h"
#include "ui_chartwindow.h"
#include<QtCharts>
#include<QChartView>
#include<iostream>
#include<algorithm>
#include<queue>
ChartWindow::ChartWindow(int pnum,QWidget *parent)
    : QMainWindow(parent),pcontent(new Pcontent[pnum]),pnumber(pnum)
    , ui(new Ui::ChartWindow)
{
    ui->setupUi(this);
}
void ChartWindow::displaydata()
{
    this->show();
}
ChartWindow::~ChartWindow()
{
    delete chart;
    delete axisY;
    delete axisX;
    delete []series;
    delete [] pcontent;
    delete ui;
}
void ChartWindow::set_chart(int cur)
{
    axisX->setRange(0, cur+1);             //设置坐标轴范围
    axisX->setLabelFormat("%.1f");  //标签格式
    axisX->setTickCount(cur+2);        //主刻度个数
    //axisX->setMinorTickCount(1);    //次刻度个数
    axisX->setTitleText("时间");

    axisY->setRange(0, pnumber+1);
    axisY->setTitleText("进程");
    axisY->setTickCount(pnumber+2);        //主刻度个数
    axisY->setTickCount(1);
    axisY->setLabelFormat("P%d");
    chart->addAxis(axisX, Qt::AlignBottom); //添加作为底边坐标轴
    chart->addAxis(axisY, Qt::AlignLeft);   //添加作为左边坐标轴
    for(int i=0;i<pnumber*4;i++)
    {
        series[i].attachAxis(axisX); //为序列附加坐标轴
        series[i].attachAxis(axisY);
    }
    ui->chartView->setChart(chart);
    ui->chartView->setRenderHint(QPainter::Antialiasing);
}

void ChartWindow::reinit_chart()
{
    // 移除并重置所有水平坐标轴
    if(axisX!=nullptr&&axisY!=nullptr)
    {
        QList<QAbstractAxis*> xAxes = chart->axes(Qt::Horizontal);
        for (QAbstractAxis* axis : xAxes) {
            chart->removeAxis(axis);
            delete axis; // 若axis由用户创建
        }
    }
    this->axisX = new QValueAxis; //X 轴
    this->axisY = new QValueAxis; //Y 轴

    //清理并重置序列
    if(series!=nullptr)
    {
        for(int i=0;i<pnumber*4;i++)
            chart->removeSeries(&series[i]);
        delete []series;
    }
    this->series = new QLineSeries[pnumber*4];

    //清理并重置表格
    if(chart!=nullptr)
    {
        QChart *oldChart = ui->chartView->chart();
        QChart *newChart=new QChart;
        ui->chartView->setChart(newChart);  // 解除旧图表关联
        delete oldChart;                   // 显式释放内存
        this->chart=newChart;
    }
    else
    {
        this->chart = new QChart();
    }
}

//先来先服务算法
void ChartWindow::on_fcfs_clicked()
{
    reinit_chart();
    chart->setTitle(tr("先来先服务"));
    Pcontent* cpcontent = new Pcontent[pnumber];
    memcpy(cpcontent, pcontent, pnumber * sizeof(Pcontent)); // 逐个元素复制
    std::sort(cpcontent,cpcontent+pnumber,[](const Pcontent a,const Pcontent b){
        return a.appear_time<b.appear_time;
    });
    int cur=0;//记录花费的总时间
    for(int i=0;i<pnumber;i++)
    {

        if(i==0)
        {
          series[i].append(cpcontent[0].appear_time,cpcontent[0].process_name);
            cur=cpcontent[i].appear_time+cpcontent[i].sevice_time;
          series[i].append(cur,cpcontent[0].process_name);
        }
        else
        {
            cur=std::max(cur,cpcontent[i].appear_time);
            series[i].append(cur,cpcontent[i].process_name);
            cur+=cpcontent[i].sevice_time;
            series[i].append(cur,cpcontent[i].process_name);
        }
        chart->addSeries(&series[i]); //将序列添加到图表
    }
    set_chart(cur);
    delete []cpcontent;
}
//短进程优先算法
void ChartWindow::on_spf_clicked()
{
    reinit_chart();
    chart->setTitle(tr("短进程优先"));
    Pcontent* cpcontent = new Pcontent[pnumber];
    memcpy(cpcontent, pcontent, pnumber * sizeof(Pcontent)); // 逐个元素复制
    std::sort(cpcontent,cpcontent+pnumber,[](const Pcontent a,const Pcontent b){
        return a.appear_time<b.appear_time;
    });
    int cur=0;//记录花费的总时间
    for(int i=0;i<pnumber;++i)
    {
        if(i==0)
        {
        series[0].append(cpcontent[0].appear_time,cpcontent[0].process_name);
        cur=cpcontent[i].appear_time+cpcontent[i].sevice_time;
        series[i].append(cur,cpcontent[0].process_name);
        }
        else
        {
            //查找多少个进程需要排序
            int pro=pnumber-1;
            for(int j=i;j<pnumber;++j)
            {
                if(cpcontent[j].appear_time>cur)
                {
                    pro=j-1;
                    break;
                }
            }
            //重排
            std::sort(cpcontent+i,cpcontent+pro,[](const Pcontent a,const Pcontent b){
        return a.sevice_time<b.sevice_time;
    });
            cur=std::max(cur,cpcontent[i].appear_time);
            series[i].append(cur,cpcontent[i].process_name);
            cur+=cpcontent[i].sevice_time;
            series[i].append(cur,cpcontent[i].process_name);
        }
        chart->addSeries(&series[i]); //将序列添加到图表
    }
    set_chart(cur);
    delete []cpcontent;
}

//短进程优先算法(抢占式)
void ChartWindow::on_srtn_clicked()
{
    reinit_chart();
    chart->setTitle(tr("短进程优先(抢占式)"));

    // 复制进程数据以便处理
    Pcontent* cpcontent = new Pcontent[pnumber];
    memcpy(cpcontent, pcontent, pnumber * sizeof(Pcontent));

    // 按到达时间排序
    std::sort(cpcontent, cpcontent+pnumber, [](const Pcontent& a, const Pcontent& b){
        return a.appear_time < b.appear_time;
    });

    // 用于记录剩余执行时间
    int* remain_time = new int[pnumber];
    for(int i = 0; i < pnumber; i++) {
        remain_time[i] = cpcontent[i].sevice_time;
    }

    int current_time = cpcontent[0].appear_time; // 当前时间
    int completed = 0; // 完成的进程数
    int series_index = 0; // 图表序列索引
    int last_process = -1; // 上一个执行的进程
    int segment_start_time = current_time; // 当前片段的开始时间

    while(completed < pnumber) {
        // 找出当前时间点可执行且剩余时间最短的进程
        int shortest = -1;
        int min_remain = INT_MAX;

        for(int i = 0; i < pnumber; i++) {
            if(cpcontent[i].appear_time <= current_time && remain_time[i] > 0) {
                if(remain_time[i] < min_remain) {
                    min_remain = remain_time[i];
                    shortest = i;
                }
            }
        }

        if(shortest != -1) {
            // 如果进程发生切换，结束当前片段并开始新片段
            if(shortest != last_process) {
                if(last_process != -1) {
                    // 结束上一个进程的片段
                    series[series_index].append(current_time, cpcontent[last_process].process_name);
                    chart->addSeries(&series[series_index]);
                    series_index++;
                }
                // 开始新进程的片段
                series[series_index].append(current_time, cpcontent[shortest].process_name);
                segment_start_time = current_time;
            }

            remain_time[shortest]--;
            last_process = shortest;

            // 如果进程完成
            if(remain_time[shortest] == 0) {
                completed++;
                series[series_index].append(current_time + 1, cpcontent[shortest].process_name);
                chart->addSeries(&series[series_index]);
                series_index++;
                last_process = -1;
            }
        }

        current_time++;
    }

    // 设置图表显示
    set_chart(current_time);

    // 清理内存
    delete[] cpcontent;
    delete[] remain_time;
}


void ChartWindow::on_hrrn_clicked()
{
    reinit_chart();
    chart->setTitle(tr("高响应比优先"));

    // 复制进程数据以便处理
    Pcontent* cpcontent = new Pcontent[pnumber];
    memcpy(cpcontent, pcontent, pnumber * sizeof(Pcontent));

    // 按到达时间排序
    std::sort(cpcontent, cpcontent+pnumber, [](const Pcontent& a, const Pcontent& b){
        return a.appear_time < b.appear_time;
    });

    int cur = cpcontent[0].appear_time; // 当前时间
    bool* completed = new bool[pnumber](); // 记录进程是否完成
    int completed_count = 0; // 完成的进程数量

    for(int i = 0; i < pnumber; i++) {
        // 找出当前时间点可执行的进程中响应比最高的
        int selected = -1;
        double max_response_ratio = -1;

        for(int j = 0; j < pnumber; j++) {
            if(!completed[j] && cpcontent[j].appear_time <= cur) {
                // 计算响应比：(等待时间 + 服务时间) / 服务时间
                double wait_time = cur - cpcontent[j].appear_time;
                double response_ratio = (wait_time + cpcontent[j].sevice_time) / cpcontent[j].sevice_time;

                if(response_ratio > max_response_ratio) {
                    max_response_ratio = response_ratio;
                    selected = j;
                }
            }
        }

        if(selected != -1) {
            // 绘制选中进程的执行时间段
            series[i].append(cur, cpcontent[selected].process_name);
            cur += cpcontent[selected].sevice_time;
            series[i].append(cur, cpcontent[selected].process_name);

            completed[selected] = true;
            completed_count++;

            chart->addSeries(&series[i]);
        } else {
            // 如果没有就绪的进程，时间向前推进到下一个到达的进程
            int next_time = INT_MAX;
            for(int j = 0; j < pnumber; j++) {
                if(!completed[j] && cpcontent[j].appear_time > cur) {
                    next_time = std::min(next_time, cpcontent[j].appear_time);
                }
            }
            if(next_time != INT_MAX) {
                cur = next_time;
                i--; // 重试当前轮次
            }
        }
    }

    set_chart(cur);
    delete[] completed;
    delete[] cpcontent;
}

//（抢占式）时间片轮转
void ChartWindow::on_rr_clicked()
{
    reinit_chart();
    chart->setTitle(tr("时间片轮转"));

    // 复制进程数据以便处理
    Pcontent* cpcontent = new Pcontent[pnumber];
    memcpy(cpcontent, pcontent, pnumber * sizeof(Pcontent));

    const int TIME_SLICE = 2; // 设置时间片大小
    int* remain_time = new int[pnumber]; // 剩余执行时间
    for(int i = 0; i < pnumber; i++) {
        remain_time[i] = cpcontent[i].sevice_time;
    }

    std::queue<int> ready_queue; // 就绪队列
    int cur_time = 0; // 当前时间
    int completed = 0; // 完成的进程数
    int series_index = 0; // 图表序列索引
    bool* in_queue = new bool[pnumber](); // 标记进程是否在队列中

    // 找到最早到达的进程时间
    cur_time = cpcontent[0].appear_time;
    for(int i = 0; i < pnumber; i++) {
        if(cpcontent[i].appear_time <= cur_time) {
            ready_queue.push(i);
            in_queue[i] = true;
        }
    }

    while(completed < pnumber) {
        if(ready_queue.empty()) {
            // 如果就绪队列为空，时间推进到下一个到达的进程
            int next_time = INT_MAX;
            int next_proc = -1;
            for(int i = 0; i < pnumber; i++) {
                if(!in_queue[i] && remain_time[i] > 0 && cpcontent[i].appear_time < next_time) {
                    next_time = cpcontent[i].appear_time;
                    next_proc = i;
                }
            }
            if(next_proc != -1) {
                cur_time = next_time;
                ready_queue.push(next_proc);
                in_queue[next_proc] = true;
            }
            continue;
        }

        int current_proc = ready_queue.front();
        ready_queue.pop();
        in_queue[current_proc] = false;

        // 执行时间片
        int exec_time = std::min(TIME_SLICE, remain_time[current_proc]);
        series[series_index].append(cur_time, cpcontent[current_proc].process_name);
        cur_time += exec_time;
        series[series_index].append(cur_time, cpcontent[current_proc].process_name);
        chart->addSeries(&series[series_index]);
        series_index++;

        remain_time[current_proc] -= exec_time;

        // 检查在这个时间片期间是否有新进程到达
        for(int i = 0; i < pnumber; i++) {
            if(!in_queue[i] && remain_time[i] > 0 &&
               cpcontent[i].appear_time <= cur_time && i != current_proc) {
                ready_queue.push(i);
                in_queue[i] = true;
            }
        }

        // 如果进程还未完成，重新加入队列
        if(remain_time[current_proc] > 0) {
            ready_queue.push(current_proc);
            in_queue[current_proc] = true;
        } else {
            completed++;
        }
    }

    set_chart(cur_time);
    delete[] remain_time;
    delete[] in_queue;
    delete[] cpcontent;
}

//(非抢占式）优先级调度
void ChartWindow::on_non_preemptive_priority_clicked()
{
    reinit_chart();
    chart->setTitle(tr("优先级调度(非抢占)"));

    // 复制进程数据以便处理
    Pcontent* cpcontent = new Pcontent[pnumber];
    memcpy(cpcontent, pcontent, pnumber * sizeof(Pcontent));

    // 记录进程是否完成
    bool* completed = new bool[pnumber]();
    int completed_count = 0;
    int cur_time = cpcontent[0].appear_time;
    int series_index = 0;

    while(completed_count < pnumber) {
        // 找到当前时间点已到达且优先级最高的进程
        int selected = -1;
        int highest_priority = INT_MIN;

        for(int i = 0; i < pnumber; i++) {
            if(!completed[i] && cpcontent[i].appear_time <= cur_time) {
                if(cpcontent[i].priority > highest_priority) {
                    highest_priority = cpcontent[i].priority;
                    selected = i;
                }
            }
        }

        if(selected != -1) {
            // 执行选中的进程
            series[series_index].append(cur_time, cpcontent[selected].process_name);
            cur_time += cpcontent[selected].sevice_time;
            series[series_index].append(cur_time, cpcontent[selected].process_name);

            chart->addSeries(&series[series_index]);
            series_index++;

            completed[selected] = true;
            completed_count++;
        } else {
            // 如果没有就绪的进程，时间推进到下一个到达的进程
            int next_time = INT_MAX;
            for(int i = 0; i < pnumber; i++) {
                if(!completed[i] && cpcontent[i].appear_time > cur_time) {
                    next_time = std::min(next_time, cpcontent[i].appear_time);
                }
            }
            if(next_time != INT_MAX) {
                cur_time = next_time;
            }
        }
    }

    set_chart(cur_time);
    delete[] completed;
    delete[] cpcontent;
}

//(抢占式）优先级调度
void ChartWindow::on_preemptive_priority_clicked()
{
    reinit_chart();
    chart->setTitle(tr("优先级调度(抢占式)"));

    // 复制进程数据以便处理
    Pcontent* cpcontent = new Pcontent[pnumber];
    memcpy(cpcontent, pcontent, pnumber * sizeof(Pcontent));

    int* remain_time = new int[pnumber]; // 剩余执行时间
    for(int i = 0; i < pnumber; i++) {
        remain_time[i] = cpcontent[i].sevice_time;
    }

    int current_time = cpcontent[0].appear_time; // 当前时间
    int completed = 0; // 完成的进程数
    int series_index = 0; // 图表序列索引
    int last_process = -1; // 上一个执行的进程

    while(completed < pnumber) {
        // 找出当前时间点已到达且优先级最高的进程
        int selected = -1;
        int highest_priority = INT_MIN;

        for(int i = 0; i < pnumber; i++) {
            if(remain_time[i] > 0 && cpcontent[i].appear_time <= current_time) {
                if(cpcontent[i].priority > highest_priority) {
                    highest_priority = cpcontent[i].priority;
                    selected = i;
                }
            }
        }

        if(selected != -1) {
            // 如果进程发生切换，需要开始新的图表序列
            if(selected != last_process) {
                if(last_process != -1) {
                    series[series_index].append(current_time, cpcontent[last_process].process_name);
                    chart->addSeries(&series[series_index]);
                    series_index++;
                }
                series[series_index].append(current_time, cpcontent[selected].process_name);
            }

            remain_time[selected]--;
            last_process = selected;

            // 如果进程完成
            if(remain_time[selected] == 0) {
                series[series_index].append(current_time + 1, cpcontent[selected].process_name);
                chart->addSeries(&series[series_index]);
                series_index++;
                last_process = -1;
                completed++;
            }
        }

        current_time++;
    }

    set_chart(current_time);
    delete[] remain_time;
    delete[] cpcontent;
}

//(抢占式)多级反馈队列
void ChartWindow::on_mfq_clicked()
{
    reinit_chart();
    chart->setTitle(tr("多级反馈队列"));

    // 复制进程数据以便处理
    Pcontent* cpcontent = new Pcontent[pnumber];
    memcpy(cpcontent, pcontent, pnumber * sizeof(Pcontent));

    const int QUEUE_NUM = 3; // 队列数量
    const int BASE_TIME_SLICE = 2; // 基础时间片

    // 多级队列，每一级队列的时间片是上一级的2倍
    std::vector<std::queue<int>> queues(QUEUE_NUM);
    int* remain_time = new int[pnumber]; // 剩余执行时间
    int* current_queue = new int[pnumber]; // 当前所在队列级别
    bool* in_queue = new bool[pnumber](); // 是否在队列中

    // 初始化
    for(int i = 0; i < pnumber; i++) {
        remain_time[i] = cpcontent[i].sevice_time;
        current_queue[i] = 0; // 所有进程初始在最高优先级队列
    }

    int current_time = cpcontent[0].appear_time;
    int completed = 0;
    int series_index = 0;
    int last_process = -1;

    // 初始将已到达的进程加入最高优先级队列
    for(int i = 0; i < pnumber; i++) {
        if(cpcontent[i].appear_time <= current_time) {
            queues[0].push(i);
            in_queue[i] = true;
        }
    }

    while(completed < pnumber) {
        // 从高优先级队列开始查找可执行的进程
        int selected = -1;
        int selected_queue = -1;

        for(int q = 0; q < QUEUE_NUM && selected == -1; q++) {
            if(!queues[q].empty()) {
                selected = queues[q].front();
                selected_queue = q;
                queues[q].pop();
                in_queue[selected] = false;
            }
        }

        if(selected != -1) {
            // 计算当前队列的时间片
            int time_slice = BASE_TIME_SLICE * (1 << selected_queue);
            int exec_time = std::min(time_slice, remain_time[selected]);

            // 如果进程发生切换，开始新的图表序列
            if(selected != last_process) {
                if(last_process != -1) {
                    series[series_index].append(current_time, cpcontent[last_process].process_name);
                    chart->addSeries(&series[series_index]);
                    series_index++;
                }
                series[series_index].append(current_time, cpcontent[selected].process_name);
            }

            // 执行进程
            remain_time[selected] -= exec_time;
            current_time += exec_time;

            // 检查是否有新进程到达
            for(int i = 0; i < pnumber; i++) {
                if(!in_queue[i] && remain_time[i] > 0 &&
                   cpcontent[i].appear_time <= current_time && i != selected) {
                    queues[0].push(i); // 新进程加入最高优先级队列
                    in_queue[i] = true;
                }
            }

            // 处理当前进程的去向
            if(remain_time[selected] > 0) {
                // 如果进程未完成，降低其优先级并加入下一级队列
                current_queue[selected] = std::min(current_queue[selected] + 1, QUEUE_NUM - 1);
                queues[current_queue[selected]].push(selected);
                in_queue[selected] = true;
            } else {
                // 进程完成
                series[series_index].append(current_time, cpcontent[selected].process_name);
                chart->addSeries(&series[series_index]);
                series_index++;
                completed++;
                last_process = -1;
                continue;
            }

            last_process = selected;
        } else {
            // 如果没有就绪的进程，时间推进到下一个到达的进程
            int next_time = INT_MAX;
            int next_proc = -1;
            for(int i = 0; i < pnumber; i++) {
                if(!in_queue[i] && remain_time[i] > 0 &&
                   cpcontent[i].appear_time > current_time &&
                   cpcontent[i].appear_time < next_time) {
                    next_time = cpcontent[i].appear_time;
                    next_proc = i;
                }
            }
            if(next_proc != -1) {
                current_time = next_time;
                queues[0].push(next_proc);
                in_queue[next_proc] = true;
            }
        }
    }

    set_chart(current_time);
    delete[] remain_time;
    delete[] current_queue;
    delete[] in_queue;
    delete[] cpcontent;
}

