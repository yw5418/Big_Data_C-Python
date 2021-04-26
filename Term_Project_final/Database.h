
#ifndef LAB1_DATABASE_H
#define LAB1_DATABASE_H

#pragma once
#include <sqlite3.h>
#include <string>
#include "Stock.h"
#include <stdio.h>

class Stock;


int OpenDatabase(sqlite3*& db)
{
    int rc = 0;
    char* error = nullptr;

    // Open Database
    cout << "Opening PairTrading.db ..." << endl;
    rc = sqlite3_open("PairTrading.db", &db);
    if (rc)
    {
        cerr << "Error opening SQLite3 database: " << sqlite3_errmsg(db) << endl << endl;
        sqlite3_close(db);
        return -1;
    }
    else
    {
        cout << "Opened PairTrading.db." << endl << endl;
    }

    return rc;
}

int CreateTables(sqlite3*& db)
{
    char* error = nullptr;
    int rc1 = 0;
    int rc2 = 0;
    int rc3 = 0;
    int rc4 = 0;


    const char* sqlCreateTable1 = "CREATE TABLE PairOnePrices (" \
      "Symbol CHAR(20) NOT NULL," \
      "Date CHAR(20) NOT NULL," \
      "Open REAL NOT NULL," \
      "High REAL NOT NULL," \
      "Low REAL NOT NULL," \
      "Close REAL NOT NULL," \
      "Adjusted_close REAL NOT NULL," \
      "volume INT NOT NULL," \
      "PRIMARY KEY(Symbol, Date));";

        const char* sqlCreateTable2 = "CREATE TABLE PairTwoPrices (" \
      "Symbol CHAR(20) NOT NULL," \
      "Date CHAR(20) NOT NULL," \
      "Open REAL NOT NULL," \
      "High REAL NOT NULL," \
      "Low REAL NOT NULL," \
      "Close REAL NOT NULL," \
      "Adjusted_close REAL NOT NULL," \
      "volume INT NOT NULL," \
      "PRIMARY KEY(Symbol, Date));";

        const char* sqlCreateTable3 = "CREATE TABLE StockPairs (" \
      "Symbol1 CHAR(20) NOT NULL," \
      "Symbol2 CHAR(20) NOT NULL,"\
      "Volatility REAL," \
      "Profit_loss REAL," \
      "PRIMARY KEY(Symbol1, Symbol2));";


        const char* sqlCreateTable4 = "CREATE TABLE PairPrices (" \
      "Symbol1 CHAR(20) NOT NULL," \
      "Symbol2 CHAR(20) NOT NULL,"\
      "Date CHAR(20) NOT NULL," \
      "Open1 REAL NOT NULL," \
      "Close1 REAL NOT NULL," \
      "Open2 REAL NOT NULL," \
      "Close2 REAL NOT NULL," \
      "Profit_loss REAL NOT NULL," \
      "PRIMARY KEY(Symbol1, Symbol2, Date),"\
      "Foreign Key(Symbol1, Date) references PairOnePrices(Symbol, Date)\n"\
      "Foreign Key(Symbol2, Date) references PairTwoPrices(Symbol, Date)\n"\
      "Foreign Key(Symbol1, Symbol2) references StockPairs(Symbol1, Symbol2)\n"\
      "ON DELETE CASCADE\n"\
      "ON UPDATE CASCADE);";
    
    
    rc1 = sqlite3_exec(db, sqlCreateTable1, NULL, NULL, &error);
    if (rc1)
    {
        cerr << "Error executing SQLite3 statement: " << sqlite3_errmsg(db) << endl << endl;
        sqlite3_free(error);
    }
    else
    {
        cout << "Created PairOnePrices table." << endl << endl;
    }

    rc2 = sqlite3_exec(db, sqlCreateTable2, NULL, NULL, &error);
    if (rc2)
    {
        cerr << "Error executing SQLite3 statement: " << sqlite3_errmsg(db) << endl << endl;
        sqlite3_free(error);
    }
    else
    {
        cout << "Created PairTwoPrices table." << endl << endl;
    }


    rc3 = sqlite3_exec(db, sqlCreateTable3, NULL, NULL, &error);
    if (rc3)
    {
        cerr << "Error executing SQLite3 statement: " << sqlite3_errmsg(db) << endl << endl;
        sqlite3_free(error);
    }
    else
    {
        cout << "Created StockPairs table." << endl << endl;
    }

    rc4 = sqlite3_exec(db, sqlCreateTable4, NULL, NULL, &error);
    if (rc4)
    {
        cerr << "Error executing SQLite3 statement: " << sqlite3_errmsg(db) << endl << endl;
        sqlite3_free(error);
    }
    else
    {
        cout << "Created PairPrices table." << endl << endl;
    }


    return 0;


}

//writing call back function for storing fetched values in memory
static size_t WriteCallback(void *contents, size_t size, size_t nmemb, void *userp)
{
    ((string*)userp)->append((char*)contents, size * nmemb);
    return size * nmemb;
}

static size_t ctr = 0;

