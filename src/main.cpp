/*
 *     ________  _____    ____  __  ___    _____ ____
 *    / ____/ / / /   |  / __ \/  |/  /   |__  // __ \  __    __
 *   / /   / /_/ / /| | / /_/ / /|_/ /     /_ </ / / /_/ /___/ /_
 *  / /___/ __  / ___ |/ _, _/ /  / /    ___/ / /_/ /_  __/_  __/
 *  \____/_/ /_/_/  |_/_/ |_/_/  /_/____/____/_____/ /_/   /_/
 *
 */


#include <iostream>
#include <ConfigException.h>
#include "Config.h"
#include "Method.h"
#include "Parallel.h"
#include <fenv.h>

using namespace charm;


int main(Int argc, char** argv) {
    feenableexcept(FE_ALL_EXCEPT & ~FE_INEXACT);


    Parallel::init(&argc, &argv);
    try {
        auto conf = Config::create("task.yaml");
        auto method = conf->createMethod();
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
    catch (Exception &e) {
        std::cerr << e.getMessage() << std::endl;
    }


    Parallel::done();
    return 0;
}
