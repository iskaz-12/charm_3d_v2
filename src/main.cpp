#include <iostream>
#include <ConfigException.h>
#include "Config.h"
#include "Method.h"

using namespace charm;
int main() {

    try {
        Config *conf = Config::create("task.yaml");
//        conf->read();

	//	UPDATE от 26.12.2022

	std::cout<<"!"<<std::endl;

        Method *method = conf->createMethod();

	std::cout<<"!!"<<std::endl;

        method->init();

	std::cout<<"From main.cpp: init done!"<<std::endl;


        method->run();

	std::cout<<"From main.cpp: run done!"<<std::endl;

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
