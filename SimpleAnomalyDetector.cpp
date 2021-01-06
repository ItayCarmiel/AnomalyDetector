/*
 * SimpleAnomalyDetector.cpp
 *
 * Author: 208464198 Itay Carmiel
 */
#include "SimpleAnomalyDetector.h"
#include "anomaly_detection_util.h"
#include "AnomalyDetector.h"
#include "minCircle.h"
#include <vector>
#include <algorithm>
#include <string.h>
#include <math.h>
map<string, vector<float>> normalM;
SimpleAnomalyDetector::SimpleAnomalyDetector() {

}

SimpleAnomalyDetector::~SimpleAnomalyDetector() {
    cf.clear();
    normalM.clear();
}
void SimpleAnomalyDetector::setMinCore(float x) {
    minCore = x;
}
void SimpleAnomalyDetector::learnNormal(const TimeSeries& ts){
    normalM = ts.getMap();
    string firstP, secondP;
    float cor, dis;
    for (auto mapF = begin(normalM); mapF != end(normalM); ++mapF) {
        if(next(mapF) != end(normalM)) {
            int flag = 0;
            correlatedFeatures co;
            float maxCor = float (minCore);
            for (auto mapL = mapF; mapL != end(normalM); ++mapL) {
                if(mapL != mapF) {
                    cor = abs(pearson(mapF->second.data(), mapL->second.data(), mapL->second.size()));
                    if (cor > maxCor) {
                        flag = 1;
                        maxCor = cor;
                        co.corrlation = cor;
                        co.feature1 = mapF->first;
                        co.feature2 = mapL->first;
                        Point *ps[mapF->second.size()];
                        for (int i = 0; i < mapF->second.size(); i++) {
                            ps[i] = new Point(mapF->second[i], mapL->second[i]);
                        }
                        co.lin_reg = linear_reg(ps, mapF->second.size());
                        co.threshold = 0;
                        for (int i = 0; i < mapF->second.size(); i++) {
                            dis = ((dev(*ps[i], co.lin_reg)) * float(1.1));
                            if (dis > co.threshold)
                                co.threshold = dis;
                        }
                    }
                    else if (cor > 0.5) {
                        flag = 1;
                        co.corrlation = cor;
                        co.feature1 = mapF->first;
                        co.feature2 = mapL->first;
                        Point *ps[mapF->second.size()];
                        for (int i = 0; i < mapF->second.size(); i++) {
                            ps[i] = new Point(mapF->second[i], mapL->second[i]);
                        }
                        Circle c = findMinCircle(ps, mapF->second.size());
                        co.center = c.center;
                        co.threshold = c.radius*(1.1);
                    }
                }
            }
            if (flag) {
                cf.push_back(co);
            }
        }
    }
}
vector<AnomalyReport> SimpleAnomalyDetector::detect(const TimeSeries& ts){
    map<string, vector<float>> detectM = ts.getMap();
    vector<AnomalyReport> var;
    string leftS, rightS;
    float dis;
    vector<float> leftValues;
    vector<float> rightValues;
    for (auto it = cf.begin() ; it != cf.end(); ++it) {
        leftS = it->feature1;
        rightS = it->feature2;
        Point *p [ts.numOfValue()];
        leftValues = ts.getValues(leftS);
        rightValues = ts.getValues(rightS);
        for (int i = 0; i < ts.numOfValue(); i++) {
            p[i] = new Point(leftValues[i],rightValues[i]);
        }
        if (it->corrlation > minCore) {
            for (int i = 0; i < ts.numOfValue(); i++) {
                dis = dev(*p[i], it->lin_reg);
                if (dis > it->threshold) {
                    AnomalyReport anr(leftS + "-" + rightS, i + 1);
                    var.push_back(anr);
                }
            }
        }
        else {
            for (int i = 0; i < ts.numOfValue(); i++) {
                dis = dist(*p[i], it->center);
                if (dis > it->threshold) {
                    AnomalyReport anr(leftS + "-" + rightS, i + 1);
                    var.push_back(anr);
                }
            }
        }
    }
    return var;
}

