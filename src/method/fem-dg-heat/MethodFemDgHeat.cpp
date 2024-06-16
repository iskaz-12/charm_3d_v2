/**
*	Начато 04.12.2022
*	Цель - реализовать разрывный метод Галёркина для решения задач теплопроводности
*/

//  UPDATE ON 13.07.2023 - добавка для применения константы Pi
#define _USE_MATH_DEFINES
#include <math.h>

#include <FluxHllc.h>
#include <iomanip>
#include <fstream>
#include <iostream>
#include <FluxLf.h>
#include "MethodFemDgHeat.h"
#include "DataDgHeat.h"

#include <cstdio>
#include <cstring>
#include <cmath>
#include <stdlib.h>

//  UPDATE ON 12.07.2023 - include ConfigFemDgHeat.h
#include "ConfigFemDgHeat.h"

//	UPDATE от 25.12.2022 - include global.cpp
//#include <global.cpp>

//	Пока за основу взяла двумерный код из nummeth2019
//	И немного скомбинировала с MethodFvm.cpp
namespace charm {

    Real matr4Det(ArrayReal a) {
        Real det_a = a[3] * a[6] * a[9] * a[12] - a[2] * a[7] * a[9] * a[12] -
                     a[3] * a[5] * a[10] * a[12] + a[1] * a[7] * a[10] * a[12] +
                     a[2] * a[5] * a[11] * a[12] - a[1] * a[6] * a[11] * a[12] -
                     a[3] * a[6] * a[8] * a[13] + a[2] * a[7] * a[8] * a[13] +
                     a[3] * a[4] * a[10] * a[13] - a[0] * a[7] * a[10] * a[13] -
                     a[2] * a[4] * a[11] * a[13] + a[0] * a[6] * a[11] * a[13] +
                     a[3] * a[5] * a[8] * a[14] - a[1] * a[7] * a[8] * a[14] -
                     a[3] * a[4] * a[9] * a[14] + a[0] * a[7] * a[9] * a[14] +
                     a[1] * a[4] * a[11] * a[14] - a[0] * a[5] * a[11] * a[14] -
                     a[2] * a[5] * a[8] * a[15] + a[1] * a[6] * a[8] * a[15] +
                     a[2] * a[4] * a[9] * a[15] - a[0] * a[6] * a[9] * a[15] -
                     a[1] * a[4] * a[10] * a[15] + a[0] * a[5] * a[10] * a[15];

        return det_a;
    }


