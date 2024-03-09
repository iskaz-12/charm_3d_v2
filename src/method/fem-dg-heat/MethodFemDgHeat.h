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

		//	UPDATE ON 12.07.2023 - дополнительно определяю BASE_FUNC_COUNT в этом классе
		Int BASE_FUNC_COUNT = 4;

		//	UPDATE от 27.12.2022

		/*
		void save(int step);
		void saveVtk(int step);
		*/		

		
		void save();
        //  UPDATE ON 08.07.2023 - функция saveVtk все-таки нужна...
		void saveVtk();
		

		Real calcDt();

		//	Пока не уверена, что нужны переменные data, integrals и flux...
		Array<DataDgHeat> data;
		Array<HeatDgFields> integrals;


		//	UPDATE ON 12.07.2023 - убираю область видимости protected, чтобы попробовать исправить ошибки...
	
	//protected:
		
		//	Вот эта часть целиком взята из nummeth2019

		void calcMassMatrix();

		//	UPDATE от 26.12.2022 - комментируем не используемые в cpp-файле функции

		//void calcGaussParam();
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

        // double getF(int i, int iCell, Point pt);

        // double getDfDx(int i, int iCell, Point pt);
        // double getDfDy(int i, int iCell, Point pt);
        // double getDfDz(int i, int iCell, Point pt);

		void calcGradients();
        void calcDiffusionVol();
        void calcDiffusionSurf();
        void calcNewValues();

		//void bnd(Face *f, Point pt, Prim p1, Prim &p2);
		//	UPDATE ON 13.07.2023 - в определении функции convertToParam меняю Prim на PrimHeat
		//	void convertToParam(int i, Point pt, Prim& p);
		void convertToParam(int i, Point pt, PrimHeat& p, HeatDgFields& flds);

		//	UPDATE ON 13.07.2023 - в определении функции flux меняю Prim на PrimHeat, Vector - на Point
		//	void flux(Prim pl, Prim pr, Vector n, double &fT, double &fqx, double &fqy, double &fqz);
		void flux(PrimHeat pl, PrimHeat pr, Point n, double &fT, double &fqx, double &fqy, double &fqz);
		
		//void saveVTK(int step);

		//	Возможно, double можно заменить на Real


		//	UPDATE ON 10.07.2023 - нужны ли матрицы A и invA???
		//	---08.03.2024---
		//	double ***A;
		//	double ***invA;

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
		double C11;

		//	UPDATE ON 12.07.2023 - добавляю массивы для сохранения температуры и тепловых потоков по аналогии с nummeth2019
		//	---08.03.2024---
		//	double **T;
    	//	double **qx;
    	//	double **qy;
		//	double **qz;

    	//	double **intT;
    	//	double **intQx;
    	//	double **intQy;
		//	double **intQz;
	
	};
	
}

#endif //CHARM_3D_V2_METHODFVM_H