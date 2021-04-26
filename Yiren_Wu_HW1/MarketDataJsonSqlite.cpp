#include <string>
#include <iostream>
#include <vector>
#include <stdio.h>
#include "json/json.h"
#include "curl/curl.h"
#include <sqlite3.h>

using namespace std;

class DailyTrade
{
private:
	string date;
	float open;
	float high;
	float low;
	float close;
	float adjusted_close;
	long volume;
public:
	DailyTrade(string date_, float open_, float high_, float low_, float close_, float adjusted_close_, int volume_) :
		date(date_), open(open_), high(high_), low(low_), close(close_), adjusted_close(adjusted_close_), volume(volume_)
	{}
	~DailyTrade() {}
	friend ostream & operator << (ostream & out, const DailyTrade & t)
	{
		out << "Date: " << t.date << " Open: " << t.open << " High: " << t.high << " Low: " << t.low << " Close: " << t.close << " Adjusted_Close: "<< t.adjusted_close << " Volume: " << t.volume << endl;
		return out;
	}
};

class Stock
{
private:
	string symbol;
	vector<DailyTrade> trades;

public:
	Stock(string symbol_) :symbol(symbol_)
	{}
	~Stock() {}
	void addTrade(DailyTrade aTrade)
	{
		trades.push_back(aTrade);
	}
	friend ostream & operator << (ostream & out, const Stock & s)
	{
		out << "Symbol: " << s.symbol << endl;
		for (vector<DailyTrade>::const_iterator itr = s.trades.begin(); itr != s.trades.end(); itr++)
			out << *itr;
		return out;
	}
};

//writing call back function for storing fetched values in memory
static size_t WriteCallback(void *contents, size_t size, size_t nmemb, void *userp)
{
	((string*)userp)->append((char*)contents, size * nmemb);
	return size * nmemb;
}