    void matr4Inv(ArrayReal a, Cell& cell) {

        ArrayReal a_;
        int i, j;
        Real det_a = matr4Det(a);

        std::cout << "From matr4Inv: 0! " << det_a << std::endl;
        std::cout << "From matr4Inv: 0! " << a[0] << std::endl;
        std::cout << "From matr4Inv: 0! " << a[15] << std::endl;
        std::cout << "From matr4Inv: 0! " << cell.A[0] << std::endl;

        //CHARM_ASSERT(det_a != 0.);

        a_.push_back(-a[7] * a[10] * a[13] + a[6] * a[11] * a[13] +
                   a[7] * a[9] * a[14] - a[5] * a[11] * a[14] -
                   a[6] * a[9] * a[15] + a[5] * a[10] * a[15]);

        std::cout << "From matr4Inv: 1!" << std::endl;


        a_.push_back(a[3] * a[10] * a[13] - a[2] * a[11] * a[13] -
                   a[3] * a[9] * a[14] + a[1] * a[11] * a[14] +
                   a[2] * a[9] * a[15] - a[1] * a[10] * a[15]);
        a_.push_back(-a[3] * a[6] * a[13] + a[2] * a[7] * a[13] +
                   a[3] * a[5] * a[14] - a[1] * a[7] * a[14] -
                   a[2] * a[5] * a[15] + a[1] * a[6] * a[15]);
        a_.push_back(a[3] * a[6] * a[9] - a[2] * a[7] * a[9] -
                   a[3] * a[5] * a[10] + a[1] * a[7] * a[10] +
                   a[2] * a[5] * a[11] - a[1] * a[6] * a[11]);
        a_.push_back(a[7] * a[10] * a[12] - a[6] * a[11] * a[12] -
                   a[7] * a[8] * a[14] + a[4] * a[11] * a[14] +
                   a[6] * a[8] * a[15] - a[4] * a[10] * a[15]);
        a_.push_back(-a[3] * a[10] * a[12] + a[2] * a[11] * a[12] +
                   a[3] * a[8] * a[14] - a[0] * a[11] * a[14] -
                   a[2] * a[8] * a[15] + a[0] * a[10] * a[15]);
        a_.push_back(a[3] * a[6] * a[12] - a[2] * a[7] * a[12] -
                   a[3] * a[4] * a[14] + a[0] * a[7] * a[14] +
                   a[2] * a[4] * a[15] - a[0] * a[6] * a[15]);
        a_.push_back(-a[3] * a[6] * a[8] + a[2] * a[7] * a[8] +
                   a[3] * a[4] * a[10] - a[0] * a[7] * a[10] -
                   a[2] * a[4] * a[11] + a[0] * a[6] * a[11]);
        a_.push_back(-a[7] * a[9] * a[12] + a[5] * a[11] * a[12] +
                   a[7] * a[8] * a[13] - a[4] * a[11] * a[13] -
                   a[5] * a[8] * a[15] + a[4] * a[9] * a[15]);
        a_.push_back(a[3] * a[9] * a[12] - a[1] * a[11] * a[12] -
                   a[3] * a[8] * a[13] + a[0] * a[11] * a[13] +
                   a[1] * a[8] * a[15] - a[0] * a[9] * a[15]);
        a_.push_back(-a[3] * a[5] * a[12] + a[1] * a[7] * a[12] +
                   a[3] * a[4] * a[13] - a[0] * a[7] * a[13] -
                   a[1] * a[4] * a[15] + a[0] * a[5] * a[15]);
        a_.push_back(a[3] * a[5] * a[8] - a[1] * a[7] * a[8] -
                   a[3] * a[4] * a[9] + a[0] * a[7] * a[9] +
                   a[1] * a[4] * a[11] - a[0] * a[5] * a[11]);
        a_.push_back(a[6] * a[9] * a[12] - a[5] * a[10] * a[12] -
                   a[6] * a[8] * a[13] + a[4] * a[10] * a[13] +
                   a[5] * a[8] * a[14] - a[4] * a[9] * a[14]);
        a_.push_back(-a[2] * a[9] * a[12] + a[1] * a[10] * a[12] +
                   a[2] * a[8] * a[13] - a[0] * a[10] * a[13] -
                   a[1] * a[8] * a[14] + a[0] * a[9] * a[14]);
        a_.push_back(a[2] * a[5] * a[12] - a[1] * a[6] * a[12] -
                   a[2] * a[4] * a[13] + a[0] * a[6] * a[13] +
                   a[1] * a[4] * a[14] - a[0] * a[5] * a[14]);
        a_.push_back(-a[2] * a[5] * a[8] + a[1] * a[6] * a[8] +
                   a[2] * a[4] * a[9] - a[0] * a[6] * a[9] -
                   a[1] * a[4] * a[10] + a[0] * a[5] * a[10]);


        std::cout << "From matr4Inv: 2!" << std::endl;

        for (i = 0; i < 4; i++) {
            for (j = 0; j < 4; j++) {

                std::cout << "From matr4Inv: cycle 0!" << std::endl;

                cell.invA[i * 4 + j] = a_[i * 4 + j] / det_a;

                std::cout << "From matr4Inv: a_inv[i * 4 + j] " << cell.invA[i * 4 + j] << std::endl;

            }
        }
    }


    //	Конвертация в примитивные переменные
    void MethodFemDgHeat::convertToParam(int i, Point pt, PrimHeat &p, HeatDgFields& flds) {
        p.t = getT(i, pt, flds);
        p.qx = getQx(i, pt, flds);
        p.qy = getQy(i, pt, flds);
        p.qz = getQz(i, pt, flds);
    }


    //	Методы, реализующие получение значений из полей
    double MethodFemDgHeat::getT(int iCell, Point pt, HeatDgFields &flds) {
        double d = 0.;
        for (int i = 0; i < BASE_FUNC_COUNT; i++) {
            d += flds.t[i] * getF(i, iCell, pt);
        }

        return d;
    }