int InsertTable_PairOne(sqlite3* db, const Stock& stock) {
    char *error = nullptr;
    int rc;


    cout << "Inserting a value into table PairOnePrices ..." << endl;
    for (vector<DailyTrade>::const_iterator itr = stock.GetDailyTrade().begin();
         itr != stock.GetDailyTrade().end(); itr++) {
        char sqlInsert[512];
        sprintf(sqlInsert,
                "INSERT INTO PairOnePrices(symbol, date, open, high, low, close, adjusted_close, volume) VALUES(\"%s\", \"%s\", %f, %f, %f, %f, %f, %d)",
                stock.GetSymbol().c_str(), (*itr).GetDate().c_str(),
                (*itr).GetOpen(), (*itr).GetHigh(), (*itr).GetLow(), (*itr).GetClose(), (*itr).GetAdjustedClose(),
                (*itr).GetVolume());

        rc = sqlite3_exec(db, sqlInsert, NULL, NULL, &error);
        if (error) { cout << error; }
    }

        return 0;


}


int InsertTable_PairTwo(sqlite3* db, const Stock& stock) {
    char *error = nullptr;
    int rc;


    cout << "Inserting a value into table PairTwoPrices ..." << endl;
    for (vector<DailyTrade>::const_iterator itr = stock.GetDailyTrade().begin();
         itr != stock.GetDailyTrade().end(); itr++) {
        char sqlInsert[512];
        sprintf(sqlInsert,
                "INSERT INTO PairTwoPrices(symbol, date, open, high, low, close, adjusted_close, volume) VALUES(\"%s\", \"%s\", %f, %f, %f, %f, %f, %d)",
                stock.GetSymbol().c_str(), (*itr).GetDate().c_str(),
                (*itr).GetOpen(), (*itr).GetHigh(), (*itr).GetLow(), (*itr).GetClose(), (*itr).GetAdjustedClose(),
                (*itr).GetVolume());
        rc = sqlite3_exec(db, sqlInsert, NULL, NULL, &error);
        if (error)
            cout << error;
    }

        return 0;

}



int InsertTable_StockPairs(sqlite3* db, const vector<string> pair1, const vector<string> pair2) {
    char *error = nullptr;
    int rc;

    cout << "Inserting a value into table StockPairs ..." << endl;
    for (int i = 0; i < pair1.size(); i++) {
        char sqlInsert[512];
        sprintf(sqlInsert,
                "INSERT INTO StockPairs(Symbol1, Symbol2, Volatility, Profit_loss) VALUES(\"%s\", \"%s\", %f, %f)",
                pair1[i].c_str(), pair2[i].c_str(),
                0.0, 0.0);

        rc = sqlite3_exec(db, sqlInsert, NULL, NULL, &error);
        if (error)
            cout << error;
    }

    return 0;

}


int InsertTable_PairPrices(sqlite3* db) {
    char *error = nullptr;
    int rc;
    char sqlInsert[512];
    cout << "Inserting a value into table PairPrices ..." << endl;
    sprintf(sqlInsert,"Insert into PairPrices \n"
                      "\tSelect \tStockPairs.symbol1 as symbol1, StockPairs.symbol2 as symbol2,\n"
                      "\t\t\tPairOnePrices.date as date, \n"
                      "\t\t\tPairOnePrices.open as open1, PairOnePrices.close as close1, \n"
                      "\t\t\tPairTwoPrices.open as open2, PairTwoPrices.close as close2, \n"
                      "\t\t\t0 as profit_loss \n"
                      "\tFrom StockPairs, PairOnePrices, PairTwoPrices \n"
                      "\tWhere (((StockPairs.symbol1 = PairOnePrices.symbol) and \n"
                      "\t\t\t(StockPairs.symbol2 = PairTwoPrices.symbol)) and \n"
                      "\t\t\t(PairOnePrices.date = PairTwoPrices.date)) \n"
                      "\tORDER BY symbol1, symbol2;");
//    string sqlInsert = string("Insert into PairPrices") + "(Select StockPairs.symbol1 as symbol1, StockPairs.symbol2 as symbol2, PairOnePrices.date as date, PairOnePrices.open as open1,"
//            + "PairOnePrices.close as close1, PairTwoPrices.open as open2, PairTwoPrices.close as close2, 0 as profit_loss)";
//
//


    rc = sqlite3_exec(db, sqlInsert, NULL, NULL, &error);
    if (error)
        cout << error;


    return 0;

}

int CalculateVol(sqlite3* db, string back_test_start_date) {
    char *error = nullptr;
    int rc;
    char sqlInsert[512];
    cout << "Calculate Volatility ..." << endl;
    string a;

    sprintf(sqlInsert, "Update StockPairs SET volatility = (SELECT(AVG((Close1/Close2)*(Close1/Close2)) - AVG(Close1/Close2)*AVG(Close1/Close2)) as variance FROM PairPrices WHERE StockPairs.symbol1 = PairPrices.symbol1 AND StockPairs.symbol2 = PairPrices.symbol2 AND PairPrices.date <= '%s')", back_test_start_date.c_str());

    cout << back_test_start_date;


    rc = sqlite3_exec(db, sqlInsert, NULL, NULL, &error);
    if (error)
        cout << error;


    return 0;

}



