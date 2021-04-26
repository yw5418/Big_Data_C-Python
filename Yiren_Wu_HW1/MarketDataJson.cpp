#include <string>
#include <iostream>
#include <vector>
#include <stdio.h>
#include "json/json.h"
#include "curl/curl.h"
#include <sqlite3.h>
#include <fstream>

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
    DailyTrade(string date_, float open_, float high_, float low_, float close_, float adjusted_close_, long volume_) :
            date(date_), open(open_), high(high_), low(low_), close(close_), adjusted_close(adjusted_close_), volume(volume_)
    {}
    ~DailyTrade() {}
    friend ostream & operator << (ostream & out, const DailyTrade & t)
    {
        out << "Date: " << t.date << " Open: " << t.open << " High: " << t.high << " Low: " << t.low << " Close: " << t.close << " Adjusted_Close: "<< t.adjusted_close << " Volume: " << t.volume << endl;
        return out;
    }
};


class IntradayTrade{

private:
    long timestamp;
    string datetime;
    float open;
    float high;
    float low;
    float close;
    int volume;

public:
    IntradayTrade(long timestamp_, string datetime_, float open_, float high_, float low_, float close_, int volume_): timestamp(timestamp_), datetime(datetime_), open(open_), high(high_), low(low_), close(close_), volume(volume_){}
    ~IntradayTrade() {}
    friend ostream& operator << (ostream& out, const IntradayTrade& t)
    {
        out << " Datetime: " << (t.datetime) << " Open: " << t.open << " High: " << t.high << " Low: " << t.low << " Close: " << t.close << " Volume: " << t.volume << endl;

        return out;

    }

    friend ofstream& operator << (ofstream& fout, const IntradayTrade& t)
    {
        fout << " Datetime: " << (t.datetime) << " Open: " << t.open << " High: " << t.high << " Low: " << t.low << " Close: " << t.close << " Volume: " << t.volume << endl;

        return fout;
    }


};

template <typename T>
class Stock
{
private:
    string symbol;
    vector<T> trades;

public:
    Stock(string symbol_) :symbol(symbol_)
    {}
    ~Stock() {}


    void addTrade(T aTrade)
    {
        trades.push_back(aTrade);
    }

    friend ostream & operator << (ostream & out, const Stock & s)
    {
        out << "Symbol: " << s.symbol << endl;
        for (typename vector<T>::const_iterator itr = s.trades.begin(); itr != s.trades.end(); itr++)
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
    string readBuffer;

    //global initiliation of curl before calling a function
    curl_global_init(CURL_GLOBAL_ALL);

    //creating session handle
    CURL * handle;

    // Store the result of CURL�s webpage retrieval, for simple error checking.
    CURLcode result;

    // notice the lack of major error-checking, for brevity
    handle = curl_easy_init();

    //after creating handle we will start transfering webpage
    //curl_easy_setopt is used to tell libcurl how to behave.
    //By setting the appropriate options, the application can change libcurl's behavior.

    string url_common = "https://eodhistoricaldata.com/api/intraday/";

    string symbol = "MSFT";
    string start_date = "1601510400";
    string end_date = "1609372800";
    string api_token = "6019b0c04b5e63.43913811";
    string url_request = url_common + symbol + ".US?" + "from=" + start_date + "&to=" + end_date + "&api_token=" + api_token + "&interval=5m&fmt=json";
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
        cout << readBuffer.size();
        if (not parsingSuccessful)
        {
            // Report failures and their locations in the document.
            cout << "Failed to parse JSON" << endl << readBuffer << errors << endl;
            return -1;
        }
        else
        {
            //cout << "\nSucess parsing json\n" << root << endl;
            long timestamp;
            string datetime;
            float open, high, low, close, adjusted_close;
            long volume;
            Stock<IntradayTrade> myStock(symbol);
            ofstream myfile;
            myfile.open("Intraday.txt");
            for (Json::Value::const_iterator itr = root.begin(); itr != root.end(); itr++)
            {
                timestamp = (*itr)["timestamp"].asFloat();
                time_t t = timestamp;
                struct tm*tm = localtime(&t);
                datetime = asctime(tm);
//                sprintf(datetime, "%s", asctime(tm));

                open = (*itr)["open"].asFloat();
                high = (*itr)["high"].asFloat();
                low = (*itr)["low"].asFloat();
                close = (*itr)["close"].asFloat();
                volume = (*itr)["volume"].asInt64();
                IntradayTrade aTrade(timestamp,datetime ,open, high, low, close, volume);
                myStock.addTrade(aTrade);
            }
            //cout << myStock;
            myfile << myStock;
            myfile.close();
        }

    }

    curl_easy_cleanup(handle);
    return 0;
}

