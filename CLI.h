

#ifndef CLI_H_
#define CLI_H_

#include "commands.h"

using namespace std;

class CLI {
    DefaultIO* dio;
    vector<Command*> co;
    Helper helper;
public:
    CLI(DefaultIO* dio);
    void start();
    virtual ~CLI();
};

#endif /* CLI_H_ */
