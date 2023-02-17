/**
*	Начато 03.12.2022
*	Цель - реализовать разрывный метод Галёркина для решения задач теплопроводности
*/

#ifndef CHARM_3D_V2_CONFIGFEMDGHEAT_H
#define CHARM_3D_V2_CONFIGFEMDGHEAT_H

#include "Config.h"

namespace charm {
	class ConfigFemDgHeat : public Config {
	public:
		ConfigFemDgHeat(const String &filename) : Config(filename) {}

		virtual void read();

		virtual Method* createMethod();

		Int useVisc;
		Int useDiff;
		Real tRef;

        static Int baseFuncCount;
        static int gpCellCount;
        static int gpEdgeCount;

    };
}
#endif	//CHARM_3D_V2_CONFIGFEMDGHEAT_H