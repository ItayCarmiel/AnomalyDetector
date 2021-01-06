/*
 * SimpleAnomalyDetector.h
 *
 * Author: 208464198 Itay Carmiel
 */

#ifndef SIMPLEANOMALYDETECTOR_H_
#define SIMPLEANOMALYDETECTOR_H_

#include "anomaly_detection_util.h"
#include "AnomalyDetector.h"
#include <vector>
#include <algorithm>
#include <string.h>
#include <math.h>

struct correlatedFeatures{
    string feature1,feature2;  // names of the correlated features
    float corrlation;
    Line lin_reg;
    Point center;
    float threshold;
};


class SimpleAnomalyDetector:public TimeSeriesAnomalyDetector{
    vector<correlatedFeatures> cf;
    float minCore = 0.9;
public:
    SimpleAnomalyDetector();
    virtual ~SimpleAnomalyDetector();
    virtual void setMinCore(float x);
    virtual void learnNormal(const TimeSeries& ts);
    virtual vector<AnomalyReport> detect(const TimeSeries& ts);

    vector<correlatedFeatures> getNormalModel(){
        return cf;
    }

};



#endif /* SIMPLEANOMALYDETECTOR_H_ */
