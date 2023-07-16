#include <iostream>
#include <ConfigException.h>
#include "Config.h"
#include "Method.h"

using namespace charm;
int main() {

    try {
        Config *conf = Config::create("task.yaml");
//        conf->read();

        Method *method = conf->createMethod();

        method->init();
        method->run();
        method->done();

    }
    catch (ConfigException &e) {
        std::cerr << "CONFIG: " << e.getMessage() << std::endl;
    }
    catch (MethodException &e) {
        std::cerr << e.getMessage() << std::endl;
    }



    return 0;
}