    //  UPDATE ON 12.07.2023 - добавляю реализацию функций getF, getDfDx, getDfDy, getDfDz
    Real MethodFemDgHeat::getF(Int i, Int iCell, Point pt)
    {
        if (BASE_FUNC_COUNT == 4)
        {
            switch (i) {
                case 0:
                    return 1.0;
                case 1:
                    return (pt.x - mesh->cells[iCell].center.x) / mesh->cells[iCell].dh[0];
                case 2:
                    return (pt.y - mesh->cells[iCell].center.y) / mesh->cells[iCell].dh[1];
                case 3:
                    return (pt.z - mesh->cells[iCell].center.z) / mesh->cells[iCell].dh[2];
            }

        }

        else {
            printf("ERROR: wrong basic functions count!\n");
            exit(1);
        }
    }

    
    double MethodFemDgHeat::getDfDx(Int i, Int iCell, Point pt)
    {
        if (BASE_FUNC_COUNT == 4)
        {
            switch(i) {
                case 0:
                    return 0.0;
                case 1:
                    return 1.0 / mesh->cells[iCell].dh[0];
                case 2:
                    return 0.0;
                case 3:
                    return 0.0;
            }
        }

        else {
            printf("ERROR: wrong basic functions count!\n");
            exit(1);
        }
    }


    double MethodFemDgHeat::getDfDy(Int i, Int iCell, Point pt)
    {
        if (BASE_FUNC_COUNT == 4)
        {
            switch(i) {
                case 0:
                    return 0.0;
                case 1:
                    return 0.0;
                case 2:
                    return 1.0 / mesh->cells[iCell].dh[1];
                case 3:
                    return 0.0;
            }
        }

        else {
            printf("ERROR: wrong basic functions count!\n");
            exit(1);
        }
    }


    double MethodFemDgHeat::getDfDz(Int i, Int iCell, Point pt)
    {
        if (BASE_FUNC_COUNT == 4)
        {
            switch(i) {
                case 0:
                    return 0.0;
                case 1:
                    return 0.0;
                case 2:
                    return 0.0;
                case 3:
                    return 1.0 / mesh->cells[iCell].dh[2];
            }
        }

        else {
            printf("ERROR: wrong basic functions count!\n");
            exit(1);
        }
    }


    //	Функция инициализации
    void MethodFemDgHeat::init() {

        std::cout << "From MethodFemDgHeat.cpp: init started!" << std::endl;

        Index cN = mesh->cells.size();
        data.resize(cN, DataDgHeat(BASE_FUNC_COUNT));

        //  UPDATE ON 12.07.2023 - начальная инициализация массивов температуры и тепловых потоков
        std::cout << "From MethodFemDgHeat.cpp: init continued!" << std::endl;
        for (Index ic = 0; ic < cN; ic++) {
            std::cout << "From MethodFemDgHeat.cpp: init continued cycle 0!" << std::endl;
            Cell &cell = mesh->cells[ic];
            std::cout << "From MethodFemDgHeat.cpp: init continued cycle 1!" << std::endl;
            std::cout << "cell.tag from MethodFemDgHeat.cpp: " << cell.tag << std::endl;
            Region *reg = Config::getRegion(cell.tag);
            std::cout << "From MethodFemDgHeat.cpp: init continued cycle 2!" << std::endl;

            // ---09.12.2023---
            // Пробую сделать всё по аналогии с nummeth2019

            for (Int ind_ = 0; ind_ < BASE_FUNC_COUNT; ind_++) {
                data[ic].flds.t[ind_] = 0.0;
                data[ic].flds.qx[ind_] = 0.0;
                data[ic].flds.qy[ind_] = 0.0;
                data[ic].flds.qz[ind_] = 0.0;
                data[ic].flds.intT[ind_] = 0.0;
                data[ic].flds.intQx[ind_] = 0.0;
                data[ic].flds.intQy[ind_] = 0.0;
                data[ic].flds.intQz[ind_] = 0.0;
            }

            //  ---09.03.2024---
            //  Пока температуру из YAML-файла переносим в первый коэффициент разложения температуры
            data[ic].flds.t[0] = reg->t;

            //  ---21.04.2024---
            //  Пробую решить тестовую задачу из статьи prep2016_66
            //  data[ic].flds.t[0] = cos(M_PI * cell.center.x) * cos(M_PI * cell.center.y) * cos(M_PI * cell.center.z);

            //  ---23.04.2024---
            //  Пробую решить тестовую задачу из статьи (задача №3)
            data[ic].flds.t[0] = 1. + cell.center.x * cell.center.x + cell.center.y * cell.center.y + cell.center.z * cell.center.z;

            for (Int ind_1 = 0; ind_1 < BASE_FUNC_COUNT; ind_1++) {
                for (Int ind_2 = 0; ind_2 < BASE_FUNC_COUNT; ind_2++) {
                    cell.A.push_back(0.0);
                    cell.invA.push_back(0.0);
                }
            }

        }


        std::cout << "From MethodFemDgHeat.cpp: init continued 2!" << std::endl;

        calcMassMatrix();

        //flux = new FluxHllc(); ///< @todo @todo
        save();
    }