int DisplayTables(sqlite3* db, const char *sqlSelect)
{
    // Display MyTable
    char* error = nullptr;
    char** results = NULL;
    int rows, columns;

    sqlite3_get_table(db, sqlSelect, &results, &rows, &columns, &error);

    // Display Table
    for (int rowCtr = 0; rowCtr <= rows; ++rowCtr)
    {
        for (int colCtr = 0; colCtr < columns; ++colCtr)
        {
            // Determine Cell Position
            int cellPosition = (rowCtr * columns) + colCtr;

            // Display Cell Value
            cout.width(12);
            cout.setf(ios::left);
            cout << results[cellPosition] << " ";
        }

        // End Line
        cout << endl;

        // Display Separator For Header
        if (0 == rowCtr)
        {
            for (int colCtr = 0; colCtr < columns; ++colCtr)
            {
                cout.width(12);
                cout.setf(ios::left);
                cout << "~~~~~~~~~~~~ ";
            }
            cout << endl;
        }
    }

    sqlite3_free_table(results);
    return 0;
};

int BackTest(sqlite3* db, string end_date,double k)
{
    char* error = nullptr;
    int rc;
    char sqlInsert[2048];
    cout << "Back Testing ..." << endl;
    
    sprintf(sqlInsert,"create table T(s1 char[20],s2 char[20] , d char[20], df REAL )");     // create table T
    rc = sqlite3_exec(db, sqlInsert, NULL, NULL, &error);
    if (error)
        cout << error;
    
    sprintf(sqlInsert, "INSERT into T select A.symbol1, A.symbol2, A.Date, abs((B.close1/B.close2)-(A.open1/A.open2)) as diff_open from PairPrices A join PairPrices B where A.ROWID==B.ROWID+1 and A.Symbol1==B.Symbol1 AND A.Symbol2==B.Symbol2 ");
    rc = sqlite3_exec(db, sqlInsert, NULL, NULL, &error);   // insert table T
    if (error)
        cout << error;

    
    sprintf(sqlInsert, "ALTER TABLE T ADD mult INT;");    //add int column
    rc = sqlite3_exec(db, sqlInsert, NULL, NULL, &error);
    if (error)
        cout << error;

   
    sprintf(sqlInsert, "UPDATE  T SET mult= CASE when T.df  > (select Volatility from StockPairs where StockPairs.Symbol1=T.s1 and StockPairs.Symbol2=T.s2) then 1 ELSE -1 END");
    rc = sqlite3_exec(db, sqlInsert, NULL, NULL, &error);
    if (error)
        cout << error;


   
    sprintf(sqlInsert, "UPDATE PairPrices SET  Profit_loss = (SELECT  ((P.Open1/P.Open2)*P.Close2-P.Close1)*10000*T.mult as p_l from PairPrices  P , T  where T.s1==P.Symbol1 and T.s2==P.Symbol2 and T.d==P.Date and p_l is not null and T.s1==PairPrices.Symbol1 AND T.s2==PairPrices.Symbol2 AND T.d ==PairPrices.Date ) WHERE EXISTS(SELECT mult FROM T WHERE T.s1==PairPrices.Symbol1 AND T.s2==PairPrices.Symbol2 AND T.d ==PairPrices.Date AND mult is not null);");
    rc = sqlite3_exec(db, sqlInsert, NULL, NULL, &error);
    if (error)
        cout << error;


    return 0;

}


int CalculatePnL(sqlite3* db, string back_test_start_date) {
    char* error = nullptr;
    int rc;
    char sqlInsert[512];
    cout << "Calculating ..." << endl;
    string a;

    sprintf(sqlInsert, "Update StockPairs SET Profit_loss = (SELECT(SUM(Profit_loss)) as pnl FROM PairPrices WHERE StockPairs.symbol1 = PairPrices.symbol1 AND StockPairs.symbol2 = PairPrices.symbol2 AND PairPrices.date > '%s')", back_test_start_date.c_str());

    rc = sqlite3_exec(db, sqlInsert, NULL, NULL, &error);
    if (error)
        cout << error;

    return 0;

}

int InsertNewValue(sqlite3* db, string table_name, string symbol, string date, double open, double close) {
    char *error = nullptr;
    int rc;
    char sqlInsert[512];
    cout << "Insert new value ..." << endl;
    string a;
    sprintf(sqlInsert, "INSERT INTO '%s' (symbol, date, open, high, low, close, adjusted_close, volume) VALUES(\"%s\", \"%s\", %f, %f, %f, %f, %f, %d)",
            table_name.c_str(), symbol.c_str(), date.c_str(), open, 0.0, 0.0, close, 0.0, 0);

    rc = sqlite3_exec(db, sqlInsert, NULL, NULL, &error);
    if (error)
        cout << error;


    return 0;

}
#endif //LAB1_DATABASE_H