int main(void)
{
	int rc = 0;
	char* error = nullptr;

	// Open Database
	cout << "Opening Stocks.db ..." << endl;
	sqlite3* db;
	rc = sqlite3_open("Stocks.db", &db);
	if (rc)
	{
		cerr << "Error opening SQLite3 database: " << sqlite3_errmsg(db) << endl << endl;
		sqlite3_close(db);
		return -1;
	}
	else
	{
		cout << "Opened Stocks.db." << endl << endl;
	}

	// Drop the table if exists
	string sql_dropatable = "DROP TABLE IF EXISTS DailyTrades";
	if (sqlite3_exec(db, sql_dropatable.c_str(), 0, 0, 0) != SQLITE_OK) {
		cout << "SQLite can't drop DailyTrade table" << std::endl;
		sqlite3_close(db);
		return -1;
	}

	// Create the table
	cout << "Creating DailyTrades table ..." << endl;
	const char* sqlCreateTable = "CREATE TABLE DailyTrades (" \
		"symbol CHAR(20) NOT NULL," \
		"date CHAR(20) NOT NULL," \
		"open REAL NOT NULL," \
		"high REAL NOT NULL," \
		"low REAL NOT NULL," \
		"close REAL NOT NULL," \
		"adjusted_close REAL NOT NULL," \
		"volume INT NOT NULL," \
		"PRIMARY KEY(symbol, date));";

	rc = sqlite3_exec(db, sqlCreateTable, NULL, NULL, &error);
	if (rc)
	{
		cerr << "Error executing SQLite3 statement: " << sqlite3_errmsg(db) << endl << endl;
		sqlite3_free(error);
	}
	else
	{
		cout << "Created DailyTrades table." << endl << endl;
	}

	string readBuffer;

	//global initiliation of curl before calling a function
	curl_global_init(CURL_GLOBAL_ALL);

	//creating session handle
	CURL * handle;

	// Store the result of CURL’s webpage retrieval, for simple error checking.
	CURLcode result;

	// notice the lack of major error-checking, for brevity
	handle = curl_easy_init();

	//after creating handle we will start transfering webpage
	//curl_easy_setopt is used to tell libcurl how to behave.
	//By setting the appropriate options, the application can change libcurl's behavior.

	string url_common = "https://eodhistoricaldata.com/api/eod/";

	string symbol = "MSFT";
	string start_date = "2021-01-01";
	string end_date = "2021-03-30";
	string api_token = "601390a0a753f2.12891709";
	string url_request = url_common + symbol + ".US?" + "from=" + start_date + "&to=" + end_date + "&api_token=" + api_token + "&period=d&fmt=json";
	curl_easy_setopt(handle, CURLOPT_URL, url_request.c_str());
	
	//adding a user agent
	curl_easy_setopt(handle, CURLOPT_USERAGENT, "Mozilla/5.0 (Windows NT 10.0; Win64; x64; rv:74.0) Gecko/20100101 Firefox/74.0");
	curl_easy_setopt(handle, CURLOPT_SSL_VERIFYPEER, 0);
	curl_easy_setopt(handle, CURLOPT_SSL_VERIFYHOST, 0);

	// send all data to this function 
	curl_easy_setopt(handle, CURLOPT_WRITEFUNCTION, WriteCallback);

	// we pass our 'chunk' struct to the callback function 
	curl_easy_setopt(handle, CURLOPT_WRITEDATA, &readBuffer);

	//perform a blocking file transfer
	result = curl_easy_perform(handle);

	// check for errors 
	if (result != CURLE_OK) {
		fprintf(stderr, "curl_easy_perform() failed: %s\n", curl_easy_strerror(result));
	}
	else
	{
		//json parsing
		Json::CharReaderBuilder builder;
		Json::CharReader* reader = builder.newCharReader();
		Json::Value root;   // will contains the root value after parsing.
		string errors;

		bool parsingSuccessful = reader->parse(readBuffer.c_str(), readBuffer.c_str() + readBuffer.size(), &root, &errors);
		if (not parsingSuccessful)
		{
			// Report failures and their locations in the document.
			cout << "Failed to parse JSON" << endl << readBuffer << errors << endl;
			return -1;
		}
		else
		{
			cout << "\nSucess parsing json\n" << root << endl;
			string date;
			float open, high, low, close, adjusted_close;
			long volume;
			Stock myStock(symbol);
			for (Json::Value::const_iterator itr = root.begin(); itr != root.end(); itr++) // iterate the Json list
			{
				date = (*itr)["date"].asString();	// read value from Json Object, it is a C++ map
				open = (*itr)["open"].asFloat();
				high = (*itr)["high"].asFloat();
				low = (*itr)["low"].asFloat();
				close = (*itr)["close"].asFloat();
				adjusted_close = (*itr)["adjusted_close"].asFloat();
				volume = (*itr)["volume"].asInt64();
				DailyTrade aTrade(date, open, high, low, close, adjusted_close, volume);
				myStock.addTrade(aTrade);

				// Execute SQL
				cout << "Inserting a value into table DailyTrades ..." << endl;
				char sqlInsert[512];
				sprintf_s(sqlInsert, "INSERT INTO DailyTrades(symbol, date, open, high, low, close, adjusted_close, volume) VALUES(\"%s\", \"%s\", %f, %f, %f, %f, %f, %d)", symbol.c_str(), date.c_str(), open, high, low, close, adjusted_close, volume);
				rc = sqlite3_exec(db, sqlInsert, NULL, NULL, &error);
				if (rc)
				{
					cerr << "Error executing SQLite3 statement: " << sqlite3_errmsg(db) << endl << endl;
					sqlite3_free(error);
				}
				else
				{
					cout << "Inserted a value into the table DailyTrades." << endl << endl;
				}
			}
			cout << myStock;
		}
	}
	curl_easy_cleanup(handle);

	// Display MyTable
	cout << "Retrieving values in table DailyTrades ..." << endl;
	const char* sqlSelect = "SELECT * FROM DailyTrades;";
	char** results = NULL;
	int rows, columns;
	sqlite3_get_table(db, sqlSelect, &results, &rows, &columns, &error);
	if (rc)
	{
		cerr << "Error executing SQLite3 query: " << sqlite3_errmsg(db) << endl << endl;
		sqlite3_free(error);
	}
	else
	{
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
	}
	sqlite3_free_table(results);

	// Close Database
	cout << "Closing Stocks.db ..." << endl;
	sqlite3_close(db);
	cout << "Closed Stocks.db" << endl << endl;

	return 0;
}