    //	Переделала формулы под обозначения gw и gp в charm_3d_v2
    void MethodFemDgHeat::calcMassMatrix() {

        std::cout << "From MethodFemDgHeat.cpp: calcMAssMatrix is started!" << std::endl;

        for (int ind = 0; ind < mesh->cells.size(); ind++) {         

            Cell &cell = mesh->cells[ind];

            ArrayReal mA = cell.A;

            ArrayReal invA_ = cell.invA;
            for (int i = 0; i < BASE_FUNC_COUNT; i++) {
                for (int j = 0; j < BASE_FUNC_COUNT; j++) {

                    for (int iGP = 0; iGP < mesh->cells[ind].gp.size(); iGP++) {
                        cell.A[i * BASE_FUNC_COUNT + j] += mesh->cells[ind].gw[iGP] * getF(i, ind, mesh->cells[ind].gp[iGP]) *
                                    getF(j, ind, mesh->cells[ind].gp[iGP]);
                    }
                }
            }

            matr4Inv(cell.A, cell);

        }

        std::cout << "From MethodFemDgHeat.cpp: calcMAssMatrix is finished!" << std::endl;
    }


    void MethodFemDgHeat::calcGradients() {

        // объёмные интегралы
        for (int i = 0; i < mesh->cells.size(); i++) {
            for (int j = 0; j < BASE_FUNC_COUNT; j++) {
                double tmpIntQx = 0.0, tmpIntQy = 0.0, tmpIntQz = 0.0;
                for (int z = 0; z < mesh->cells[i].gp.size(); z++) {
                    tmpIntQx +=
                            mesh->cells[i].gw[z] * getT(i, mesh->cells[i].gp[z], data[i].flds) * getDfDx(j, i, mesh->cells[i].gp[z]);
                    tmpIntQy +=
                            mesh->cells[i].gw[z] * getT(i, mesh->cells[i].gp[z], data[i].flds) * getDfDy(j, i, mesh->cells[i].gp[z]);

                    tmpIntQz +=
                            mesh->cells[i].gw[z] * getT(i, mesh->cells[i].gp[z], data[i].flds) * getDfDz(j, i, mesh->cells[i].gp[z]);
                }

                //tmpIntQx *= cellJ[i];

                data[i].flds.intQx[j] -= tmpIntQx;
                data[i].flds.intQy[j] -= tmpIntQy;

                data[i].flds.intQz[j] -= tmpIntQz;
            }
        }


        // поверхностные интегралы
        Index cCount = Config::getCompCount();

        for (int i = 0; i < mesh->faces.size(); i++) {

            Face &f = mesh->faces[i];
            Index c1 = f.cells[0];
            Index c2;

            bool isBnd = f.cells.size() == 1;

            Index cCount = Config::getCompCount();

            for (int j = 0; j < BASE_FUNC_COUNT; j++) {

                double tmpIntQx1 = 0.0, tmpIntQx2 = 0.0, tmpIntQy1 = 0.0, tmpIntQy2 = 0.0, tmpIntQz1 = 0.0, tmpIntQz2 = 0.0;
                for (int iGP = 0; iGP < f.gp.size(); iGP++) {

                    Point &pt = f.gp[iGP];

                    //  ---08.03.2024---
                    PrimHeat p1(BASE_FUNC_COUNT);

                    PrimHeat p2(BASE_FUNC_COUNT);
                    convertToParam(c1, pt, p1, data[c1].flds);

                    if (isBnd) {

                        f.bnd->calcHeat(p1, p2, f.n);

                        //  ---23.04.2024---
                        //  Дополнительное условие для задачи из статьи
                        //  с граничным условием для температуры, задаваемым через полином
                        if (f.bnd->name == "FRONT" || f.bnd->name == "RIGHT" || f.bnd->name == "TOP") {
                            p2.t = 1 + pt.x * pt.x + pt.y * pt.y + pt.z * pt.z;
                        }

                    } else {

                        c2 = f.cells[1];

                        convertToParam(c2, pt, p2, data[c2].flds);                      
                        
                    }

                    double fT, fqx, fqy, fqz;
                    flux(p1, p2, f.n, fT, fqx, fqy, fqz);

                    tmpIntQx1 += f.n.x * fT * getF(j, c1, pt) * f.gw[iGP];
                    tmpIntQy1 += f.n.y * fT * getF(j, c1, pt) * f.gw[iGP];

                    tmpIntQz1 += f.n.z * fT * getF(j, c1, pt) * f.gw[iGP];

                    if (!isBnd) {
                        tmpIntQx2 += f.n.x * fT * getF(j, c2, pt) * f.gw[iGP];
                        tmpIntQy2 += f.n.y * fT * getF(j, c2, pt) * f.gw[iGP];

                        tmpIntQz2 += f.n.z * fT * getF(j, c2, pt) * f.gw[iGP];
                    }
                }

                tmpIntQx1 *= f.gj;
                tmpIntQy1 *= f.gj;

                tmpIntQz1 *= f.gj;

                tmpIntQx2 *= f.gj;
                tmpIntQy2 *= f.gj;
                tmpIntQz2 *= f.gj;

                data[c1].flds.intQx[j] += tmpIntQx1;
                data[c1].flds.intQy[j] += tmpIntQy1;

                data[c1].flds.intQz[j] += tmpIntQz1;

                if (!isBnd) {
                    data[c2].flds.intQx[j] -= tmpIntQx2;
                    data[c2].flds.intQy[j] -= tmpIntQy2;

                    data[c2].flds.intQz[j] -= tmpIntQz2;
                }

            }

        }

        //вычисляем градиенты на новом шаге по времени
        for (int i = 0; i < mesh->cells.size(); i++) {
            for (int j = 0; j < BASE_FUNC_COUNT; j++) {
                double tmpQx = 0.0, tmpQy = 0.0, tmpQz = 0.0;
                for (int z = 0; z < BASE_FUNC_COUNT; z++) {

                    tmpQx += mesh->cells[i].invA[j*BASE_FUNC_COUNT + z] * data[i].flds.intQx[z];
                    tmpQy += mesh->cells[i].invA[j*BASE_FUNC_COUNT + z] * data[i].flds.intQy[z];

                    tmpQz += mesh->cells[i].invA[j*BASE_FUNC_COUNT + z] * data[i].flds.intQz[z];

                }
                data[i].flds.qx[j] = tmpQx;
                data[i].flds.qy[j] = tmpQy;

                data[i].flds.qz[j] = tmpQz;

            }
        }
    }


