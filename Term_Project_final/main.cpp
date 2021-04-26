#include <string>
#include <iostream>
#include <vector>
#include <stdio.h>
#include <regex>

#include "json/json.h"
#include "curl/curl.h"
#include <sqlite3.h>
#include "Stock.h"
#include "Database.h"
#include "MarketData.h"

using namespace std;



// Get current date/time, format is YYYY-MM-DD.HH:mm:ss
const std::string currentDateTime() {
    time_t     now = time(0);
    struct tm  tstruct;
    char       buf[80];
    tstruct = *localtime(&now);
    // Visit http://en.cppreference.com/w/cpp/chrono/c/strftime
    // for more information about date/time format
    strftime(buf, sizeof(buf), "%Y-%m-%d", &tstruct);
    std::string str(buf);

    return str;
}

int main() {

    fstream fin;
    std::regex csvParser("([A-Z]+),([A-Z]+)");
    std::smatch csvParseMatch;

    fin.open("PairTrading.csv", ios::in);
    string symbol;
    string start_date = "2011-01-01";
    string back_test_start_date = "2020-12-31";
    string end_date = "2021-03-12";

    vector<string> PairOneTickers;
    vector<string> PairTwoTickers;
    map<string, string> StockPairs;
    string url_common_daily = "https://eodhistoricaldata.com/api/eod/";
    string api_token = "6019b0c04b5e63.43913811";

    ////////// Get All Ticker///////////////////////////
    std::string line;


    while (getline(fin, line)) {
        const string csvLine = line;
        cout << line;
        cout << 1 << endl;
        if (regex_search(csvLine.begin(), csvLine.end(), csvParseMatch, csvParser)) {
            PairOneTickers.push_back((string) csvParseMatch[1].str());
            PairTwoTickers.push_back((string) csvParseMatch[2].str());
        }

    }
    sqlite3 *db = NULL;
    string read_buffer;
    if (OpenDatabase(db) != 0) return -1;
    bool bCompleted = false;
    char selection;
    map<string, Stock *> stockMapPair1;
    map<string, Stock *> stockMapPair2;
    string url_request;

    while (!bCompleted) {

        cout << endl;
        cout << "Menu" << endl;
        cout << "==========" << endl;
        cout << "A - Create and Populate Table" << endl;
        cout << "B - Retrieve and Populate Historical Data for Each Stock" << endl;
        cout << "C - Create PairPrices table" << endl;
        cout << "D - Calculate volatility" << endl;
        cout << "E - Back Test" << endl;
        cout << "F - Calculate Profit and Loss for Each Pair" << endl;
        cout << "G - Manual Testing" << endl;
        cout << "H - Drop All the Tables" << endl;
        cout << "X - Exit" << endl;

        cout << "Please enter a letter" << endl;
        char menu;
        cin >> menu;

        switch (menu) {
            case 'A': {
                CreateTables(db);
                break;
            }

            case 'B': {
                size_t ctr1 = 0;
                for (vector<string>::const_iterator itr = PairOneTickers.begin(); itr != PairOneTickers.end(); itr++) {
                    symbol = *itr;
                    Stock *mystock = new Stock(symbol);

                    url_request = url_common_daily + symbol + ".US?" + "from=" + start_date + "&to=" + end_date +
                                  "&api_token=" + api_token + "&period=d&fmt=json";
                    PullMarketData(url_request, read_buffer);
                    PopulateDailyTrades(read_buffer, *mystock);
                    read_buffer = ""; // Clean read_buffer after populating


                    stockMapPair1.insert(std::make_pair(symbol, mystock));
                    //  cout << "Iteration " << ++ctr1 << "\t" << "size\t" << stockMapPair1.size() << "\n";
                }


                for (vector<string>::const_iterator itr = PairTwoTickers.begin(); itr != PairTwoTickers.end(); itr++) {

                    symbol = *itr;
                    Stock *mystock = new Stock(symbol);

                    url_request = url_common_daily + symbol + ".US?" + "from=" + start_date + "&to=" + end_date +
                                  "&api_token=" + api_token + "&period=d&fmt=json";
                    PullMarketData(url_request, read_buffer);
                    PopulateDailyTrades(read_buffer, *mystock);
                    read_buffer = ""; // Clean read_buffer after populating


                    stockMapPair2.insert(std::make_pair(symbol, mystock));
                }


                for (map<string, Stock *>::const_iterator itr = stockMapPair1.begin();
                     itr != stockMapPair1.end(); itr++) {

                    InsertTable_PairOne(db, *(itr->second));
                }

                for (map<string, Stock *>::const_iterator itr = stockMapPair2.begin();   // B stand for Insert value into pairtwo prices
                     itr != stockMapPair2.end(); itr++) {

                    InsertTable_PairTwo(db, *(itr->second));
                }


                break;
            }

            case 'C': {

                InsertTable_StockPairs(db, PairOneTickers, PairTwoTickers);
                InsertTable_PairPrices(db);

                break;
            }

            case 'D': {

                CalculateVol(db, back_test_start_date);

                break;
            }
            case 'E': {
                int k = 1;
                BackTest(db, end_date, k);
                break;
            }
            case 'F': {
                CalculatePnL(db, back_test_start_date);
                break;
            }

            case 'G': {
                string symbol1, symbol2;
                float close1d1, close2d1, open1d2, open2d2, close1d2, close2d2;
                char *error = nullptr;
                int k;

                char *sqlSelect1 = "select Symbol1, Symbol2 from StockPairs";
                DisplayTables(db, sqlSelect1);

                cout << "Enter Symbol1" << endl;
                cin >> symbol1;
                cout << "Enter Close1d1:" << endl;
                cin >> close1d1;
                cout << "Enter Open1d2:" << endl;
                cin >> open1d2;
                cout << "Enter Close1d2" << endl;
                cin >> close1d2;

                cout << "Enter Symbol2" << endl;
                cin >> symbol2;
                cout << "Enter Close2d1:" << endl;
                cin >> close2d1;
                cout << "Enter Open2d2:" << endl;
                cin >> open2d2;
                cout << "Enter Close2d2" << endl;
                cin >> close2d2;

                cout << "Enter k:" << endl;
                cin >> k;


                char sql[512];
                sprintf(sql, "Select volatility from StockPairs where Symbol1 = '%s' and symbol2 = '%s';",
                        symbol1.c_str(), symbol2.c_str());

                char **results = NULL;
                int rows, columns;
                sqlite3_get_table(db, sql, &results, &rows, &columns, &error);
                if (error) {
                    cout << error;
                }
                float sigma = std::stof(results[1]);
                bool Short;


                if (std::abs(close1d1 / close2d1 - open1d2 / open2d2) > k * sigma) {
                    Short = true;
                } else {
                    Short = false;
                }

                int N1 = 10000;
                int N2 = N1 * (open1d2 / open2d2);
                float profit;

                if (Short) {
                    profit = -N1 * (open1d2 - close1d2) + N2 * (open2d2 - close2d2);
                    cout << "Short the pair " << endl;
                } else {
                    profit = N1 * (open1d2 - close1d2) - N2 * (open2d2 - close2d2);
                    cout << "Long the pair " << endl;
                }

                cout << "Profit = " << profit << endl;
                break;
            }

            case 'H': {
                string sql_dropatable1 = "DROP TABLE IF EXISTS PairOnePrices";
                if (sqlite3_exec(db, sql_dropatable1.c_str(), 0, 0, 0) != SQLITE_OK) {
                    cout << "SQLite can't drop PairOnePrices table" << std::endl;
                    sqlite3_close(db);
                    return -1;
                }

                string sql_dropatable2 = "DROP TABLE IF EXISTS PairTwoPrices";
                if (sqlite3_exec(db, sql_dropatable2.c_str(), 0, 0, 0) != SQLITE_OK) {
                    cout << "SQLite can't drop PairTwoPrices table" << std::endl;
                    sqlite3_close(db);
                    return -1;
                }
                string sql_dropatable3 = "DROP TABLE IF EXISTS PairPrices";
                if (sqlite3_exec(db, sql_dropatable3.c_str(), 0, 0, 0) != SQLITE_OK) {
                    cout << "SQLite can't drop PairPrices table" << std::endl;
                    sqlite3_close(db);
                    return -1;
                }
                string sql_dropatable4 = "DROP TABLE IF EXISTS StockPairs";
                if (sqlite3_exec(db, sql_dropatable4.c_str(), 0, 0, 0) != SQLITE_OK) {
                    cout << "SQLite can't drop StockPairs table" << std::endl;
                    sqlite3_close(db);
                    return -1;
                }
                string sql_dropatable5 = "DROP TABLE IF EXISTS T";
                if (sqlite3_exec(db, sql_dropatable5.c_str(), 0, 0, 0) != SQLITE_OK) {
                    cout << "SQLite can't drop T table" << std::endl;
                    sqlite3_close(db);
                    return -1;
                }
                break;
            }

            case 'X': {
                exit(0);
            }
        }


    }
return 0;
}