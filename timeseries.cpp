/*
 * timeseries.cpp
 *
 * Author: 208464198 Itay Carmiel
 */
#include <string>
#include <iostream>
#include <vector>
#include <map>
#include <stdexcept>
#include "timeseries.h"

using namespace std;
TimeSeries::TimeSeries() {

}
TimeSeries::TimeSeries(const char* CSVfileName) {
    maps.clear();
    load(CSVfileName);
}
void TimeSeries::load(const char* CSVfileName){
    map<string, vector<float>> temp_maps;
    vector<pair<string, vector<float>>> result;
    ifstream myFile(CSVfileName);
    // Make sure the file is open
    if (!myFile.is_open()) throw runtime_error("Could not open file");
    string line, colname;
    float val;
    // Read the column names
    if (myFile.good()) {
        // Extract the first line in the file
        getline(myFile, line);

        // Create a stringstream from line
        stringstream ss(line);

        // Extract each column name
        while (getline(ss, colname, ',')) {

            // Initialize and add <colname, int vector> pairs to result
            result.push_back({colname, vector<float>{}});
        }
    }
    // Read data, line by line
    while (getline(myFile, line)) {
        // Create a stringstream of the current line
        stringstream ss(line);

        // Keep track of the current column index
        int colIdx = 0;

        // Extract each integer
        while (ss >> val) {

            // Add the current integer to the 'colIdx' column's values vector
            result.at(colIdx).second.push_back(val);

            // If the next token is a comma, ignore it and move on
            if (ss.peek() == ',') ss.ignore();

            // Increment the column index
            colIdx++;
        }
    }
    // Close file
    myFile.close();
    for (int i = 0; i < result.size(); i++) {
        temp_maps.insert({result.at(i)});
    }
    maps = temp_maps;
}
map<string, vector<float>> TimeSeries::getMap() const {
    return maps;
}
// return the value in feature j in line i.
float TimeSeries::getValByIndex(string j, int i)const{
    int index=1;
    vector<float> v = maps.at(j);
    for (vector<float>::iterator it = v.begin() ; it != v.end(); ++it){
        if (index == i)
            return *it;
        index++;
    }
    cout<<"false combination"<<endl;
    return -1;
}

// return vector of strings the features of map.
vector<string> TimeSeries::getFeature() const{
    vector<string> veS;
    for (auto mapIt = begin(maps); mapIt != end(maps); ++mapIt) {
        veS.push_back(mapIt->first);
    }
    return veS;
}

//return vector of values in specific feature
vector<float> TimeSeries::getValues(string s)const {
    vector<float> veF;
    int flag = 0;
    for (auto mapIt = begin(maps); mapIt != end(maps); ++mapIt) {
        if (mapIt->first == s) {
            veF = mapIt->second;
            flag = 1;
            break;
        }
    }
    if (flag)
        return veF;
    else {
        cout << "string doesn't match to any feature" << endl;
        veF.push_back(-1);
        return veF;
    }
}
//assume that every columns has ame size.
int TimeSeries::numOfValue ()const{
    auto mapIt = begin(maps);
    return mapIt->second.size();
}

//add an value f to specific feature s.
void TimeSeries::addVal(string s, float f) {
    int flag = 0;
    for (auto mapIt = begin(maps); mapIt != end(maps); ++mapIt) {
        if (mapIt->first == s) {
            mapIt->second.push_back(f);
            flag = 1;
            break;
        }
    }
    if (!flag)
        cout << "string doesn't match to any feature" << endl;
}