    void MethodFemDgHeat::calcDiffusionVol() {
        for (int i = 0; i < mesh->cells.size(); i++) {
            for (int j = 0; j < BASE_FUNC_COUNT; j++) {
                double tmpIntT = 0.0;
                for (int z = 0; z < mesh->cells[i].gp.size(); z++) {
                    tmpIntT +=
                            mesh->cells[i].gw[z] * getQx(i, mesh->cells[i].gp[z], data[i].flds) * getDfDx(j, i, mesh->cells[i].gp[z]);
                    tmpIntT +=
                            mesh->cells[i].gw[z] * getQy(i, mesh->cells[i].gp[z], data[i].flds) * getDfDy(j, i, mesh->cells[i].gp[z]);

                    tmpIntT +=
                            mesh->cells[i].gw[z] * getQz(i, mesh->cells[i].gp[z], data[i].flds) * getDfDz(j, i, mesh->cells[i].gp[z]);
                }

                data[i].flds.intT[j] -= tmpIntT;
            }
        }
    }


    void MethodFemDgHeat::calcDiffusionSurf() {
        for (int i = 0; i < mesh->faces.size(); i++) {
            Face &f = mesh->faces[i];
            Index c1 = f.cells[0];
            Index c2;

            bool isBnd = f.cells.size() == 1;

            Index cCount = Config::getCompCount();

            for (int j = 0; j < BASE_FUNC_COUNT; j++) {

                double tmpIntT1 = 0.0, tmpIntT2 = 0.0;
                for (int iGP = 0; iGP < f.gp.size(); iGP++) {
                    Point &pt = f.gp[iGP];

                    PrimHeat p1(BASE_FUNC_COUNT);

                    PrimHeat p2(BASE_FUNC_COUNT);

                    convertToParam(c1, pt, p1, data[c1].flds);
                    if (isBnd) {
                        f.bnd->calcHeat(p1, p2, f.n);

                        //  ---24.04.2024---
                        //  Дополнительное условие для задачи из статьи
                        //  с граничным условием для температуры, задаваемым через полином
                        if (f.bnd->name == "FRONT" || f.bnd->name == "RIGHT" || f.bnd->name == "TOP") {
                            p2.t = 1 + pt.x * pt.x + pt.y * pt.y + pt.z * pt.z;
                        }

                    } else {
                        c2 = f.cells[1];
                        convertToParam(c2, pt, p2, data[c2].flds);
                    }
                    double fT, fqx, fqy, fqz;
                    flux(p1, p2, f.n, fT, fqx, fqy, fqz);

                    tmpIntT1 += f.n.x * fqx * getF(j, c1, pt) * f.gw[iGP];
                    tmpIntT1 += f.n.y * fqy * getF(j, c1, pt) * f.gw[iGP];

                    tmpIntT1 += f.n.z * fqz * getF(j, c1, pt) * f.gw[iGP];

                    if (!isBnd) {
                        tmpIntT2 += f.n.x * fqx * getF(j, c2, pt) * f.gw[iGP];
                        tmpIntT2 += f.n.y * fqy * getF(j, c2, pt) * f.gw[iGP];

                        tmpIntT2 += f.n.z * fqz * getF(j, c2, pt) * f.gw[iGP];
                    }
                }

                tmpIntT1 *= f.gj;

                tmpIntT2 *= f.gj;

                data[c1].flds.intT[j] += tmpIntT1;

                if (!isBnd) {
                    data[c2].flds.intT[j] -= tmpIntT2;
                }
            }

        }
    }


