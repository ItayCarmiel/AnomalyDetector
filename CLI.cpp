#include "CLI.h"
#include "vector"
CLI::CLI(DefaultIO* dio) {
    this->dio = dio;
    this->co.clear();
}

void CLI::start(){
    int num = 0;
    ShowMenu *sm = new ShowMenu(dio);
    Upload *up = new Upload(dio);
    AlgoSet *as = new AlgoSet(dio);
    DetectAno *da = new DetectAno(dio);
    Display *di = new Display(dio);
    UploadAnl *ua = new UploadAnl(dio);
    ExitSer *es = new ExitSer(dio);
    this->co.push_back(sm);
    this->co.push_back(up);
    this->co.push_back(as);
    this->co.push_back(da);
    this->co.push_back(di);
    this->co.push_back(ua);
    this->co.push_back(es);
    while (num != 6) {
        for (int i = 0; i < co.size(); i++) {
            dio->write(co[i]->description());
        }
        string s = dio->read();
        num = stoi(s);
        if ((num >= 0) && (num < 7)) {
            co[num]->execute(dio, &helper);
        }
    }
}

CLI::~CLI() {
}

