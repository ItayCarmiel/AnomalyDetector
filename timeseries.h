/*
 * timeseries.h
 *
 * Author: 208464198 Itay Carmiel
 */
#ifndef TIMESERIES_H_
#define TIMESERIES_H_
#include <string>
#include <fstream>
#include <iostream>
#include <vector>
#include <map>
#include <stdexcept>
#include <sstream>

using namespace std;

class TimeSeries{
private:
    map<string, vector<float>> maps;

public:
    TimeSeries();
    TimeSeries(const char* CSVfileName);
    void load (const char* CSVfileName);
    map<string, vector<float>> getMap()const ;
    // return the value in feature j in line i.
    float getValByIndex(string j, int i)const;
    // return vector of strings the features of map.
    vector<string> getFeature()const;
    //return vector of values in specific feature.
    vector<float> getValues(string s)const;
    //add an value f to specific feature s.
    void addVal(string s, float f);
    //assume that every columns has ame size.
    int numOfValue ()const;

};
#endif /* TIMESERIES_H_ */