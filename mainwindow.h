#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QMessageBox>
#include <QtConcurrent/QtConcurrent>

#include "database.h"
#include "stats.h"


QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    void IsConnected(bool connected);
    void DisplayError(QSqlError error);

    void FillAirportList(QVector<QString> list);
    void ShowTable(QTableView *view);

private slots:

    void on_pb_ShowTable_clicked();
    void on_pb_ShowStat_clicked();

    void on_cBox_isDate_toggled(bool checked);


private:
    Ui::MainWindow *ui;
    QMessageBox* msg;
    QTimer *timer;

    DataBase *DB;
    Stats *stats;

};
#endif // MAINWINDOW_H
