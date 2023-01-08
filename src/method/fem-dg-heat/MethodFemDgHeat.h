/**
*	Начато 04.12.2022
*	Цель - реализовать разрывный метод Галёркина для решения задач теплопроводности
*/

#ifndef CHARM_3D_V2_METHODFEMDGHEAT_H
#define CHARM_3D_V2_METHODFEMDGHEAT_H

#include <Flux.h>
#include "Method.h"

//	UPDATE от 07.01.2022 - пытаюсь переделать код как в MethodFvm.h

//	Пока за основу взяла двумерный код из nummeth2019
namespace charm {
	
	/*
	//	Структура Param взята из файла method.h из nummeth2019
	
	struct Param
	{
		
    		double r;		//!< плотность
		
    		double p;		//!< давление
    		double u;		//!< первая  компонента  вектора  скорости
    		double v;		//!< вторая  компонента  вектора  скорости
    		double e;		//!< внутренняя   энергия
		
    		double T;		//!< температура
    		//double M;		//!< число  Маха

    		double qx;      //!< первая компонента градиента температуры
    		double qy;      //!< вторая компонента градиента температуры
		double qz;      //!< третья компонента градиента температуры

    		//inline double U2() { return u*u+v*v; };
	};
	*/

	class MethodFemDgHeat : public Method {
	public:
		explicit MethodFemDgHeat(Config *conf, Mesh *mesh): Method(conf, mesh) {}

		void init() override;
		void run() override;
		void done() override;

		//	UPDATE от 27.12.2022

		/*
		void save(int step);
		void saveVtk(int step);
		*/		

		
		void save();
		void saveVtk();
		

		Real calcDt();

		//	Пока не уверена, что нужны переменные data, integrals и flux...
		Array<Data> data;
		//Array<Cons> integrals;

		//	UPDATE от 25.12.2022 - пытаюсь разрешить коллизию между определениями функции потока flux!!!

		//Flux* flux;

	
	protected:
		
		//	Вот эта часть целиком взята из nummeth2019

		void calcMassMatrix();

		//	UPDATE от 26.12.2022 - комментируем не используемые в cpp-файле функции

		//void calcGaussParam();
		double getT(int iCell, Point pt);
		double getQx(int iCell, Point pt);
		double getQy(int iCell, Point pt);
		double getQz(int iCell, Point pt);

		double getF(int i, int iCell, Point pt);

		double getDfDx(int i, int iCell, Point pt);
		double getDfDy(int i, int iCell, Point pt);
		double getDfDz(int i, int iCell, Point pt);

		void calcGradients();
    		void calcDiffusionVol();
    		void calcDiffusionSurf();
    		void calcNewValues();

		//void bnd(Face *f, Point pt, Prim p1, Prim &p2);
		void convertToParam(int i, Point pt, Prim& p);
		void flux(Prim pl, Prim pr, Vector n, double &fT, double &fqx, double &fqy, double &fqz);
		
		//void saveVTK(int step);

		//	Возможно, double можно заменить на Real

		double **T;
		double **qx;
		double **qy;
		double **qz;

		double **intT;
		double **intQx;
		double **intQy;
		double **intQz;

		double ***A;
		double ***invA;

		//	Может, эти переменные уже задавались в основном коде в другой форме...
		//double **cellGW;
		//Point **cellGP;
		//double **edgeGW;
		//Point **edgeGP;
		//double *cellJ;
		//double *edgeJ;

		//	TAU и TMAX точно задавались в task.yaml
		//double TMAX;
		//double TAU;
		int BASE_FUNC_COUNT;
		int GP_CELL_COUNT;
		int GP_EDGE_COUNT;
		double C11;
	
	};
	
}

#endif //CHARM_3D_V2_METHODFVM_H