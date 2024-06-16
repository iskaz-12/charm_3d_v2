/**
*	Начато 04.12.2022
*	Цель - реализовать разрывный метод Галёркина для решения задач теплопроводности
*/

#ifndef CHARM_3D_V2_METHODFEMDGHEAT_H
#define CHARM_3D_V2_METHODFEMDGHEAT_H

#include <Flux.h>
#include "Method.h"
#include "DataDgHeat.h"

//	UPDATE от 07.01.2022 - пытаюсь переделать код как в MethodFvm.h

//	Пока за основу взяла двумерный код из nummeth2019
namespace charm {
	
	class MethodFemDgHeat : public Method {
	public:
		explicit MethodFemDgHeat(Config *conf, Mesh *mesh): Method(conf, mesh) {}

		void init() override;
		void run() override;
		void done() override;

		//	UPDATE ON 12.07.2023 - дополнительно определяю BASE_FUNC_COUNT в этом классе
		Int BASE_FUNC_COUNT = 4;
		
		void save();
		void saveVtk();
		

		Real calcDt();

		Array<DataDgHeat> data;
		Array<HeatDgFields> integrals;


		void calcMassMatrix();

		double getT(int iCell, Point pt, HeatDgFields &flds);
		double getQx(int iCell, Point pt, HeatDgFields &flds);
		double getQy(int iCell, Point pt, HeatDgFields &flds);
		double getQz(int iCell, Point pt, HeatDgFields &flds);

		//	UPDATE ON 08.07.2023 - эти 4 функции есть в DataDgHeat, убираю их из MethodFemDgHeat
		//	UPDATE ON 12.07.2023 - переношу 4 функции ниже в том виде, в котором они определены в DataDgHeat

		Real getF(int i, Int iCell, Point pt);
        Real getDfDx(int i, Int iCell, Point pt);
        Real getDfDy(int i, Int iCell, Point pt);
        Real getDfDz(int i, Int iCell, Point pt);

		void calcGradients();
        void calcDiffusionVol();
        void calcDiffusionSurf();
        void calcNewValues();

		void convertToParam(int i, Point pt, PrimHeat& p, HeatDgFields& flds);

		void flux(PrimHeat pl, PrimHeat pr, Point n, double &fT, double &fqx, double &fqy, double &fqz);

		double C11;
	};
	
}

#endif //CHARM_3D_V2_METHODFVM_H