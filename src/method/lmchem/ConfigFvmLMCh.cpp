/*
 *     ________  _____    ____  __  ___    _____ ____
 *    / ____/ / / /   |  / __ \/  |/  /   |__  // __ \  __    __
 *   / /   / /_/ / /| | / /_/ / /|_/ /     /_ </ / / /_/ /___/ /_
 *  / /___/ __  / ___ |/ _, _/ /  / /    ___/ / /_/ /_  __/_  __/
 *  \____/_/ /_/_/  |_/_/ |_/_/  /_/____/____/_____/ /_/   /_/
 *
 */


//
// Created by zhrv on 17.12.2020.
//

#include "ConfigFvmLMCh.h"
#include "MethodFvmLMCh.h"

namespace charm {

    ConfigFvmLMCh::ConfigFvmLMCh(const String &fileName) : Config(fileName) {
        useVisc = 0;
        useDiff = 0;
        tRef    = 298.5;
    }


    void ConfigFvmLMCh::read() {
        Config::read();
        YAML::Node model = confYaml["control"]["MODEL"];
        useVisc = model["use_visc"].as<Int>();
        useDiff = model["use_diffusion"].as<Int>();
        tRef    = model["t_ref"].as<Real>();

        pressEps        = model["press_eps"].as<Real>();
        pressMaxIter    = model["press_max_iter"].as<Real>();
    }


    Method* ConfigFvmLMCh::createMethod() {
        Config::createMethod();
        return new MethodFvmLMCh(this);
    }
}