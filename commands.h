

#ifndef COMMANDS_H_
#define COMMANDS_H_

#include<iostream>
#include "map"
#include <fstream>
#include <vector>
#include "HybridAnomalyDetector.h"
#include <sys/socket.h>
#include <unistd.h>
#include <netinet/in.h>
#include <netdb.h>

using namespace std;
struct Helper {
    TimeSeries ts1;
    TimeSeries ts2;
    float threshold = 0.9;
    HybridAnomalyDetector hd;
    vector<AnomalyReport> ar;
};
//reciver
class DefaultIO{
public:
    virtual string read()=0;
    virtual void write(string text)=0;
    virtual void write(float f)=0;
    virtual void read(float* f)=0;
    virtual ~DefaultIO(){}

};
class SocketIO: public DefaultIO{
    int serverFD;
public:
    SocketIO(int serverFD) {
        this->serverFD = serverFD;
    }
    virtual string read() {
        string serverInput="";
        char c=0;
        ::read(serverFD,&c,sizeof(char));
        while(c!='\n'){
            serverInput+=c;
            ::read(serverFD,&c,sizeof(char));
        }
        return serverInput;
    }
    virtual void write(string text){
        ::write(serverFD, text.c_str(),text.length());
    }
    virtual void write(float f){
        char buf[64];
        snprintf(buf, sizeof(buf), "%g", f);
        ::write(serverFD, buf,strlen(buf));
    }
    virtual void read(float* f){}
};

