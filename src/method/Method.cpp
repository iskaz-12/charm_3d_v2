/**
 * Created by zhrv on 18.11.2020.
 * @author R.V.Zhalnin, zhalnin@gmail.com
 */

#include <MethodFvm.h>
#include <iostream>
#include "Method.h"

namespace charm {
    Method::Method(Config *_conf, Mesh *_mesh): conf(_conf), mesh(_mesh) {}

    Method* Method::create(Config *conf, Mesh *mesh) {
        if (conf->methodName == "CHARM_FVM") {
            return new MethodFvm(conf, mesh);
        }
        else {
            std::cerr << "Wrong method!!!\n";
            exit(1);
        }
    }
}
