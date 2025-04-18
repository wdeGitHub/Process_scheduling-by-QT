#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include"chartwindow.h"
QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT
private:
   void  table_Init();
    void setprocess_name();
   void createItemsARow(int norow,QString processName);

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_setprocess_num_clicked();

    void on_add_process_clicked();

    void on_sub_process_clicked();

    void on_read_excel_clicked();

    void on_clear_num_clicked();

private:
    Ui::MainWindow *ui;
    ChartWindow *chartwindow;
};
#endif // MAINWINDOW_H
