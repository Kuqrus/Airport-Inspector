#include "database.h"
#include "qdatetime.h"

DataBase::DataBase(QObject *parent)
    : QObject{parent}
{
    DB = new QSqlDatabase;    


    query = new QSqlQuery();

    QueryModel = new QSqlQueryModel();
    View = new QTableView();
}

void DataBase::AddDataBase(QString driver, QString DBName)
{
    *DB = QSqlDatabase::addDatabase(driver, DBName);

}


void DataBase::ConnectToDB()
{
    DB->setHostName(HOST);
    DB->setDatabaseName(DATABASE);
    DB->setUserName(USER);
    DB->setPassword(PASS);
    DB->setPort(PORT);

    if(DB->open()){
        emit sig_Connection(DB->isOpen());
    }
    else{
        emit sig_SendError(DB->lastError());
    }



}


//void DataBase::DisconnectFromDB(QString DBName)
//{
//    *DB = QSqlDatabase::database(DBName);
//    DB->close();
//}

void DataBase::GetAirpoptList()
{
    QVector<QString> List;
    request = "SELECT airport_name->>'ru', airport_code FROM bookings.airports_data";
    *query = QSqlQuery(*DB);
    query->exec(request);

    QString name, code;

    while(query->next()){
        name = query->value(0).toString();
        code = query->value(1).toString();

        List.push_back(name);
        airPorts[name] = code;
    }
    List.sort();

    emit sig_GetAirportList(List);


}

void DataBase::GetDepartureFlights(QString airportName, bool isCurrentDate, QDate date)
{
    airportCode = airPorts[airportName];
    request =   "SELECT flight_no, scheduled_departure, ad.airport_name->>'ru' "
                "FROM bookings.flights f "
                "JOIN bookings.airports_data ad on ad.airport_code = f.arrival_airport "
                "WHERE f.departure_airport = '" + airportCode + "'";

    if(isCurrentDate){

        QString today = date.toString(Qt::DateFormat::ISODate);
        QString nextDay = date.addDays(1).toString(Qt::DateFormat::ISODate);

        request += " AND (scheduled_departure::date >= date('" + today + "')) AND (scheduled_departure::date < date(' " +nextDay + "'))";

    }

    *query = QSqlQuery(*DB);
    query->exec(request);

    QueryModel->setQuery(*query);
    QueryModel->setHeaderData(0, Qt::Horizontal, QObject::tr("Номер рейса"));
    QueryModel->setHeaderData(1, Qt::Horizontal, QObject::tr("Время вылета"));
    QueryModel->setHeaderData(2, Qt::Horizontal, QObject::tr("Аэропорт назначения"));

    View->setModel(QueryModel);
    emit sig_GetView(View);
}

void DataBase::GetArrivalFlights(QString airportName, bool isCurrentDate, QDate date)
{
    airportCode = airPorts[airportName];
    request =   "SELECT flight_no, scheduled_arrival, ad.airport_name->>'ru' "
                "FROM bookings.flights f "
                "JOIN bookings.airports_data ad on ad.airport_code = f.departure_airport "
                "WHERE f.arrival_airport = '" + airportCode + "'";

    if(isCurrentDate){

        QString today = date.toString(Qt::DateFormat::ISODate);
        QString nextDay = date.addDays(1).toString(Qt::DateFormat::ISODate);

        request += " AND (scheduled_arrival::date >= date('" + today + "')) AND (scheduled_arrival::date < date(' " +nextDay + "'))";

    }

    *query = QSqlQuery(*DB);
    query->exec(request);

    QueryModel->setQuery(*query);
    QueryModel->setHeaderData(0, Qt::Horizontal, QObject::tr("Номер рейса"));
    QueryModel->setHeaderData(1, Qt::Horizontal, QObject::tr("Время вылета"));
    QueryModel->setHeaderData(2, Qt::Horizontal, QObject::tr("Аэропорт отправления"));

    View->setModel(QueryModel);
    emit sig_GetView(View);
}

void DataBase::SendYearStatistic(QString airportName)
{
    airportCode = airPorts[airportName];
    request =   "SELECT count(flight_no), date_trunc('month', scheduled_departure) as Month "
                "FROM bookings.flights f "
                "WHERE (scheduled_departure::date > date('2016-08-31') "
                "AND scheduled_departure::date <= date('2017-08-31')) "
                "AND (departure_airport = '" + airportCode + "' or arrival_airport = '" + airportCode + "') "
                "GROUP BY Month";

    *query = QSqlQuery(*DB);
    query->exec(request);

    int count;
    QDate date;

    QVector<int> vectorCount;
    QVector<int> vectorDate;


    while(query->next()){
        count = query->value(0).toInt();
        date = query->value(1).toDate();

        vectorCount.push_back(count);
        vectorDate.push_back(date.month());
    }

    emit sig_SendYearStat(vectorCount, vectorDate);

}

void DataBase::SendMonthStatistic(QString airportName, int selectedMonth)
{
    airportCode = airPorts[airportName];
    selectedMonth++;

    request =   "SELECT count(flight_no), date_trunc('day', scheduled_departure) as Day "
                "FROM bookings.flights f "
                "WHERE (scheduled_departure::date > date('2016-08-31') "
                "AND scheduled_departure::date <= date('2017-08-31')) "
                "AND (departure_airport = '" + airportCode + "' or arrival_airport = '" + airportCode + "') "
                "GROUP BY Day";

    *query = QSqlQuery(*DB);
    query->exec(request);

    int count;
    QDate month;

    QVector<int> vectorCount;

    while(query->next()){

        count = query->value(0).toInt();
        month = query->value(1).toDate();

        if(month.month() == selectedMonth){
            vectorCount.push_back(count);
        }
    }

    emit sig_SendMonthStat(vectorCount);
}