    void MethodFemDgHeat::calcNewValues() {

        Index cCount = Config::getCompCount();


        Real dt = calcDt();

        for (int i = 0; i < mesh->cells.size(); i++) {

            PrimHeat p(BASE_FUNC_COUNT);

            for (int j = 0; j < BASE_FUNC_COUNT; j++) {
                double tmpT = 0.0;
                for (int z = 0; z < BASE_FUNC_COUNT; z++) {
                    tmpT += mesh->cells[i].invA[j*BASE_FUNC_COUNT + z] * data[i].flds.intT[z];
                }

                //  ---09.03.2024---
                data[i].flds.t[j] += dt * tmpT;

                convertToParam(i, mesh->cells[i].center, data[i].p, data[i].flds);

            }

        }
    }


    // UPDATE от 07.01.2023 - переделываем функцию запуска расчёта
    void MethodFemDgHeat::run() {

        std::cout << "From MethodFemDgHeat.cpp: run is started!" << std::endl;

        while (true) {
            Index cCount = Config::getCompCount();

            Real dt = calcDt();


            conf->t += conf->dt;
            conf->timestep++;

            for (int i = 0; i < mesh->cells.size(); i++) {
                for (int j = 0; j < BASE_FUNC_COUNT; j++) {
                    data[i].flds.intT[j] = 0.;
                    data[i].flds.intQx[j] = 0.;
                    data[i].flds.intQy[j] = 0.;
                    data[i].flds.intQz[j] = 0.;
                }
            }

            calcGradients();

            calcDiffusionVol();
            calcDiffusionSurf();

            calcNewValues();

            /* Real dt = calcDt();

            conf->t += conf->dt;
            conf->timestep++; */

            save();

            if (conf->timestep % conf->logPeriod == 0) {
                std::cout << "STEP: " << conf->timestep << std::endl;
            }

            if (conf->t >= conf->time) {
                break;
            }
        }

    }