class Command{
    DefaultIO* dio;
public:
    Command(){}
    Command(DefaultIO* dio):dio(dio){}
    virtual string description()=0;
    virtual void execute(DefaultIO* dio, Helper *helper) = 0;
    virtual ~Command(){}
};
class ShowMenu: public Command {
public: ShowMenu(DefaultIO* dio) {
    }
public: string description() override {
        return("Welcome to the Anomaly Detection Server.\nPlease choose an option:\n");
    }
    void execute(DefaultIO* dio, Helper *helper) override {
        int flag; // just to fill
    }
};
class Upload: public Command {
    DefaultIO *d;
public: Upload(DefaultIO* dio) {
    this->d = dio;
}
public: string description() override {
   return("1.upload a time series csv file\n");
}
    void execute(DefaultIO* dio, Helper *helper) override {
        dio->write("Please upload your local train CSV file.\n");
        const char* train = "AnomalyTrain.csv";
        ofstream myFile(train);
        string s = dio->read();
        while(s.compare("done")!= 0) {
            myFile << s;
            myFile << "\n";
            s = dio->read();
        }
        myFile.close();
        TimeSeries t1(train);
        helper->ts1 = t1;
        dio->write("Upload complete.\n");
        dio->write("Please upload your local test CSV file.\n");
        const char* test = "AnomalyTest.csv";
        ofstream myFile1(test);
        string s1 = dio->read();
        while(s1.compare("done")!= 0) {
            myFile1 << s1;
            myFile1 << "\n";
            s1 = dio->read();
        }
        myFile1.close();
        TimeSeries t2(test);
        helper->ts2 = t2;
        dio->write("Upload complete.\n");
    }
};
class AlgoSet: public Command {
    DefaultIO *d;
public: AlgoSet(DefaultIO* dio) {
        this->d = dio;
    }
public: string description() override {
        return("2.algorithm settings\n");
    }
    void execute(DefaultIO* dio, Helper *helper) override {
        float thr = 2;
        while ((thr < 0) || (thr > 1)) {
            dio->write("The current correlation threshold is");
            dio->write(helper->threshold);
            dio->write("\n");
            dio->write("Type a new threshold\n");
            string c = dio->read();
            thr = stof(c);
        }
        helper->threshold = thr;
    }
};
class DetectAno: public Command {
    DefaultIO *d;
public: DetectAno(DefaultIO* dio) {
        this->d = dio;
    }
public: string description() override {
        return("3.detect anomalies\n");
    }
    void execute(DefaultIO* dio, Helper *helper) override  {
        helper->hd.setMinCore(helper->threshold);
        helper->hd.learnNormal(helper->ts1);
        helper->ar = helper->hd.detect(helper->ts2);
        dio->write("anomaly detection complete.\n");
    }
};
class Display: public Command {
    DefaultIO *d;
public: Display(DefaultIO* dio) {
        this->d = dio;
    }
public: string description() override {
       return("4.display results\n");
    }
    void execute(DefaultIO* dio, Helper *helper) override  {
        for (int i=0; i<helper->ar.size(); i++) {
            dio->write(helper->ar[i].timeStep);
            dio->write("    ");
            dio->write(helper->ar[i].description);
            dio->write("\n");
        }
        dio->write("Done.\n");
    }
};
class UploadAnl: public Command {
    DefaultIO *d;
public: UploadAnl(DefaultIO* dio) {
        this->d = dio;
    }
public: string description() override {
        return("5.upload anomalies and analyze results\n");
    }
    void execute(DefaultIO* dio, Helper *helper) override  {
        dio->write("Please upload your local anomalies file.\n");
        vector<pair<int, int>> test;
        vector<pair<int, int>> anomaly;
        int first, second, temp, sum = 0;
        pair<int, int> pai;
        string s = dio->read();
        while(s.compare("done")!= 0)
        {
            int pos = s.find(",");
            string fir = s.substr(0,pos);
            string sec = s.substr(pos + 1,s.size()-1);
            first = stoi(fir);
            second = stoi(sec);
            pai = make_pair(first,second);
            anomaly.push_back(pai);
            s = dio->read();
        }
        dio->write("Upload complete.\n");
        int p = anomaly.size();
        int n = helper->ts2.numOfValue();
        for (int j=0; j<anomaly.size(); j++) {
            temp = (anomaly[j].second - anomaly[j].first + 1);
            sum = sum + temp;
        }
        int N = n - sum;
        int TP = 0, FP = 0, fi, la;
        fi = helper->ar[0].timeStep;
        for(int i = 0; i<helper->ar.size()-1; i++) {
            if((helper->ar[i+1].timeStep - helper->ar[i].timeStep) != 1) {
                la = helper->ar[i].timeStep;
                pai = make_pair(fi, la);
                test.push_back(pai);
                fi = helper->ar[i+1].timeStep;
            }
        }
        la = helper->ar[helper->ar.size()-1].timeStep;
        pai = make_pair(fi, la);
        test.push_back(pai);
        for (int i=0; i<test.size(); i++) {
            int flag = 0;
            int fReport = test[i].first;
            int lReport = test[i].second;
            for (int j = 0; j < anomaly.size(); j++) {
                if (((fReport >= anomaly[j].first) && (fReport <= anomaly[j].second))
                    || ((lReport >= anomaly[j].first) && (lReport <= anomaly[j].second))
                    ||((anomaly[j].first >= fReport) && (anomaly[j].first <= lReport))
                    ||((anomaly[j].second >= fReport) && (anomaly[j].second <= lReport))){
                    TP++;
                    flag = 1;
                }
            }
            if (flag == 0) {
                FP++;
            }
        }
        float truePositive = ((float)TP/(float)p);
        float falseAlarm = ((float)FP/(float)N);
        truePositive = (floor(truePositive * 1000)) / 1000;
        falseAlarm = (floor(falseAlarm * 1000)) / 1000;
        dio->write("True Positive Rate: ");
        dio->write(truePositive);
        dio->write("\n");
        dio->write("False Positive Rate: ");
        dio->write(falseAlarm);
        dio->write("\n");
    }
};
class ExitSer: public Command {
    DefaultIO *d;
public: ExitSer(DefaultIO* dio) {
        this->d = dio;
    }
public: string description() override {
        return("6.exit\n");
    }
    void execute(DefaultIO* dio, Helper *helper) override {
        int flag; // just to fill
    }
};

#endif /* COMMANDS_H_ */
