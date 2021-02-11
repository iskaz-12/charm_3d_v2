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
        useReconstruct = model["use_reconstruct"].as<bool>();
    }

    Method* ConfigFvm::createMethod() {
        Config::createMethod();
        return new MethodFvm(this);
    }

}