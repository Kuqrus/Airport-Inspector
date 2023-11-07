#include "mainwindow.h"
#include "./ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    msg = new QMessageBox(this);
    timer = new QTimer(this);

    ui->rb_departure->setChecked(1);
    IsConnected(0);

    DB = new DataBase(this);
    DB->AddDataBase(POSTGRE_DRIVER, DB_NAME);

    timer->start(0);
    connect(timer, &QTimer::timeout, this, [&]{

        auto conn = [&]{
            DB->ConnectToDB();
        };
        QtConcurrent::run(conn);
        timer->stop();

    });

    stats = new Stats(this);
    stats->setModal(1);

    connect(DB, &DataBase::sig_Connection, this, &MainWindow::IsConnected);
    connect(DB, &DataBase::sig_SendError, this, &MainWindow::DisplayError);

    connect(DB, &DataBase::sig_GetAirportList, this, &MainWindow::FillAirportList);
    connect(DB, &DataBase::sig_GetView, this, &MainWindow::ShowTable);

    connect(DB, &DataBase::sig_SendYearStat, stats, &Stats::sig_GetYearStat);
    connect(DB, &DataBase::sig_SendMonthStat, stats, &Stats::sig_GetMonthStat);
    connect(DB, &DataBase::sig_SendAllMonthStat, stats, &Stats::sig_GetAllMonthStat);

}

MainWindow::~MainWindow()
{    
    delete ui;
}

void MainWindow::IsConnected(bool connected)
{
    ui->pb_ShowStat->setEnabled(connected);
    ui->pb_ShowTable->setEnabled(connected);
    ui->cb_Aeroports->setEnabled(connected);
    ui->cBox_isDate->setEnabled(connected);
    ui->rb_arrival->setEnabled(connected);
    ui->rb_departure->setEnabled(connected);

    if(connected){
        ui->statusbar->showMessage("Подключено");
        ui->statusbar->setStyleSheet("color : green");
        DB->GetAirpoptList();
    }
    else{
        ui->statusbar->showMessage("Отключено");
        ui->statusbar->setStyleSheet("color : red");
    }
}

void MainWindow::DisplayError(QSqlError error)
{
    msg->setIcon(QMessageBox::Critical);
    msg->setText(error.text());
    msg->exec();
    timer->start(5000);
}

void MainWindow::FillAirportList(QVector<QString> list)
{
    for(const auto& el : list){
        ui->cb_Aeroports->addItem(el);
    }
}

void MainWindow::ShowTable(QTableView *view)
{
    view->setSortingEnabled(1);
    view->sortByColumn(2, Qt::AscendingOrder);
    ui->tv_flights->setModel(view->model());
    ui->tv_flights->resizeColumnsToContents();
}

void MainWindow::on_pb_ShowTable_clicked()
{
    QString SelectedPort = ui->cb_Aeroports->currentText();
    bool isCurrentDate = ui->cBox_isDate->isChecked();
    QDate date = ui->de_SelectDate->date();

    if(ui->rb_arrival->isChecked()){
        DB->GetArrivalFlights(SelectedPort, isCurrentDate, date);
    }
    else if (ui->rb_departure->isChecked()){
        DB->GetDepartureFlights(SelectedPort, isCurrentDate, date);
    }
}

void MainWindow::on_cBox_isDate_toggled(bool checked)
{
    ui->de_SelectDate->setEnabled(checked);
}


void MainWindow::on_pb_ShowStat_clicked()
{
    stats->UpdateSatistic(DB, ui->cb_Aeroports->currentText());
    stats->show();
}


