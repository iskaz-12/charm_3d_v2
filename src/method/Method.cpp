/**
 * Created by zhrv on 18.11.2020.
 * @author R.V.Zhalnin, zhalnin@gmail.com
 */

#include <MethodFvm.h>
#include <iostream>
#include "Method.h"

//  UPDATE ON 12.07.2023 - include MethodFemDgHeat.h
#include <MethodFemDgHeat.h>

namespace charm {
    Method::Method(Config *_conf, Mesh *_mesh): conf(_conf), mesh(_mesh) {}

    Method* Method::create(Config *conf, Mesh *mesh) {
        if (conf->methodName == "CHARM_FVM") {
            return new MethodFvm(conf, mesh);
        }

        //  UPDATE ON 12.07.2023 - добавляем MethodFemDgHeat
        else if (conf->methodName == "HEAT_FEM_DG") {
            return new MethodFemDgHeat(conf, mesh);
        }

        else {
            std::cerr << "Wrong method!!!\n";
            exit(1);
        }
    }
}
