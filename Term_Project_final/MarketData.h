//
// Created by YIREN WU on 2/17/21.
//

#ifndef LAB1_MARKETDATA_H
#define LAB1_MARKETDATA_H
#pragma once
#include <string>
#include "Stock.h"

class Stock;
int PullMarketData(const std::string& url_request, std::string& read_buffer)
{

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
    curl_easy_setopt(handle, CURLOPT_URL, url_request.c_str());

    //adding a user agent
    curl_easy_setopt(handle, CURLOPT_USERAGENT, "Mozilla/5.0 (Windows NT 10.0; Win64; x64; rv:74.0) Gecko/20100101 Firefox/74.0");
    curl_easy_setopt(handle, CURLOPT_SSL_VERIFYPEER, 0);
    curl_easy_setopt(handle, CURLOPT_SSL_VERIFYHOST, 0);

    // send all data to this function
    curl_easy_setopt(handle, CURLOPT_WRITEFUNCTION, WriteCallback);

    // we pass our 'chunk' struct to the callback function
    curl_easy_setopt(handle, CURLOPT_WRITEDATA, &read_buffer);

    //perform a blocking file transfer
    result = curl_easy_perform(handle);
    curl_easy_cleanup(handle);

    return 0;

}



int PopulateDailyTrades(const std::string& read_buffer, Stock& stock)
{

    //json parsing
    Json::CharReaderBuilder builder;
    Json::CharReader* reader = builder.newCharReader();
    Json::Value root;   // will contains the root value after parsing.
    string errors;
    bool parsingSuccessful = reader->parse(read_buffer.c_str(), read_buffer.c_str() + read_buffer.size(), &root, &errors);

    string date;
    float open, high, low, close, adjusted_close;
    long volume;

    for (Json::Value::const_iterator itr = root.begin(); itr != root.end(); itr++)
    {
        date = (*itr)["date"].asString();
        open = (*itr)["open"].asFloat();
        high = (*itr)["high"].asFloat();
        low = (*itr)["low"].asFloat();
        close = (*itr)["close"].asFloat();
        adjusted_close = (*itr)["adjusted_close"].asFloat();
        volume = (*itr)["volume"].asInt64();
        DailyTrade aTrade(date, open, high, low, close, adjusted_close, volume);
        stock.addDailyTrade(aTrade);

    }

    return 0;
}




#endif //LAB1_MARKETDATA_H
