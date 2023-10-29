#ifndef DATABASE_H
#define DATABASE_H

#include <QObject>
#include <QSqlDatabase>
#include <QSqlError>
#include <QTableView>
#include <QSqlQueryModel>
#include <QSqlQuery>

#define POSTGRE_DRIVER "QPSQL"
#define DB_NAME "MyDB"

#define HOST "981757-ca08998.tmweb.ru"
#define DATABASE "demo"
#define USER "netology_usr_cpp"
#define PASS "CppNeto3"
#define PORT 5432


class DataBase  : public QObject
{
    Q_OBJECT
public:
    explicit DataBase(QObject *parent = nullptr);

    void AddDataBase(QString driver, QString DBName);
    void ConnectToDB();

    void GetAirpoptList();

    void GetDepartureFlights(QString airportName, bool isCurrentDate, QDate date);
    void GetArrivalFlights(QString airportName, bool isCurrentDate, QDate date);

    void SendYearStatistic(QString airportName);
    void SendMonthStatistic(QString airportName, int selectedMonth);


signals:
    void sig_Connection(bool);
    void sig_GetAirportList(QVector<QString>);
    void sig_GetView(QTableView*);

    void sig_SendError(QSqlError);

    void sig_SendYearStat(QVector<int>&, QVector<int>&);
    void sig_SendMonthStat(QVector<int>&);



private:
    QSqlDatabase *DB;

    QHash<QString, QString> airPorts;
    QString airportCode;
    QString request;

    QSqlQuery *query;
    QSqlQueryModel *QueryModel;

    QTableView *View;

};

#endif // DATABASE_H
