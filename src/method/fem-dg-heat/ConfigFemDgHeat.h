/**
*	Начато 04.07.2023
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

		//	UPDATE ON 12.07.2023 - меняю название переменной
        //	static Int baseFuncCount;

		//	UPDATE ON 13.07.2023 - пока попробую убрать из классов ConfigFemDgHeat.h и ConfigFemDgHeat.cpp переменную BASE_FUNC_COUNT
		//	static Int BASE_FUNC_COUNT;
        static int gpCellCount;
        static int gpEdgeCount;

    };
}
#endif	//CHARM_3D_V2_CONFIGFEMDGHEAT_H