/**
*	Начато 04.07.2023
*	Цель - реализовать разрывный метод Галёркина для решения задач теплопроводности
*/

#include "ConfigFemDgHeat.h"
#include "MethodFemDgHeat.h"

namespace charm {
	void ConfigFemDgHeat::read() {
		Config::read();
		YAML::Node model = confYaml["control"]["MODEL"];
        useVisc = model["use_visc"].as<Int>();
        useDiff = model["use_diffusion"].as<Int>();
        tRef    = model["t_ref"].as<Real>();
	}

	Method *ConfigFemDgHeat::createMethod() {
        	Config::createMethod();
        	return new MethodFemDgHeat(this, mesh);
	}
}