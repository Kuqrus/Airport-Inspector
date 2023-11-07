#ifndef STATS_H
#define STATS_H

#include "database.h"
#include <QDialog>

#include <QtWidgets/QApplication>
#include <QtWidgets/QMainWindow>

#include <QtCharts/QChartView>
#include <QtCharts/QLineSeries>
#include <QtCharts/QBarSeries>
#include <QtCharts/QBarSet>
#include <QtCharts/QLegend>
#include <QtCharts/QBarCategoryAxis>
#include <QtCharts/QValueAxis>


namespace Ui {
class Stats;
}

class Stats : public QDialog
{
    Q_OBJECT

public:
    explicit Stats(QWidget *parent = nullptr);
    ~Stats();

    void UpdateSatistic(DataBase *DB, QString port);

    void WriteYearStatistic(QVector<int> &vecCount, QVector<int> &vecDate);
    void WriteMonthStatistic(QVector<int> &vecCount);
    void SaveHash(QHash<int, QVector<int>> &hash);

    void clearLayout(QLayout *layout);


private slots:
    void on_pb_exit_clicked();

    void on_cb_months_currentIndexChanged(int index);

signals:
    void sig_GetYearStat(QVector<int>&, QVector<int>&);
    void sig_GetMonthStat(QVector<int>&);

    void sig_GetAllMonthStat(QHash<int, QVector<int>>&);


private:
    Ui::Stats *ui;

    DataBase *DB;
    QString selectedAirport;
    QHash<int, QVector<int>> monthStats;

    QChart *chart;
    QChartView *chartView;

};

#endif // STATS_H
