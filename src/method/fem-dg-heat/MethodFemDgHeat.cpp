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

    //	UPDATE от 26.12.2022 - перенесла сюда функции matr4Det и matr4Inv из global.cpp

    //  UPDATE ON 10.07.2023 - проверить вычисление определителя!!!
    // Добавляю вычисление определителя для матрицы 4*4
    Real matr4Det(Real **a) {
        Real det_a = a[0][3] * a[1][2] * a[2][1] * a[3][0] - a[0][2] * a[1][3] * a[2][1] * a[3][0] -
                     a[0][3] * a[1][1] * a[2][2] * a[3][0] + a[0][1] * a[1][3] * a[2][2] * a[3][0] +
                     a[0][2] * a[1][1] * a[2][3] * a[3][0] - a[0][1] * a[1][2] * a[2][3] * a[3][0] -
                     a[0][3] * a[1][2] * a[2][0] * a[3][1] + a[0][2] * a[1][3] * a[2][0] * a[3][1] +
                     a[0][3] * a[1][0] * a[2][2] * a[3][1] - a[0][0] * a[1][3] * a[2][2] * a[3][1] -
                     a[0][2] * a[1][0] * a[2][3] * a[3][1] + a[0][0] * a[1][2] * a[2][3] * a[3][1] +
                     a[0][3] * a[1][1] * a[2][0] * a[3][2] - a[0][1] * a[1][3] * a[2][0] * a[3][2] -
                     a[0][3] * a[1][0] * a[2][1] * a[3][2] + a[0][0] * a[1][3] * a[2][1] * a[3][2] +
                     a[0][1] * a[1][0] * a[2][3] * a[3][2] - a[0][0] * a[1][1] * a[2][3] * a[3][2] -
                     a[0][2] * a[1][1] * a[2][0] * a[3][3] + a[0][1] * a[1][2] * a[2][0] * a[3][3] +
                     a[0][2] * a[1][0] * a[2][1] * a[3][3] - a[0][0] * a[1][2] * a[2][1] * a[3][3] -
                     a[0][1] * a[1][0] * a[2][2] * a[3][3] + a[0][0] * a[1][1] * a[2][2] * a[3][3];

        return det_a;
    }

    //	Добавляю инверсию матрицы 4*4, если буду брать 4 базисные функции в методе Галёркина!!!

    void matr4Inv(Real **a, Real **a_inv) {
        Real a_[4][4];
        int i, j;
        Real det_a = matr4Det(a);

        //CHARM_ASSERT(det_a != 0.);

        a_[0][0] = -a[1][3] * a[2][2] * a[3][1] + a[1][2] * a[2][3] * a[3][1] +
                   a[1][3] * a[2][1] * a[3][2] - a[1][1] * a[2][3] * a[3][2] -
                   a[1][2] * a[2][1] * a[3][3] + a[1][1] * a[2][2] * a[3][3];
        a_[0][1] = a[0][3] * a[2][2] * a[3][1] - a[0][2] * a[2][3] * a[3][1] -
                   a[0][3] * a[2][1] * a[3][2] + a[0][1] * a[2][3] * a[3][2] +
                   a[0][2] * a[2][1] * a[3][3] - a[0][1] * a[2][2] * a[3][3];
        a_[0][2] = -a[0][3] * a[1][2] * a[3][1] + a[0][2] * a[1][3] * a[3][1] +
                   a[0][3] * a[1][1] * a[3][2] - a[0][1] * a[1][3] * a[3][2] -
                   a[0][2] * a[1][1] * a[3][3] + a[0][1] * a[1][2] * a[3][3];
        a_[0][3] = a[0][3] * a[1][2] * a[2][1] - a[0][2] * a[1][3] * a[2][1] -
                   a[0][3] * a[1][1] * a[2][2] + a[0][1] * a[1][3] * a[2][2] +
                   a[0][2] * a[1][1] * a[2][3] - a[0][1] * a[1][2] * a[2][3];
        a_[1][0] = a[1][3] * a[2][2] * a[3][0] - a[1][2] * a[2][3] * a[3][0] -
                   a[1][3] * a[2][0] * a[3][2] + a[1][0] * a[2][3] * a[3][2] +
                   a[1][2] * a[2][0] * a[3][3] - a[1][0] * a[2][2] * a[3][3];
        a_[1][1] = -a[0][3] * a[2][2] * a[3][0] + a[0][2] * a[2][3] * a[3][0] +
                   a[0][3] * a[2][0] * a[3][2] - a[0][0] * a[2][3] * a[3][2] -
                   a[0][2] * a[2][0] * a[3][3] + a[0][0] * a[2][2] * a[3][3];
        a_[1][2] = a[0][3] * a[1][2] * a[3][0] - a[0][2] * a[1][3] * a[3][0] -
                   a[0][3] * a[1][0] * a[3][2] + a[0][0] * a[1][3] * a[3][2] +
                   a[0][2] * a[1][0] * a[3][3] - a[0][0] * a[1][2] * a[3][3];
        a_[1][3] = -a[0][3] * a[1][2] * a[2][0] + a[0][2] * a[1][3] * a[2][0] +
                   a[0][3] * a[1][0] * a[2][2] - a[0][0] * a[1][3] * a[2][2] -
                   a[0][2] * a[1][0] * a[2][3] + a[0][0] * a[1][2] * a[2][3];
        a_[2][0] = -a[1][3] * a[2][1] * a[3][0] + a[1][1] * a[2][3] * a[3][0] +
                   a[1][3] * a[2][0] * a[3][1] - a[1][0] * a[2][3] * a[3][1] -
                   a[1][1] * a[2][0] * a[3][3] + a[1][0] * a[2][1] * a[3][3];
        a_[2][1] = a[0][3] * a[2][1] * a[3][0] - a[0][1] * a[2][3] * a[3][0] -
                   a[0][3] * a[2][0] * a[3][1] + a[0][0] * a[2][3] * a[3][1] +
                   a[0][1] * a[2][0] * a[3][3] - a[0][0] * a[2][1] * a[3][3];
        a_[2][2] = -a[0][3] * a[1][1] * a[3][0] + a[0][1] * a[1][3] * a[3][0] +
                   a[0][3] * a[1][0] * a[3][1] - a[0][0] * a[1][3] * a[3][1] -
                   a[0][1] * a[1][0] * a[3][3] + a[0][0] * a[1][1] * a[3][3];
        a_[2][3] = a[0][3] * a[1][1] * a[2][0] - a[0][1] * a[1][3] * a[2][0] -
                   a[0][3] * a[1][0] * a[2][1] + a[0][0] * a[1][3] * a[2][1] +
                   a[0][1] * a[1][0] * a[2][3] - a[0][0] * a[1][1] * a[2][3];
        a_[3][0] = a[1][2] * a[2][1] * a[3][0] - a[1][1] * a[2][2] * a[3][0] -
                   a[1][2] * a[2][0] * a[3][1] + a[1][0] * a[2][2] * a[3][1] +
                   a[1][1] * a[2][0] * a[3][2] - a[1][0] * a[2][1] * a[3][2];
        a_[3][1] = -a[0][2] * a[2][1] * a[3][0] + a[0][1] * a[2][2] * a[3][0] +
                   a[0][2] * a[2][0] * a[3][1] - a[0][0] * a[2][2] * a[3][1] -
                   a[0][1] * a[2][0] * a[3][2] + a[0][0] * a[2][1] * a[3][2];
        a_[3][2] = a[0][2] * a[1][1] * a[3][0] - a[0][1] * a[1][2] * a[3][0] -
                   a[0][2] * a[1][0] * a[3][1] + a[0][0] * a[1][2] * a[3][1] +
                   a[0][1] * a[1][0] * a[3][2] - a[0][0] * a[1][1] * a[3][2];
        a_[3][3] = -a[0][2] * a[1][1] * a[2][0] + a[0][1] * a[1][2] * a[2][0] +
                   a[0][2] * a[1][0] * a[2][1] - a[0][0] * a[1][2] * a[2][1] -
                   a[0][1] * a[1][0] * a[2][2] + a[0][0] * a[1][1] * a[2][2];

        //  UPDATE ON 16.07.2023 - возможно, ошибка была в этом: должен проходить цикл до 4!!!
        for (i = 0; i < 4; i++) {
            for (j = 0; j < 4; j++) {
                a_inv[i][j] = a_[i][j] / det_a;
            }
        }
    }

    //	UPDATE от 07.01.2023 - пытаюсь переделать программу по образцу MethodFvm.cpp

    //  UPDATE ON 12.07.2023 - меняю Prim на PrimHeat
    //	Конвертация в примитивные переменные
    void MethodFemDgHeat::convertToParam(int i, Point pt, PrimHeat &p) {

        //	UPDATE от 27.12.2022 - неиспользуемым переменным присваиваем произвольные значения

        // UPDATE от 07.01.2023 - оставляем p.r таким

        //  UPDATE ON 12.07.2023 - совсем убираю плотность, давление, скорость и энергию

        //  p.r = 1000.0;
        //  p.p = 101325.0;
        //  p.v[0] = 0.0;
        //  p.v[1] = 0.0;
        //  p.v[2] = 0.0;

        // UPDATE от 07.01.2023 - оставляем p.e таким
        //  p.e = 100.0;

        p.t = getT(i, pt);
        //p.M = 0.0;
        p.qx = getQx(i, pt);
        p.qy = getQy(i, pt);
        p.qz = getQz(i, pt);
    }


    //	Вроде бы, все параметры типа gw, gp вычисляются в Mesh.cpp
    /*
    void MethodFemDgHeat::calcGaussParam() {

        //	Переработать эту функцию из nummeth2019 под трёхмерный случай!
        // для ячеек
            if (GP_CELL_COUNT == 8) {
                for (int i = 0; i < mesh->cells.size(); i++) {

                        double a = 1.0 / 3.0;
                        double b = 1.0 / 5.0;
                        double c = 3.0 / 5.0;

                        double x1 = mesh->nodes[mesh->cells[i].nodesInd[0]].x;
                        double y1 = mesh->nodes[mesh->cells[i].nodesInd[0]].y;
                        double x2 = mesh->nodes[mesh->cells[i].nodesInd[1]].x;
                        double y2 = mesh->nodes[mesh->cells[i].nodesInd[1]].y;
                        double x3 = mesh->nodes[mesh->cells[i].nodesInd[2]].x;
                        double y3 = mesh->nodes[mesh->cells[i].nodesInd[2]].y;
                        double a1 = x1 - x3;
                        double a2 = y1 - y3;
                        double b1 = x2 - x3;
                        double b2 = y2 - y3;
                        double c1 = x3;
                        double c2 = y3;

                        cellGW[i][0] = -27.0 / 48.0;
                        cellGP[i][0].x = a1 * a + b1 * a + c1;
                        cellGP[i][0].y = a2 * a + b2 * a + c2;

                        cellGW[i][1] = 25.0 / 48.0;
                        cellGP[i][1].x = a1 * c + b1 * b + c1;
                        cellGP[i][1].y = a2 * c + b2 * b + c2;

                        cellGW[i][2] = 25.0 / 48.0;
                        cellGP[i][2].x = a1 * b + b1 * c + c1;
                        cellGP[i][2].y = a2 * b + b2 * c + c2;

                        cellGW[i][3] = 25.0 / 48.0;
                        cellGP[i][3].x = a1 * b + b1 * b + c1;
                        cellGP[i][3].y = a2 * b + b2 * b + c2;

                        cellJ[i] = 0.5 * fabs(a1 * b2 - a2 * b1);
                }

            }
        else if (GP_CELL_COUNT == 3) {
                for (int i = 0; i < mesh->cCount; i++) {
                        double a = 1.0 / 6.0;
                        double b = 2.0 / 3.0;
                        double x1 = mesh->nodes[mesh->cells[i].nodesInd[0]].x;
                        double y1 = mesh->nodes[mesh->cells[i].nodesInd[0]].y;
                        double x2 = mesh->nodes[mesh->cells[i].nodesInd[1]].x;
                        double y2 = mesh->nodes[mesh->cells[i].nodesInd[1]].y;
                        double x3 = mesh->nodes[mesh->cells[i].nodesInd[2]].x;
                        double y3 = mesh->nodes[mesh->cells[i].nodesInd[2]].y;
                        double a1 = x1 - x3;
                        double a2 = y1 - y3;
                        double b1 = x2 - x3;
                        double b2 = y2 - y3;
                        double c1 = x3;
                        double c2 = y3;

                        cellGW[i][0] = 1.0 / 6.0;
                        cellGP[i][0].x = a1 * a + b1 * a + c1;
                        cellGP[i][0].y = a2 * a + b2 * a + c2;

                        cellGW[i][1] = 1.0 / 6.0;
                        cellGP[i][1].x = a1 * a + b1 * b + c1;
                        cellGP[i][1].y = a2 * a + b2 * b + c2;

                        cellGW[i][2] = 1.0 / 6.0;
                        cellGP[i][2].x = a1 * b + b1 * a + c1;
                        cellGP[i][2].y = a2 * b + b2 * a + c2;

                        cellJ[i] = fabs(a1 * b2 - a2 * b1);

                }
            }


            // для ребер
            if (GP_EDGE_COUNT == 3) {
                for (int i = 0; i < mesh->eCount; i++) {
                        double gp1 = -3.0 / 5.0;
                        double gp2 = 0.0;
                        double gp3 = 3.0 / 5.0;
                        double x1 = mesh->nodes[mesh->edges[i].n1].x;
                        double y1 = mesh->nodes[mesh->edges[i].n1].y;
                        double x2 = mesh->nodes[mesh->edges[i].n2].x;
                        double y2 = mesh->nodes[mesh->edges[i].n2].y;

                        edgeGW[i][0] = 5.0 / 9.0;
                        edgeGP[i][0].x = ((x1 + x2) + gp1 * (x2 - x1)) / 2.0;
                        edgeGP[i][0].y = ((y1 + y2) + gp1 * (y2 - y1)) / 2.0;

                        edgeGW[i][1] = 8.0 / 9.0;
                        edgeGP[i][1].x = (x1 + x2) / 2.0;
                        edgeGP[i][1].y = (y1 + y2) / 2.0;

                        edgeGW[i][2] = 5.0 / 9.0;
                        edgeGP[i][2].x = ((x1 + x2) + gp3 * (x2 - x1)) / 2.0;
                        edgeGP[i][2].y = ((y1 + y2) + gp3 * (y2 - y1)) / 2.0;

                        edgeJ[i] = sqrt((x2 - x1)*(x2 - x1) + (y2 - y1)*(y2 - y1)) * 0.5;

                }
            } else if (GP_EDGE_COUNT == 2) {
                for (int i = 0; i < mesh->eCount; i++) {
                        double _sqrt3 = 1.0 / sqrt(3.0);
                        double x1 = mesh->nodes[mesh->edges[i].n1].x;
                        double y1 = mesh->nodes[mesh->edges[i].n1].y;
                        double x2 = mesh->nodes[mesh->edges[i].n2].x;
                        double y2 = mesh->nodes[mesh->edges[i].n2].y;

                        edgeGW[i][0] = 1.0;
                        edgeGP[i][0].x = ((x1 + x2) - _sqrt3 * (x2 - x1)) * 0.5;
                        edgeGP[i][0].y = ((y1 + y2) - _sqrt3 * (y2 - y1)) * 0.5;

                        edgeGW[i][1] = 1.0;
                        edgeGP[i][1].x = ((x1 + x2) + _sqrt3 * (x2 - x1)) * 0.5;
                        edgeGP[i][1].y = ((y1 + y2) + _sqrt3 * (y2 - y1)) * 0.5;

                        edgeJ[i] = sqrt((x2 - x1)*(x2 - x1) + (y2 - y1)*(y2 - y1)) * 0.5;

                }
            }
    }
    */



    //	Методы, реализующие получение значений из полей
    double MethodFemDgHeat::getT(int iCell, Point pt) {
        double d = 0.;
        for (int i = 0; i < BASE_FUNC_COUNT; i++) {
            d += T[iCell][i] * getF(i, iCell, pt);
        }

        //	UPDATE от 27.12.2022 - добавляю return
        return d;
    }

    //	Переделала эту функцию в соответствии с обозначениями из charm_3d_v2

    //  UPDATE ON 12.07.2023 - пробую пока закомментировать данные варианты функций getF, getDfDx, getDfDy, getDfDz

    //double MethodFemDgHeat::getF(int i, int iCell, Point pt) {
    //    return ((DataDgHeat *) getData(iCell))->getF(i, pt);
    //}


    //double MethodFemDgHeat::getDfDx(int i, int iCell, Point pt) {
    //    return ((DataDgHeat *) getData(iCell))->getDfDx(i, pt);
    //}


    //double MethodFemDgHeat::getDfDy(int i, int iCell, Point pt) {
    //    return ((DataDgHeat *) getData(iCell))->getDfDy(i, pt);
    //}


    //double MethodFemDgHeat::getDfDz(int i, int iCell, Point pt) {
    //    return ((DataDgHeat *) getData(iCell))->getDfDz(i, pt);
    //}

    //  UPDATE ON 12.07.2023 - добавляю реализацию функций getF, getDfDx, getDfDy, getDfDz
    Real MethodFemDgHeat::getF(Int i, Int iCell, Point pt)
    {
        //  UPDATE ON 12.07.2023 - меняю название переменной
        //if (ConfigFemDgHeat::baseFuncCount == 4)
        if (BASE_FUNC_COUNT == 4)
        {
            switch (i) {
                case 0:
                    return 1.0;
                case 1:
                    //  UPDATE ON 10.07.2023 - всё-таки лучше добавить индекс ячейки...
                    //return (pt.x - cell->center.x) / cell->dh[0];
                    //return (pt.x - mesh->cells[iCell].center.x) / mesh->cells[iCell].dh[0];
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
        //  UPDATE ON 12.07.2023 - меняю название переменной
        //if (ConfigFemDgHeat::baseFuncCount == 4)
        if (BASE_FUNC_COUNT == 4)
        {
            switch(i) {
                case 0:
                    return 0.0;
                case 1:
                    //  UPDATE ON 10.07.2023 - всё-таки лучше добавить индекс ячейки...
                    //  return 1.0 / cell->dh[0];
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
        //  UPDATE ON 12.07.2023 - меняю название переменной
        //if (ConfigFemDgHeat::baseFuncCount == 4)
        if (BASE_FUNC_COUNT == 4)
        {
            switch(i) {
                case 0:
                    return 0.0;
                case 1:
                    return 0.0;
                case 2:
                    //  return 1.0 / cell->dh[1];
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
        //  UPDATE ON 12.07.2023 - меняю название переменной
        //if (ConfigFemDgHeat::baseFuncCount == 4)
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
                    //  return 1.0 / cell->dh[2];
                    return 1.0 / mesh->cells[iCell].dh[2];
            }
        }

        else {
            printf("ERROR: wrong basic functions count!\n");
            exit(1);
        }
    }


    //	UPDATE от 07.01.2023 - меняем функцию инициализации

    //	Функция инициализации
    void MethodFemDgHeat::init() {

        std::cout << "From MethodFemDgHeat.cpp: init started!" << std::endl;

        //  UPDATE ON 10.07.2023 - меняем процесс получения количества ячеек в сетке
        //  Index cN = mesh->cCountGhost;
        Index cN = mesh->cells.size();
        //  UPDATE ON 10.07.2023 - меняю написание на BASE_FUNC_COUNT
        //data.resize(cN, DataDgHeat(ConfigFemDgHeat::baseFuncCount));
        data.resize(cN, DataDgHeat(BASE_FUNC_COUNT));

        //  UPDATE ON 12.07.2023 - начальная инициализация массивов температуры и тепловых потоков
        T = new double*[cN];
        qx = new double*[cN];
        qy = new double*[cN];
        qz = new double*[cN];

        intT = new double*[cN];
        intQx = new double*[cN];
        intQy = new double*[cN];
        intQz = new double*[cN];

        std::cout << "From MethodFemDgHeat.cpp: init continued!" << std::endl;
        for (Index ic = 0; ic < cN; ic++) {
            std::cout << "From MethodFemDgHeat.cpp: init continued cycle 0!" << std::endl;
            Cell &cell = mesh->cells[ic];
            std::cout << "From MethodFemDgHeat.cpp: init continued cycle 1!" << std::endl;
            std::cout << "cell.tag from MethodFemDgHeat.cpp: " << cell.tag << std::endl;
            Region *reg = Config::getRegion(cell.tag);
            std::cout << "From MethodFemDgHeat.cpp: init continued cycle 2!" << std::endl;

            //  UPDATE ON 10.07.2023 - пробую заполнять массивы температуры и тепловых потоков без использования явных многомерных массивов
            //  UPDATE ON 13.07.2023 - пробую убрать круглые скобки при создании экземпляра класса PrimHeat
            //  PrimHeat p();
            PrimHeat p;
            p.t = reg->t;
            //  UPDATE ON 10.07.2023 - принимаем, что в начальный момент времени тепловые потоки равны 0
            p.qx = 0.0;
            p.qy = 0.0;
            p.qz = 0.0;

            //  UPDATE ON 13.07.2023 - пока убираю этот фрагмент
            //  Может быть, он имел ключевое значение при передаче данных...
            //  data[ic].setVar(p);

            //	UPDATE от 29.12.2022 - температура инициализируется здесь!!!
            //	А должна бы из файла...

            //	UPDATE от 02.01.2023 - нужно задать начальную температуру как функцию, зависящую от координат
            //	Пытаюсь решить тест из статьи ИПМ им. Келдыша...
            //	Попробую оставить ненулевой только первый элемент T[ic]

            T[ic] = new double[BASE_FUNC_COUNT];
            memset(T[ic], 0.0, BASE_FUNC_COUNT * sizeof(double));

            //  UPDATE ON 10.07.2023 - пробуем код на базовой задаче - сохранении нулевых условий
            //  UPDATE ON 10.07.2023 - каково соотношение примитивных и консервативных переменных???
            //  ИТОГ - пока оставляю двумерный массив T
            //  T[ic][0] = cos(M_PI * cell.center.x) * cos(M_PI * cell.center.y) * cos(M_PI * cell.center.z);
            
            //  UPDATE ON 13.07.2023 - тест с нулевой температурой успешно пройден, приступаем к следующему
            //  Нулевая температура на границе и произведение синусов как начальное условие
            //  T[ic][0] = 0.0;
            T[ic][0] = sin(M_PI * cell.center.x) * sin(M_PI * cell.center.y) * sin(M_PI * cell.center.z);



            //  std::cout << cell.center.x << std::endl;

            /*
            for (int j=0;j<BASE_FUNC_COUNT;j++){
                T[ic][j] = 300.;
            }
            */

            //  UPDATE ON 12.07.2023 - лучше пока попробовать передавать данные через массивы

            qx[ic] = new double[BASE_FUNC_COUNT];
            qy[ic] = new double[BASE_FUNC_COUNT];
            qz[ic] = new double[BASE_FUNC_COUNT];

            intT[ic] = new double[BASE_FUNC_COUNT];
            intQx[ic] = new double[BASE_FUNC_COUNT];
            intQy[ic] = new double[BASE_FUNC_COUNT];
            intQz[ic] = new double[BASE_FUNC_COUNT];

            //cellGP[ic] = new Point[GP_CELL_COUNT];
            //cellGW[ic] = new double[GP_CELL_COUNT];

            std::cout << "From MethodFemDgHeat.cpp: init continued cycle 3!" << std::endl;


            // Возможно, одно дублирует другое...
            //  Prim p(cCount);
            //  p.matId = reg->matId;

            //	UPDATE от 07.01.2023 - p.v, p.t и p.p инициализируем в функции convertToParam()

            //p.v     = reg->v;
            //p.t     = reg->t;
            //p.p     = reg->p;


            //  convertToParam(ic, cell.center, p);

            //  p.c.assign(reg->c.begin(), reg->c.end());
            //  data[ic].grav = reg->grav;
            //  p.eos(Material::EOS_T_P_TO_R_CZ_E);
            //  p.eTot = p.e + 0.5 * p.v.sqr();


            //	UPDATE от 07.01.2023 - нужно ли применять setCons(p)???

            //  data[ic].setCons(p);

            //	UPDATE от 27.12.2022

            //std::cout<<p.t<<std::endl;


            //	UPDATE от 28.12.2022
            //	Можно ли так???

            //for (Index j=0; j< BASE_FUNC_COUNT;j++)	{
            //	T[ic][j] = p.t;
            //}

            std::cout << "From MethodFemDgHeat.cpp: init continued cycle 4!" << std::endl;

        }


        std::cout << "From MethodFemDgHeat.cpp: init continued 2!" << std::endl;

        //	Вроде, запись в файл vtk есть в другом месте кода
        //	А вычисление параметров Гаусса - в классе Mesh

        /*
        saveVTK(0);
        */

        /*
            for(int i = 0; i < fN; i++){
                edgeGP[i] = new Point[GP_EDGE_COUNT];
                edgeGW[i] = new double[GP_EDGE_COUNT];
            }
        */

        //calcGaussParam();


        A = new double **[cN];
        invA = new double **[cN];
        for (int i = 0; i < cN; i++) {
            A[i] = new double *[BASE_FUNC_COUNT];
            invA[i] = new double *[BASE_FUNC_COUNT];
            for (int j = 0; j < BASE_FUNC_COUNT; j++) {
                A[i][j] = new double[BASE_FUNC_COUNT];
                invA[i][j] = new double[BASE_FUNC_COUNT];
            }
        }
        calcMassMatrix();


        //integrals.resize(cN, Cons(cCount));
        //integrals.shrink_to_fit();

        //	UPDATE от 25.12.2022 - пока убираем переменную flux

        //flux = new FluxHllc(); ///< @todo @todo
        save();

        //save(0);
    }


    //	Переделала формулы под обозначения gw и gp в charm_3d_v2
    void MethodFemDgHeat::calcMassMatrix() {
        for (int i = 0; i < mesh->cells.size(); i++) {
            double **mA = A[i];

            //  UPDATE ON 14.07.2023 - проверяем, какое значение принимает mA
            //  std::cout << i << ": " << **mA << std::endl;

            double **invA_ = invA[i];
            for (int i = 0; i < BASE_FUNC_COUNT; i++) {
                for (int j = 0; j < BASE_FUNC_COUNT; j++) {
                    mA[i][j] = 0.0;
                    //  UPDATE ON 10.07.2023 - меняем GP_CELL_COUNT на длину gp, вычисленного в классе Mesh
                    //  for (int iGP = 0; iGP < GP_CELL_COUNT; iGP++) {
                    for (int iGP = 0; iGP < mesh->cells[i].gp.size(); iGP++) {
                        //mA[i][j] += cellGW[i][iGP] * getF(i, i, cellGP[ic][iGP]) * getF(j, i, cellGP[ic][iGP]);
                        mA[i][j] += mesh->cells[i].gw[iGP] * getF(i, i, mesh->cells[i].gp[iGP]) *
                                    getF(j, i, mesh->cells[i].gp[iGP]);
                    }
                    //mA[i][j] *= cellJ[i];

                    //  UPDATE ON 16.07.2023 - попробую в качестве аналога cellJ взять cell.volume / 8.
                    //  Лучше проверить, совпадают ли (mesh->cells[i].dh[0] * mesh->cells[i].dh[1] * mesh->cells[i].dh[2]) / 8. и  cell.volume / 8.

                    //  std::cout << i << ": " << mesh->cells[i].volume / 8. << std::endl;
                    //  std::cout << i << ": " << (mesh->cells[i].dh[0] * mesh->cells[i].dh[1] * mesh->cells[i].dh[2]) / 8. << std::endl;

                    //  mA[i][j] *= (mesh->cells[i].dh[0] * mesh->cells[i].dh[1] * mesh->cells[i].dh[2]) / 8.;
                    mA[i][j] *= mesh->cells[i].volume / 8.;

                    //	UPDATE от 28.12.2022 - попытка отловить ошибку

                    //std::cout<<"mA["<<i<<"]["<<j<<"] = "<<mA[i][j]<<std::endl;

                    //  std::cout << ic << ": " << mA[i][j] << std::endl;

                }
            }
            //inverseMatrix(mA, invA_, BASE_FUNC_COUNT);
            matr4Inv(mA, invA_);

            //	UPDATE от 28.12.2022 - попытка отловить ошибку
            //	Скорее всего, матрица invA_ составляется правильно...

            
            for (int i = 0; i < BASE_FUNC_COUNT; i++) {
                        for (int j = 0; j < BASE_FUNC_COUNT; j++) {
                    std::cout<<"invA_["<<i<<"]["<<j<<"] = "<<invA_[i][j]<<std::endl;
                }
            }
            

        }
    }

    void MethodFemDgHeat::calcGradients() {

        //	UPDATE от 26.12.2022 - вывод вспомогательной информации

        //std::cout<<"From MethodFemDgHeat.cpp: calcGradients started!"<<std::endl;

        //  UPDATE ON 13.07.2023 - ошибка где-то в функции calcGradients, нужно определить, где именно...
        //  std::cout << "From MethodFemDgHeat.cpp: calcGradients is started!" << std::endl;

        // поверхностные интегралы (в нашем случае - объёмные???)
        for (int i = 0; i < mesh->cells.size(); i++) {
            for (int j = 0; j < BASE_FUNC_COUNT; j++) {
                double tmpIntQx = 0.0, tmpIntQy = 0.0, tmpIntQz = 0.0;
                //  UPDATE ON 12.07.2023 - аналог GP_CELL_COUNT задаю из mesh
                //  for (int z = 0; z < GP_CELL_COUNT; z++) {
                for (int z = 0; z < mesh->cells[i].gp.size(); z++) {
                    //tmpIntQx += cellGW[i][z] * getT(i, cellGP[i][z]) * getDfDx(j, i, cellGP[i][z]);
                    tmpIntQx +=
                            mesh->cells[i].gw[z] * getT(i, mesh->cells[i].gp[z]) * getDfDx(j, i, mesh->cells[i].gp[z]);
                    tmpIntQy +=
                            mesh->cells[i].gw[z] * getT(i, mesh->cells[i].gp[z]) * getDfDy(j, i, mesh->cells[i].gp[z]);

                    tmpIntQz +=
                            mesh->cells[i].gw[z] * getT(i, mesh->cells[i].gp[z]) * getDfDz(j, i, mesh->cells[i].gp[z]);
                }

                // Так ли посчитан cellJ???
                //tmpIntQx *= cellJ[i];


                //  std::cout << "From MethodFemDgHeat.cpp: calcGradients is continued cycle 1!" << std::endl;


                //std::cout<<"dh[0]:"<<mesh->cells[i].dh[0]<<std::endl;

                //  UPDATE ON 16.07.2023 - меняю аналог cellJ
                //  tmpIntQx *= (mesh->cells[i].dh[0] * mesh->cells[i].dh[1] * mesh->cells[i].dh[2]) / 8.;
                tmpIntQx *= mesh->cells[i].volume / 8.;

                //  tmpIntQy *= (mesh->cells[i].dh[0] * mesh->cells[i].dh[1] * mesh->cells[i].dh[2]) / 8.;
                //  tmpIntQz *= (mesh->cells[i].dh[0] * mesh->cells[i].dh[1] * mesh->cells[i].dh[2]) / 8.;
                tmpIntQy *= mesh->cells[i].volume / 8.;
                tmpIntQz *= mesh->cells[i].volume / 8.;

                //  std::cout << "From MethodFemDgHeat.cpp: calcGradients is continued cycle 2!" << std::endl;

                //  std::cout << "!!! " << intQx[i][j] << std::endl;

                //  UPDATE ON 13.07.2023 - проверяем величину tmpIntQx
                //  std::cout << "tmpIntQx = " << tmpIntQx << std::endl;

                intQx[i][j] -= tmpIntQx;
                intQy[i][j] -= tmpIntQy;

                intQz[i][j] -= tmpIntQz;

                //  std::cout << "From MethodFemDgHeat.cpp: calcGradients is continued cycle 2.1!" << std::endl;

                //  std::cout << "intQx["<<i<<"]["<<j<<"] = " << intQx[i][j] << std::endl;

                //	UPDATE от 28.12.2022 - попытка отловить, в каком месте кода возникает ошибка в вычислениях

                /*
                if (intQx[i][j]!=0){
                    std::cout<<"intQx["<<i<<"]["<<j<<"] = "<<intQx[i][j]<<std::endl;
                }

                if (intQy[i][j]!=0){
                    std::cout<<"intQy["<<i<<"]["<<j<<"] = "<<intQy[i][j]<<std::endl;
                }

                if (intQz[i][j]!=0){
                    std::cout<<"intQz["<<i<<"]["<<j<<"] = "<<intQz[i][j]<<std::endl;
                }
                */
            }
        }

        //std::cout<<"From MethodFemDgHeat.cpp: calcGradients continued 0!"<<std::endl;


        // криволинейные интегралы (в нашем случае - поверхностные???)
        Index cCount = Config::getCompCount();

        //std::cout<<"From MethodFemDgHeat.cpp: calcGradients continued 1!"<<std::endl;


        for (int i = 0; i < mesh->faces.size(); i++) {


            //  std::cout << "From MethodFemDgHeat.cpp: calcGradients is continued cycle 3!" << std::endl;

            Face &f = mesh->faces[i];
            Index c1 = f.cells[0];
            Index c2;
            //Prim p1 = data[c1].getPrim();
            //Prim p2(cCount);

            bool isBnd = f.cells.size() == 1;

            Index cCount = Config::getCompCount();

            //std::cout<<"From MethodFemDgHeat.cpp: calcGradients continued 1 cycle 0!"<<std::endl;

            //  std::cout << "From MethodFemDgHeat.cpp: calcGradients is continued cycle 4!" << std::endl;

            for (int j = 0; j < BASE_FUNC_COUNT; j++) {

                double tmpIntQx1 = 0.0, tmpIntQx2 = 0.0, tmpIntQy1 = 0.0, tmpIntQy2 = 0.0, tmpIntQz1 = 0.0, tmpIntQz2 = 0.0;
                //  UPDATE ON 12.07.2023 - немного меняю синтаксис
                //  for (int iGP = 0; iGP < GP_EDGE_COUNT; iGP++) {
                for (int iGP = 0; iGP < f.gp.size(); iGP++) {
                    //	Point &pt = edgeGP[i][iGP];

                    //	UPDATE 21.12.22 - попробовать разобраться с вычислением EDGE_GP
                    //	ЛИБО по коду из nummeth 2019, ЛИБО по коду из charm_3d_v2

                    //	Непонятно, как соотносятся edgeGP и f.gp
                    //	Возможно, стоит всё-таки реализовать функцию, вычисляющую
                    //	EDGE_GP и т.д.!

                    //	Point &pt = edgeGP[i][iGP];

                    Point &pt = f.gp[iGP];

                    //std::cout<<"From MethodFemDgHeat.cpp: calcGradients continued 1 cycle 0 cycle 0!"<<std::endl;

                    //std::cout<<"From MethodFemDgHeat.cpp: calcGradients continued 1 cycle 0 cycle 0!"<<std::endl;

                    //  UPDATE ON 12.07.2023 - меняю Prim на PrimHeat и getPrim на getFields 
                    //  Prim p1 = data[c1].getPrim();

                    //  UPDATE ON 13.07.2023 - попробую пока вообще убрать этот фрагмент...
                    //  Это действие может существенно повлиять на работу программы...
                    //  PrimHeat p1 = data[c1].getFields();

                    PrimHeat p1;


                    //  Prim p2(cCount);
                    //  PrimHeat p2();
                    PrimHeat p2;
                    convertToParam(c1, pt, p1);

                    //std::cout<<"From MethodFemDgHeat.cpp: calcGradients continued 1 cycle 0 cycle 1!"<<std::endl;

                    //	UPDATE от 23.12.2022 - интеграция с MethodFvm.cpp

                    //	Какое значение принимает c2, если есть вторая ячейка???
                    if (isBnd) {

                        //std::cout<<i<<": isBnd: From MethodFemDgHeat.cpp: calcGradients continued 1 cycle 0 cycle 1!"<<std::endl;

                        //  UPDATE ON 13.07.2023 - создала специальную функцию calcHeat
                        //  f.bnd->calc(p1, p2, f.n);
                        f.bnd->calcHeat(p1, p2, f.n);

                        //  UPDATE ON 15.07.2023 - пробую занулять температуру на границах вручную...
                        
                        p2.t = 0.;
                        //  p2.qx = 0.;
                        //  p2.qy = 0.;
                        //  p2.qz = 0.;

                    } else {

                        //std::cout<<i<<": Not isBnd: From MethodFemDgHeat.cpp: calcGradients continued 1 cycle 0 cycle 1!"<<std::endl;


                        c2 = f.cells[1];

                        //	UPDATE от 08.01.2023 - пока убираем определение p2 через getPrim()...

                        //p2 = data[c2].getPrim();
                        convertToParam(c2, pt, p2);
                    }

                    //std::cout<<"From MethodFemDgHeat.cpp: calcGradients continued 1 cycle 0 cycle 2!"<<std::endl;

                    double fT, fqx, fqy, fqz;
                    flux(p1, p2, f.n, fT, fqx, fqy, fqz);

                    //	правильно ли обращаюсь к f.gw???
                    tmpIntQx1 += f.n.x * fT * getF(j, c1, pt) * f.gw[iGP];
                    tmpIntQy1 += f.n.y * fT * getF(j, c1, pt) * f.gw[iGP];

                    tmpIntQz1 += f.n.z * fT * getF(j, c1, pt) * f.gw[iGP];

                    if (!isBnd) {
                        tmpIntQx2 += f.n.x * fT * getF(j, c2, pt) * f.gw[iGP];
                        tmpIntQy2 += f.n.y * fT * getF(j, c2, pt) * f.gw[iGP];

                        tmpIntQz2 += f.n.z * fT * getF(j, c2, pt) * f.gw[iGP];
                    }
                }


                //	правильно ли заменила edgeJ???


                //std::cout<<"f.area = "<<f.area<<"; edgeJ = "<<(mesh->cells[i].dh[0]*mesh->cells[i].dh[1]) / 4.<<std::endl;


                //	UPDATE от 28.12.2022 - пробуем edgeJ ~ f.area/4.
                //	БЫЛО: tmpIntQx1 *= f.area;

                tmpIntQx1 *= f.area / 4.;
                tmpIntQy1 *= f.area / 4.;

                tmpIntQz1 *= f.area / 4.;

                tmpIntQx2 *= f.area / 4.;
                tmpIntQy2 *= f.area / 4.;

                tmpIntQz2 *= f.area / 4.;


                intQx[c1][j] += tmpIntQx1;
                intQy[c1][j] += tmpIntQy1;

                intQz[c1][j] += tmpIntQz1;

                if (!isBnd) {
                    intQx[c2][j] -= tmpIntQx2;
                    intQy[c2][j] -= tmpIntQy2;

                    intQz[c2][j] -= tmpIntQz2;
                }

                //  std::cout << "! intQx["<<c1<<"]["<<j<<"] = " << intQx[c1][j] << std::endl;

                /*
                if (intQx[c1][j]!=0)
                {
                    std::cout<<"intQx["<<c1<<"]["<<j<<"] = "<<intQx[c1][j]<<std::endl;
                }
                */

            }

        }

        //std::cout<<"From MethodFemDgHeat.cpp: calcGradients continued 2!"<<std::endl;

        //вычисляем градиенты на новом шаге по времени
        for (int i = 0; i < mesh->cells.size(); i++) {
            for (int j = 0; j < BASE_FUNC_COUNT; j++) {
                double tmpQx = 0.0, tmpQy = 0.0, tmpQz = 0.0;
                for (int z = 0; z < BASE_FUNC_COUNT; z++) {

                    //  UPDATE ON 12.07.2023 - заполняется ли до этого в коде матрица invA???

                    //  UPDATE ON 14.07.2023 - В КАКОМ МЕСТЕ ПРОГРАММЫ ЗАПОЛНЯЕТСЯ МАТРИЦА invA???
                    //  ВОЗМОЖНО, ОШИБКА В НЕЙ... ЕСТЬ КАК ОЧЕНЬ БОЛЬШИЕ, ТАК И ОЧЕНЬ МАЛЕНЬКИЕ ЗНАЧЕНИЯ...

                    tmpQx += invA[i][j][z] * intQx[i][z];
                    tmpQy += invA[i][j][z] * intQy[i][z];

                    tmpQz += invA[i][j][z] * intQz[i][z];

                    //  UPDATE ON 13.07.2023 - лучше проверить, чем заполнена матрица invA...
                    //  std::cout<<"invA["<<i<<"]["<<j<<"]["<<z<<"] = "<<invA[i][j][z]<<std::endl;

                    //  std::cout<<"intQx["<<i<<"]["<<z<<"] = "<<intQx[i][z]<<std::endl;

                    /*
                    if (invA[i][j][z]!=0)
                    {
                        std::cout<<"invA["<<i<<"]["<<j<<"]["<<z<<"] = "<<invA[i][j][z]<<std::endl;
                    }
                    */

                }
                qx[i][j] = tmpQx;
                qy[i][j] = tmpQy;

                qz[i][j] = tmpQz;

                //  UPDATE ON 13.07.2023 - пробую вывести qx[i][j], чтобы определить место, где программа начинает выдавать слишком большие числа...
                //  std::cout<<"qx["<<i<<"]["<<j<<"] = "<<qx[i][j]<<std::endl;

                //  UPDATE ON 14.07.2023 - ЕСТЬ СОМНЕНИЯ НАСЧЁТ ВТОРОГО КОМПОНЕНТА qx...
                //  !!!

                //  ИТОГ ОТ 14.07.2023: ОПРЕДЕЛИТЬ, КАКОЙ ИМЕННО ФРАГМЕНТ МЕТОДА ВЫЗЫВАЕТ БЫСТРЫЙ РОСТ ЗНАЧЕНИЯ ОШИБКИ!!!


                //	UPDATE от 28.12.2022 - скорее всего, первая из крупных вычислительных ошибок - в этом блоке!!!
                //	Может быть связано с матрицей invA!!!

                /*
                if (qx[i][j]!=0)
                {
                    std::cout<<"qx["<<i<<"]["<<j<<"] = "<<qx[i][j]<<std::endl;
                }
                */

            }
        }
    }


    void MethodFemDgHeat::calcDiffusionVol() {
        for (int i = 0; i < mesh->cells.size(); i++) {
            for (int j = 0; j < BASE_FUNC_COUNT; j++) {
                double tmpIntT = 0.0;
                //  UPDATE ON 12.07.2023 - меняю обозначения в соответствии с кодом
                //  for (int z = 0; z < GP_CELL_COUNT; z++) {
                for (int z = 0; z < mesh->cells[i].gp.size(); z++) {
                    tmpIntT +=
                            mesh->cells[i].gw[z] * getQx(i, mesh->cells[i].gp[z]) * getDfDx(j, i, mesh->cells[i].gp[z]);
                    tmpIntT +=
                            mesh->cells[i].gw[z] * getQy(i, mesh->cells[i].gp[z]) * getDfDy(j, i, mesh->cells[i].gp[z]);

                    tmpIntT +=
                            mesh->cells[i].gw[z] * getQz(i, mesh->cells[i].gp[z]) * getDfDz(j, i, mesh->cells[i].gp[z]);
                }

                //  UPDATE ON 16.07.2023 - меняю аналог cellJ
                //  tmpIntT *= (mesh->cells[i].dh[0] * mesh->cells[i].dh[1] * mesh->cells[i].dh[2]) / 8.;
                tmpIntT *= mesh->cells[i].volume / 8.;

                intT[i][j] -= tmpIntT;

                /*
                if (intT[i][j]!=0)
                {
                    std::cout<<"intT["<<i<<"]["<<j<<"] = "<<intT[i][j]<<std::endl;
                }
                */

                //  std::cout<<"intT["<<i<<"]["<<j<<"] = "<<intT[i][j]<<std::endl;

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
                //  UPDATE ON 12.07.2023 - меняю синтаксис
                //  for (int iGP = 0; iGP < GP_EDGE_COUNT; iGP++) {
                for (int iGP = 0; iGP < f.gp.size(); iGP++) {
                    Point &pt = f.gp[iGP];

                    //	UPDATE от 25.12.2022 - меняем Param на Prim

                    //  UPDATE ON 12.07.2023 - меняю Prim на PrimHeat
                    //  Prim p1 = data[c1].getPrim();

                    //  UPDATE ON 13.07.2023 - оставляю просто инициализацию переменной класса PrimHeat
                    //  PrimHeat p1 = data[c1].getPrim();
                    PrimHeat p1;

                    //  UPDATE ON 12.07.2023 - пока нам не нужно количество компонентов
                    //  Prim p2(cCount);
                    PrimHeat p2;

                    convertToParam(c1, pt, p1);
                    if (isBnd) {
                        //  UPDATE ON 13.07.2023 - создала специальную функцию calcHeat
                        //  f.bnd->calc(p1, p2, f.n);
                        f.bnd->calcHeat(p1, p2, f.n);

                        //  UPDATE ON 15.07.2023 - пробую занулять температуру на границах вручную...
                        p2.t = 0.;
                        //  p2.qx = 0.;
                        //  p2.qy = 0.;
                        //  p2.qz = 0.;

                    } else {
                        c2 = f.cells[1];
                        convertToParam(c2, pt, p2);
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

                //	UPDATE от 28.12.2022 - пробуем edgeJ ~ f.area/4.
                //	БЫЛО: tmpIntT1 *= f.area;

                tmpIntT1 *= f.area / 4.;

                tmpIntT2 *= f.area / 4.;

                intT[c1][j] += tmpIntT1;

                if (!isBnd) {
                    intT[c2][j] -= tmpIntT2;
                }

                /*
                if (intT[c1][j]!=0)
                {
                    std::cout<<"intT["<<c1<<"]["<<j<<"] = "<<intT[c1][j]<<std::endl;
                }
                */

                //  std::cout<<"intT["<<c1<<"]["<<j<<"] = "<<intT[c1][j]<<std::endl;

            }

        }
    }


    void MethodFemDgHeat::calcNewValues() {

        //	UPDATE от 27.12.2022

        Index cCount = Config::getCompCount();


        Real dt = calcDt();


        for (int i = 0; i < mesh->cells.size(); i++) {

            //  UPDATE ON 13.07.2023 - меняю Prim на PrimHeat
            //  Prim p(cCount);
            PrimHeat p;

            for (int j = 0; j < BASE_FUNC_COUNT; j++) {
                double tmpT = 0.0;
                for (int z = 0; z < BASE_FUNC_COUNT; z++) {
                    tmpT += invA[i][j][z] * intT[i][z];
                }
                //T[i][j] += TAU*tmpT;

                T[i][j] += dt * tmpT;

                //	UPDATE от 29.12.2022 - пока убираем вспомогательный вывод...

                /*
                if (i<1000){

                    std::cout<<"cell "<<i<<": tmpT = "<<tmpT<<std::endl;

                    std::cout<<"cell "<<i<<": TAU = "<<TAU<<std::endl;


                    std::cout<<"cell "<<i<<": func "<<j<<": T = "<<T[i][j]<<std::endl;
                }
                */

                //  std::cout<<"T["<<i<<"]["<<j<<"] = "<<T[i][j]<<std::endl;

            }


            Cell &cell = mesh->cells[i];


            //	UPDATE от 27.12.2022 - убираем обновление data

            //	UPDATE от 07.01.2023 - пробуем обновлять данные с помощью convertToParam()

            convertToParam(i, cell.center, p);

            //p.t = getT(i, mesh->cells[i].center);

            //p.qx = getQx(i, mesh->cells[i].center);
            //p.qy = getQy(i, mesh->cells[i].center);
            //p.qz = getQz(i, mesh->cells[i].center);

            //data[i].setCons(p);
        }
    }


    // UPDATE от 07.01.2023 - переделываем функцию запуска расчёта
    void MethodFemDgHeat::run() {

        //  UPDATE ON 13.07.2023 - при запуске возникает ошибка Segmentation fault (core dumped)
        //  Пробую добавить текстовые метки в лог, чтобы отследить конкретное место, где возникает ошибка

        std::cout << "From MethodFemDgHeat.cpp: run is started!" << std::endl;

        while (true) {
            Index cCount = Config::getCompCount();

            //  std::cout << "From MethodFemDgHeat.cpp: run is continued cycle 1!" << std::endl;

            //  UPDATE ON 13.07.2023 - возможно, ошибку вызывает обнуление массивов...
            //  Попробую немного поменять синтаксис            
            //  for (int i = 0; i < mesh->cells.size(); i++) {
            //      memset(intT[i], 0, sizeof(double) * BASE_FUNC_COUNT);
            //      memset(intQx[i], 0, sizeof(double) * BASE_FUNC_COUNT);
            //      memset(intQy[i], 0, sizeof(double) * BASE_FUNC_COUNT);
            //      memset(intQz[i], 0, sizeof(double) * BASE_FUNC_COUNT);
            //  }

            //  memset(intT, 0, sizeof intT);
            //  memset(intQx, 0, sizeof intQx);
            //  memset(intQy, 0, sizeof intQy);
            //  memset(intQz, 0, sizeof intQz);

            for (int i = 0; i < mesh->cells.size(); i++) {
                for (int j = 0; j < BASE_FUNC_COUNT; j++) {
                    intT[i][j] = 0.;
                    intQx[i][j] = 0.;
                    intQy[i][j] = 0.;
                    intQz[i][j] = 0.;
                }
            }

            //std::cout<<"From MethodFemDgHeat.cpp: run continued 2!"<<std::endl;

            //  std::cout << "From MethodFemDgHeat.cpp: run is continued cycle 2!" << std::endl;

            calcGradients();


            //  UPDATE ON 14.07.2023 - УБРАТЬ ЭТОТ break!!!
            //  break;



            //std::cout<<"From MethodFemDgHeat.cpp: run continued 3!"<<std::endl;

            //  std::cout << "From MethodFemDgHeat.cpp: run is continued cycle 3!" << std::endl;

            calcDiffusionVol();
            calcDiffusionSurf();

            calcNewValues();

            Real dt = calcDt();

            conf->t += conf->dt;
            conf->timestep++;

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

    //  UPDATE ON 12.07.2023 - пока оставляем функцию для расчёта времени такой, нужно подумать, как рассчитать число Куранта!!!
    Real MethodFemDgHeat::calcDt() {
        Config *conf = Config::get();
        Real dt = conf->dt;

        dt = conf->dt;

        return dt;
    }





    //	Обновление от 20.12.22

    /*
    //	Функция запуска расчёта
    void MethodFemDgHeat::run() {

        //	UPDATE от 26.12.2022 - добавление доп.меток

        std::cout<<"From MethodFemDgHeat.cpp: run started!"<<std::endl;

        double t = 0.0;
            int step = 0;

        std::cout<<"From MethodFemDgHeat.cpp: run continued 0!"<<std::endl;

        //	UPDATE от 27.12.2022 - добавляю config

        Config *conf = Config::get();

            while (t < TMAX) {
                t += TAU;
                step++;

            std::cout<<"From MethodFemDgHeat.cpp: run continued 1!"<<std::endl;

                for(int i = 0; i < mesh->cells.size(); i++){
                        memset(intT[i], 0, sizeof(double)*BASE_FUNC_COUNT);
                        memset(intQx[i], 0, sizeof(double)*BASE_FUNC_COUNT);
                        memset(intQy[i], 0, sizeof(double)*BASE_FUNC_COUNT);
                memset(intQz[i], 0, sizeof(double)*BASE_FUNC_COUNT);
                }

            std::cout<<"From MethodFemDgHeat.cpp: run continued 2!"<<std::endl;

                calcGradients();

            std::cout<<"From MethodFemDgHeat.cpp: run continued 3!"<<std::endl;

                calcDiffusionVol();
                calcDiffusionSurf();

                calcNewValues();

            //	UPDATE от 26.12.2022 - исправляю saveVtk на save

            //	Возможно, для сохранения файла нужно другое число шагов

            //	UPDATE от 27.12.2022 - по-другому вывожу количество шагов

            std::cout << "STEP: " << step << std::endl;


            //	UPDATE от 02.01.2023 - сохраняем результаты каждые 100 шагов

                if (step % 100 == 0)
            //if (step % 10 == 0)
                {
                        save(step);
                        printf("Calculation results for step %d are saved.\n", step);
                }
            }
    }
    */


    double MethodFemDgHeat::getQx(int iCell, Point pt) {
        double d = 0.0;
        for (int i = 0; i < BASE_FUNC_COUNT; i++) {
            d += qx[iCell][i] * getF(i, iCell, pt);
        }
        return d;
    }

    double MethodFemDgHeat::getQy(int iCell, Point pt) {
        double d = 0.0;
        for (int i = 0; i < BASE_FUNC_COUNT; i++) {
            d += qy[iCell][i] * getF(i, iCell, pt);
        }
        return d;
    }

    double MethodFemDgHeat::getQz(int iCell, Point pt) {
        double d = 0.0;
        for (int i = 0; i < BASE_FUNC_COUNT; i++) {
            d += qz[iCell][i] * getF(i, iCell, pt);
        }
        return d;
    }


    //	Не факт, что нужна отдельная функция для граничных условий
    /*
    void MethodFemDgHeat::bnd(Face *f, Point pt, Param p1, Param &p2) {
            switch (f.type) {

            //	UPDATE от 23.12.2022 - добавить свои граничные условия

                case 1: // тепловой поток = 0
                        p2.t = 0.0;
                        p2.qx = p1.qx;
                        p2.qy = p1.qy;
                        break;
                case 2: // постоянное значение
                        p2.t = 1.0;
                        p2.qx = p1.qx;
                        p2.qy = p1.qy;
                        break;
            }
    }
    */

    //	UPDATE от 07.01.2023 - так ли рассчитывается поток в разрывном методе Галёркина???

    //  UPDATE ON 12.07.2023 - меняю Vector на Point
    //  void MethodFemDgHeat::flux(Prim pl, Prim pr, Vector n, double &fT, double &fqx, double &fqy, double &fqz) {
    void MethodFemDgHeat::flux(PrimHeat pl, PrimHeat pr, Point n, double &fT, double &fqx, double &fqy, double &fqz) {
        fT = 0.5 * (pl.t + pr.t);
        fqx = 0.5 * (pl.qx + pr.qx) + C11 * (pr.t - pl.t) * n.x;
        fqy = 0.5 * (pl.qy + pr.qy) + C11 * (pr.t - pl.t) * n.y;

        fqz = 0.5 * (pl.qz + pr.qz) + C11 * (pr.t - pl.t) * n.z;
    }

    //	UPDATE от 26.12.2022 - переопределяю функции done, save, saveVtk!!!

    void MethodFemDgHeat::done() {

    }


    //	UPDATE от 07.01.2023 - переделываем функцию записи в файл
    void MethodFemDgHeat::save() {
        if (conf->timestep % conf->writePeriod == 0) {
            saveVtk();
        }
    }


    /*
    void MethodFemDgHeat::save(int step) {
            //if (conf->timestep % conf->writePeriod == 0) {

        //	UPDATE от 27.12.2022
        if (step % 100 == 0) {
                    saveVtk(step);
            }
        }
    */

    //	UPDATE от 27.12.2022 - другая версия функции saveVtk (из nummeth2019)
    //	UPDATE от 07.01.2023 - немного меняем функцию записи в файл


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

        //	UPDATE от 27.12.2022

        //ss << "charm_" << std::setfill('0') << std::setw(8) << step << ".vtk";

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


        // Плотность
        fout << "CELL_DATA " << cellsCount << std::endl;

        /*

            fout << std::endl << "SCALARS Density double 1" << std::endl << "LOOKUP_TABLE default" << std::endl;
            for (int i = 0; i < cellsCount; i++)
            {
            Prim p(cCount);

                    convertToParam(i, mesh->cells[i].center, p);
                    fout << p.r << " ";
                    if (i % 8 == 0  ||  i == cellsCount) {
                        fout << std::endl;
                    }
            }

            // Давление
            fout << std::endl << "SCALARS Pressure double 1" << std::endl << "LOOKUP_TABLE default" << std::endl;
            for (int i = 0; i < cellsCount; i++)
            {
                    Prim p(cCount);

                    convertToParam(i, mesh->cells[i].center, p);
                    fout << p.p << " ";
                    if (i % 8 == 0  ||  i == cellsCount) {
                        fout << std::endl;
                    }
            }
        */

        // Температура
        fout << std::endl << "SCALARS Temperature double 1" << std::endl << "LOOKUP_TABLE default" << std::endl;
        for (int i = 0; i < cellsCount; i++) {

            //	UPDATE от 28.12.2022 - меняю вывод температуры в vtk

            //Prim p(cCount);

            //convertToParam(i, mesh->cells[i].center, p);
            //fout << p.t << " ";

            fout << getT(i, mesh->cells[i].center) << " ";

            if (i % 8 == 0 || i == cellsCount) {
                fout << std::endl;
            }
        }

        /*
            // Полная энергия
            fout << std::endl << "SCALARS TotalEnergy double 1" << std::endl << "LOOKUP_TABLE default" << std::endl;
            for (int i = 0; i < cellsCount; i++)
            {
                    Prim p(cCount);

                    convertToParam(i, mesh->cells[i].center, p);
                    fout << p.eTot << " ";
                    if (i % 8 == 0  ||  i == cellsCount) {
                        fout << std::endl;
                    }
            }
        */

        //	UPDATE от 29.12.2022 - также убираю вывод числа Маха в файл (чтобы не было nan)


        /*
            // Число Маха
            fout << std::endl << "SCALARS Mach double 1" << std::endl << "LOOKUP_TABLE default" << std::endl;
            for (int i = 0; i < cellsCount; i++)
            {
                    Prim p(cCount);

                    convertToParam(i, mesh->cells[i].center, p);
                    fout << ::sqrt(p.v2())/p.cz << " ";
                    if (i % 8 == 0  ||  i == cellsCount) {
                        fout << std::endl;
                    }
            }
        */

        /*
            // Вектор скорости
            fout << std::endl << "VECTORS Velosity double" << std::endl;
            for (int i = 0; i < cellsCount; i++)
            {
                    Prim p(cCount);

                    convertToParam(i, mesh->cells[i].center, p);
                    fout << p.v.x << " " << p.v.y << " " << p.v.z << " ";
                    if (i % 8 == 0  ||  i == cellsCount) {
                        fout << std::endl;
                    }
            }
        */

        //	UPDATE от 29.12.2022 - пока убираю полное давление, чтобы в vtk-файлах не было nan...
        /*
            // Полное давление
            fout << std::endl << "SCALARS TotalPressure double 1" << std::endl << "LOOKUP_TABLE default" << std::endl;
            for (int i = 0; i < cellsCount; i++)
            {
                    Prim p(cCount);

                    convertToParam(i, mesh->cells[i].center, p);
                    fout << p.p * ::pow( 1.0 + 0.5 * ( p.gam - 1.0 ) * p.v2() / ( p.cz*p.cz ) , p.gam / ( p.gam - 1.0 ) ) << " ";
                    if (i % 8 == 0  ||  i == cellsCount) {
                        fout << std::endl;
                    }
            }
        */

        //	UPDATE от 02.01.2023 - пока убираю полную температуру, чтобы не было nan...
        /*
            // Полная температура
            fout << std::endl << "SCALARS TotalTemperature double 1" << std::endl << "LOOKUP_TABLE default" << std::endl;
            for (int i = 0; i < cellsCount; i++)
            {
                    Prim p(cCount);

                    convertToParam(i, mesh->cells[i].center, p);
                    fout << p.t + 0.5 * p.v2() / p.cp << " ";
                    if (i % 8 == 0  ||  i == cellsCount) {
                        fout << std::endl;
                    }
            }
        */

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


    /*
    void MethodFemDgHeat::saveVtk(int step) // @todo:
        {
            Index cCount = Config::getCompCount();
            Prim p(cCount);
            Material *mat;
            String fileName;
            std::ofstream fout;

            Index nodesCount = mesh->nodes.size();
            Index cellsCount = mesh->cells.size();

            std::stringstream ss;
            //ss << "charm_" << std::setfill('0') << std::setw(8) << conf->timestep << ".vtk";

        //	UPDATE от 27.12.2022

        ss << "charm_" << std::setfill('0') << std::setw(8) << step << ".vtk";

            fileName = ss.str();

            std::cout <<" Writing VTK file '" << fileName << "'..." << std::endl;

            fout.open(fileName, std::ios_base::out);
            fout << "# vtk DataFile Version 3.0" << std::endl;
            fout << "3D RKDG method results for task '%s'." << std::endl;
            fout << "ASCII" << std::endl;
            fout << "DATASET UNSTRUCTURED_GRID" << std::endl;



            fout << "POINTS " << nodesCount << " double" << std::endl;

            for (const auto &node: mesh->nodes) {
                    fout << node.x << " " << node.y << " " << node.z << std::endl;
            }

            fout << std::endl << "CELLS " << cellsCount << " " << 9*cellsCount << std::endl;

            for (const auto &cell: mesh->cells) {

                    fout << cell.nodesInd.size();
                    for (auto nIdx: cell.nodesInd) {
                        fout << " " << nIdx;
                    }
                    fout << std::endl;
            }

            fout << std::endl << "CELL_TYPES " << cellsCount << std::endl;

            for (int i = 1; i <= cellsCount; i++)
            {
                    fout << "11 ";
                    if (i % 20 == 0 ) {
                        fout << std::endl;
                    }
            }
            fout << std::endl;


            // Плотность
            fout << "CELL_DATA " << cellsCount << std::endl;


            fout << std::endl << "SCALARS Density double 1" << std::endl << "LOOKUP_TABLE default" << std::endl;
            for (int i = 0; i < cellsCount; i++)
            {
                    Prim prim = data[i].getPrim();
                    fout << prim.r << " ";
                    if (i % 8 == 0  ||  i == cellsCount) {
                        fout << std::endl;
                    }
            }



            // Давление
            fout << std::endl << "SCALARS Pressure double 1" << std::endl << "LOOKUP_TABLE default" << std::endl;
            for (int i = 0; i < cellsCount; i++)
            {
                    Prim prim = data[i].getPrim();
                    fout << prim.p << " ";
                    if (i % 8 == 0  ||  i == cellsCount) {
                        fout << std::endl;
                    }
            }


            // Температура
            fout << std::endl << "SCALARS Temperature double 1" << std::endl << "LOOKUP_TABLE default" << std::endl;
            for (int i = 0; i < cellsCount; i++)
            {
                    Prim prim = data[i].getPrim();
                    fout << prim.r << " ";
                    if (i % 8 == 0  ||  i == cellsCount) {
                        fout << std::endl;
                    }
            }


            // Полная энергия
            fout << std::endl << "SCALARS TotalEnergy double 1" << std::endl << "LOOKUP_TABLE default" << std::endl;
            for (int i = 0; i < cellsCount; i++)
            {
                    Prim prim = data[i].getPrim();
                    fout << prim.eTot << " ";
                    if (i % 8 == 0  ||  i == cellsCount) {
                        fout << std::endl;
                    }
            }




            // Число Маха
            fout << std::endl << "SCALARS Mach double 1" << std::endl << "LOOKUP_TABLE default" << std::endl;
            for (int i = 0; i < cellsCount; i++)
            {
                    Prim prim = data[i].getPrim();
                    fout << ::sqrt(prim.v2())/prim.cz << " ";
                    if (i % 8 == 0  ||  i == cellsCount) {
                        fout << std::endl;
                    }
            }



            // Вектор скорости
            fout << std::endl << "VECTORS Velosity double" << std::endl;
            for (int i = 0; i < cellsCount; i++)
            {
                    Prim prim = data[i].getPrim();
                    fout << prim.v.x << " " << prim.v.y << " " << prim.v.z << " ";
                    if (i % 8 == 0  ||  i == cellsCount) {
                        fout << std::endl;
                    }
            }



            // Полное давление

            fout << std::endl << "SCALARS TotalPressure double 1" << std::endl << "LOOKUP_TABLE default" << std::endl;
            for (int i = 0; i < cellsCount; i++)
            {
                    Prim prim = data[i].getPrim();
                    fout << prim.p * ::pow( 1.0 + 0.5 * ( prim.gam - 1.0 ) * prim.v2() / ( prim.cz*prim.cz ) , prim.gam / ( prim.gam - 1.0 ) ) << " ";
                    if (i % 8 == 0  ||  i == cellsCount) {
                        fout << std::endl;
                    }
            }


            // Полная температура
            fout << std::endl << "SCALARS TotalTemperature double 1" << std::endl << "LOOKUP_TABLE default" << std::endl;
            for (int i = 0; i < cellsCount; i++)
            {
                    Prim prim = data[i].getPrim();
                    fout << prim.t + 0.5 * prim.v2() / prim.cp << " ";
                    if (i % 8 == 0  ||  i == cellsCount) {
                        fout << std::endl;
                    }
            }

            // Полный объём
            fout << std::endl << "SCALARS Volume double 1" << std::endl << "LOOKUP_TABLE default" << std::endl;
            for (int i = 0; i < cellsCount; i++)
            {
                    fout << mesh->cells[i].volume << " ";
                    if (i % 8 == 0  ||  i == cellsCount) {
                        fout << std::endl;
                    }
            }

            // id ячеек
            fout << std::endl << "SCALARS CellId double 1" << std::endl << "LOOKUP_TABLE default" << std::endl;
            for (int i = 0; i < cellsCount; i++)
            {
                    fout << i << " ";
                    if (i % 8 == 0  ||  i == cellsCount) {
                        fout << std::endl;
                    }
            }

            fout.close();
            std::cout << "File '" << fileName << "' is saved." << std::endl;
        }
    */




}