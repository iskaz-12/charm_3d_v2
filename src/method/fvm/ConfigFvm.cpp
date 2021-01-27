/**
 * Created by zhrv on 18.11.2020.
 * @author R.V.Zhalnin, zhalnin@gmail.com
 */

#include "ConfigFvm.h"
#include "MethodFvm.h"

namespace charm {
    void ConfigFvm::read() {
        Config::read();
        YAML::Node model = confYaml["control"]["MODEL"];
        useVisc = model["use_visc"].as<Int>();
        useDiff = model["use_diffusion"].as<Int>();
        tRef    = model["t_ref"].as<Real>();
    }

    Ptr<Method> ConfigFvm::createMethod() {
        Config::createMethod();
        return Ptr<Method>(new MethodFvm(Ptr<Config>(this)));
    }

}