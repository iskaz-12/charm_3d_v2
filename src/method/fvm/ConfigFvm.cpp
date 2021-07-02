/*
 *     ________  _____    ____  __  ___    _____ ____
 *    / ____/ / / /   |  / __ \/  |/  /   |__  // __ \  __    __
 *   / /   / /_/ / /| | / /_/ / /|_/ /     /_ </ / / /_/ /___/ /_
 *  / /___/ __  / ___ |/ _, _/ /  / /    ___/ / /_/ /_  __/_  __/
 *  \____/_/ /_/_/  |_/_/ |_/_/  /_/____/____/_____/ /_/   /_/
 *
 */


#include "ConfigFvm.h"
#include "MethodFvm.h"

namespace charm {
    void ConfigFvm::read() {
        Config::read();
        YAML::Node model = confYaml["control"]["MODEL"];
        useReconstruct = model["use_reconstruct"].as<bool>();
    }

    Method* ConfigFvm::createMethod() {
        Config::createMethod();
        return new MethodFvm(this);
    }

}