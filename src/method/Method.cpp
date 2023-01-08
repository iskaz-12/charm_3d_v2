/**
 * Created by zhrv on 18.11.2020.
 * @author R.V.Zhalnin, zhalnin@gmail.com
 */

#include <MethodFvm.h>
#include <iostream>
#include "Method.h"

//	Исправление от 03.12.22
#include <MethodFemDgHeat.h>

namespace charm {
    Method::Method(Config *_conf, Mesh *_mesh): conf(_conf), mesh(_mesh) {}

    Method* Method::create(Config *conf, Mesh *mesh) {
        if (conf->methodName == "CHARM_FVM") {
            return new MethodFvm(conf, mesh);
        }

	//	Исправление от 03.12.22
	//	Цель - написать свой разрывный метод Галёркина для решения задач теплопроводности
	
	else if (conf->methodName == "CHARM_FEM_DG_HEAT") {
		return new MethodFemDgHeat(conf, mesh);
	}

        else {
            std::cerr << "Wrong method!!!\n";
            exit(1);
        }
    }
}
