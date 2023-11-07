#include "stats.h"
#include "ui_stats.h"

Stats::Stats(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Stats)
{
    ui->setupUi(this);

    connect(this, &Stats::sig_GetYearStat, this, &Stats::WriteYearStatistic);
    connect(this, &Stats::sig_GetMonthStat, this, &Stats::WriteMonthStatistic);
    connect(this, &Stats::sig_GetAllMonthStat, this, &Stats::SaveHash);
}

Stats::~Stats()
{
    delete ui;
    delete DB;
    delete chart;
    delete chartView;
}

void Stats::UpdateSatistic(DataBase *dBase, QString port)
{
    selectedAirport = port;

    DB = dBase;

    ui->lb_portName->setText("Статистика аэропорта: " + port);

    DB->SendYearStatistic(selectedAirport);
    //DB->SendMonthStatistic(selectedAirport, ui->cb_months->currentIndex());         // Статистика с запросом каждого месяца отдельно
    DB->SendAllMonthStatistic(selectedAirport);                                   // Статистика с запросом всех месяцев разом

}

void Stats::WriteYearStatistic(QVector<int> &vecCount, QVector<int> &vecDate)
{
    clearLayout(ui->vLay_year);

    QLocale loc = QLocale(QLocale::Russian);
    QString month;

    QBarSet *set0 = new QBarSet("", this);
    QStringList categories;

    int max = 0;

    for(int i = 0; i < vecCount.size(); i++){
        month = loc.standaloneMonthName(vecDate[i]);
        month[0] = month[0].toUpper();

        *set0 << vecCount[i];
        categories << month;

        if(max < vecCount[i]){
            max = vecCount[i];
        }
    }

    QBarSeries *series = new QBarSeries(this);
    series->append(set0);

    chart = new QChart();
    chart->addSeries(series);
    chart->setAnimationOptions(QChart::SeriesAnimations);

    QBarCategoryAxis *axisX = new QBarCategoryAxis(this);
    axisX->append(categories);
    chart->addAxis(axisX, Qt::AlignBottom);
    series->attachAxis(axisX);

    QValueAxis *axisY = new QValueAxis(this);
    axisY->setRange(0, max);
    chart->addAxis(axisY, Qt::AlignLeft);
    series->attachAxis(axisY);

    chart->legend()->setVisible(false);

    chartView = new QChartView(chart);
    chartView->setRenderHint(QPainter::Antialiasing);

    ui->vLay_year->addWidget(chartView);
}

void Stats::WriteMonthStatistic(QVector<int> &vecCount)
{
    clearLayout(ui->vLay_month);

    QLineSeries *series = new QLineSeries(this);

    int day = 1;
    int max = vecCount[0];
    int min = vecCount[0];
    for(const auto& el : vecCount){
        series->append(day, el);
        day++;

        if(el > max){
            max = el;
        }
        if(min > el){
            min = el;
        }
    }

    chart = new QChart();
    chart->legend()->hide();
    chart->addSeries(series);

    QValueAxis *xAxis = new QValueAxis(this);
    xAxis->setLabelFormat("%.0f");
    xAxis->setTickCount(vecCount.size());
    chart->addAxis(xAxis, Qt::AlignBottom);
    series->attachAxis(xAxis);


    QValueAxis *yAxis = new QValueAxis(this);
    yAxis->setLabelFormat("%.0f");

    // Если сделать +-1, то график отображается не совсем корректно
    // По сути восприятию не мешает, но так аккуратнее)
    if(min == max){
        min -= 2;
        max += 2;
    }

    yAxis->setMin(min);
    yAxis->setMax(max);
    chart->addAxis(yAxis, Qt::AlignLeft);
    series->attachAxis(yAxis);

    chartView = new QChartView(chart);
    chartView->setRenderHint(QPainter::Antialiasing);

    ui->vLay_month->addWidget(chartView);
}

void Stats::SaveHash(QHash<int, QVector<int> > &hash)
{
    monthStats = hash;
    WriteMonthStatistic(monthStats[ui->cb_months->currentIndex() + 1]);
}

void Stats::clearLayout(QLayout *layout)
{
    if (layout) {
        while(layout->count() > 0){
            QLayoutItem *item = layout->takeAt(0);
            QWidget* widget = item->widget();
            if(widget)
                delete widget;
            delete item;
        }
    }
}

void Stats::on_cb_months_currentIndexChanged(int index)
{    
    //DB->SendMonthStatistic(selectedAirport, index);                         // Статистика с запросом каждого месяца отдельно
    WriteMonthStatistic(monthStats[ui->cb_months->currentIndex() + 1]);   // Статистика с запросом всех месяцев разом
}

void Stats::on_pb_exit_clicked()
{
    close();
}