    //	UPDATE от 07.01.2023 - переделанная версия функции расчёта временного шага из MethodFvm.cpp
    Real MethodFemDgHeat::calcDt() {
        Config *conf = Config::get();
        Real dt = conf->dt;

        dt = conf->dt;

        return dt;
    }


    double MethodFemDgHeat::getQx(int iCell, Point pt, HeatDgFields &flds) {
        double d = 0.0;
        for (int i = 0; i < BASE_FUNC_COUNT; i++) {
            d += flds.qx[i] * getF(i, iCell, pt);
        }
        return d;
    }

    double MethodFemDgHeat::getQy(int iCell, Point pt, HeatDgFields &flds) {
        double d = 0.0;
        for (int i = 0; i < BASE_FUNC_COUNT; i++) {
            d += flds.qy[i] * getF(i, iCell, pt);
        }
        return d;
    }

    double MethodFemDgHeat::getQz(int iCell, Point pt, HeatDgFields &flds) {
        double d = 0.0;
        for (int i = 0; i < BASE_FUNC_COUNT; i++) {
            d += flds.qz[i] * getF(i, iCell, pt);
        }
        return d;
    }


    //	UPDATE от 07.01.2023 - так ли рассчитывается поток в разрывном методе Галёркина???
    /* void MethodFemDgHeat::flux(PrimHeat pl, PrimHeat pr, Point n, double &fT, double &fqx, double &fqy, double &fqz) {

        fT = 0.5 * (pl.t + pr.t);
        fqx = 0.5 * (pl.qx + pr.qx) + C11 * (pr.t - pl.t) * n.x;
        fqy = 0.5 * (pl.qy + pr.qy) + C11 * (pr.t - pl.t) * n.y;

        fqz = 0.5 * (pl.qz + pr.qz) + C11 * (pr.t - pl.t) * n.z;

    } */

    //  ---10.05.2024---
    //  В источнике Zhang было рекомендовано использовать не центральные потоки, а чередовать значения слева и справа для T и q, соответственно
    // T - снаружи, q - внутри
    /* void MethodFemDgHeat::flux(PrimHeat pl, PrimHeat pr, Point n, double &fT, double &fqx, double &fqy, double &fqz) {

        fT = pr.t;
        fqx = pl.qx;
        fqy = pl.qy;
        fqz = pl.qz;

    } */

    //  ---10.05.2024---
    //  T - внутри, q - снаружи
    void MethodFemDgHeat::flux(PrimHeat pl, PrimHeat pr, Point n, double &fT, double &fqx, double &fqy, double &fqz) {

        fT = pl.t;
        fqx = pr.qx;
        fqy = pr.qy;
        fqz = pr.qz;

    }


    void MethodFemDgHeat::done() {

    }


    void MethodFemDgHeat::save() {
        if (conf->timestep % conf->writePeriod == 0) {
            saveVtk();
        }
    }


    //	UPDATE от 27.12.2022 - другая версия функции saveVtk (из nummeth2019)
    void MethodFemDgHeat::saveVtk() // @todo:
    {
        Index cCount = Config::getCompCount();
        Prim p(cCount);
        Material *mat;
        String fileName;
        std::ofstream fout;

        Index nodesCount = mesh->nodes.size();
        Index cellsCount = mesh->cells.size();

        std::stringstream ss;
        ss << "charm_" << std::setfill('0') << std::setw(8) << conf->timestep << ".vtk";

        fileName = ss.str();

        std::cout << " Writing VTK file '" << fileName << "'..." << std::endl;

        fout.open(fileName, std::ios_base::out);
        fout << "# vtk DataFile Version 3.0" << std::endl;
        fout << "3D RKDG method results for task '%s'." << std::endl;
        fout << "ASCII" << std::endl;
        fout << "DATASET UNSTRUCTURED_GRID" << std::endl;


        fout << "POINTS " << nodesCount << " double" << std::endl;

        for (const auto &node: mesh->nodes) {
            fout << node.x << " " << node.y << " " << node.z << std::endl;
        }

        fout << std::endl << "CELLS " << cellsCount << " " << 9 * cellsCount << std::endl;

        for (const auto &cell: mesh->cells) {

            fout << cell.nodesInd.size();
            for (auto nIdx: cell.nodesInd) {
                fout << " " << nIdx;
            }
            fout << std::endl;
        }

        fout << std::endl << "CELL_TYPES " << cellsCount << std::endl;

        for (int i = 1; i <= cellsCount; i++) {
            fout << "11 ";
            if (i % 20 == 0) {
                fout << std::endl;
            }
        }
        fout << std::endl;


        //	UPDATE от 07.01.2023 - убираем из вывода всё, кроме температуры
        fout << "CELL_DATA " << cellsCount << std::endl;

        // Температура
        fout << std::endl << "SCALARS Temperature double 1" << std::endl << "LOOKUP_TABLE default" << std::endl;
        for (int i = 0; i < cellsCount; i++) {

            fout << getT(i, mesh->cells[i].center, data[i].flds) << " ";
            if (i % 8 == 0 || i == cellsCount) {
                fout << std::endl;
            }
        }

        //  ---01.04.2024---
        // Вектор градиента температуры
        fout << std::endl << "VECTORS TotalTemperatureGradient double" << std::endl;
        for (int i = 0; i < cellsCount; i++)
        {
            fout << getQx(i, mesh->cells[i].center, data[i].flds) << " " << getQy(i, mesh->cells[i].center, data[i].flds) << " " << getQz(i, mesh->cells[i].center, data[i].flds) << " ";
            if (i % 8 == 0  ||  i == cellsCount) {
                fout << std::endl;
            }
        }

        //  x-координата градиента температуры
        fout << std::endl << "SCALARS TemperatureGradientX double 1" << std::endl << "LOOKUP_TABLE default" << std::endl;
        for (int i = 0; i < cellsCount; i++) {

            fout << getQx(i, mesh->cells[i].center, data[i].flds) << " ";
            if (i % 8 == 0 || i == cellsCount) {
                fout << std::endl;
            }
        }

        //  y-координата градиента температуры
        fout << std::endl << "SCALARS TemperatureGradientY double 1" << std::endl << "LOOKUP_TABLE default" << std::endl;
        for (int i = 0; i < cellsCount; i++) {

            fout << getQy(i, mesh->cells[i].center, data[i].flds) << " ";
            if (i % 8 == 0 || i == cellsCount) {
                fout << std::endl;
            }
        }

        //  z-координата градиента температуры
        fout << std::endl << "SCALARS TemperatureGradientZ double 1" << std::endl << "LOOKUP_TABLE default" << std::endl;
        for (int i = 0; i < cellsCount; i++) {

            fout << getQz(i, mesh->cells[i].center, data[i].flds) << " ";
            if (i % 8 == 0 || i == cellsCount) {
                fout << std::endl;
            }
        }

        /*
            // Полный объём
            fout << std::endl << "SCALARS Volume double 1" << std::endl << "LOOKUP_TABLE default" << std::endl;
            for (int i = 0; i < cellsCount; i++)
            {
                    fout << mesh->cells[i].volume << " ";
                    if (i % 8 == 0  ||  i == cellsCount) {
                        fout << std::endl;
                    }
            }
        */

        // id ячеек
        fout << std::endl << "SCALARS CellId double 1" << std::endl << "LOOKUP_TABLE default" << std::endl;
        for (int i = 0; i < cellsCount; i++) {
            fout << i << " ";
            if (i % 8 == 0 || i == cellsCount) {
                fout << std::endl;
            }
        }

        fout.close();
        std::cout << "File '" << fileName << "' is saved." << std::endl;
    }

}