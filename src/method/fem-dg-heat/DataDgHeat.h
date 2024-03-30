//
// Created by zhrv on 17.02.2023.
//

//  UPDATE ON 04.07.2023 - новый класс для данных (температура + тепловые потоки) для решения задач теплопроводности

#ifndef CHARM_3D_V2_DATADGHEAT_H
#define CHARM_3D_V2_DATADGHEAT_H

#include <Point.h>
#include "global.h"
#include "Material.h"
#include <cmath>
#include <method/Data.h>
#include <Cell.h>
//  UPDATE ON 04.07.2023 - пока убираю mpi.h, т.к. параллельность не обязательна...
//  #include "mpi.h"

//  ---18.01.24---
// Для избежания ошибок переполнения переделываю примитивные и консервативные переменные в РМГ по аналогии с fvm!!!

namespace charm {

    class PrimHeat
    {
    public:
        Real        t; 
        Real        qx;
        Real        qy;
        Real        qz;

        // Вспомогательная переменная (для корректного определения конструктора в getVar)...
        Real helpVar;

        // PrimHeat() {}

        PrimHeat(Index compCount): helpVar(0.) {}

        PrimHeat(const PrimHeat& VarHeat) { *this = VarHeat; }

        //  UPDATE ON 04.07.2023 - реализация операторов присвоения значения, сложения, вычитания и умножения на число примитивных переменных
        PrimHeat& operator  = (const PrimHeat& var);
        PrimHeat& operator  = (const Real& a);
        PrimHeat& operator += (const PrimHeat& var);
        PrimHeat& operator -= (const PrimHeat& var);
        PrimHeat& operator *= (const Real& a);

        //  UPDATE ON 04.07.2023 - нормализация примитивных переменных (присвоение 0 переменным со значением меньше EPS)
        void normalize();

        //  UPDATE ON 04.07.2023 - функция, проверяющая, принимает ли хоть одна переменная значение NaN
        inline bool isNaN() {
            bool res = false;
            res |= std::isnan(t);
            res |= std::isnan(qx);
            res |= std::isnan(qy);
            res |= std::isnan(qz);
            return res;
        }

        inline Index size() const { return sizeof(Real)*4; }
    };


    //  Консервативные переменные
    class HeatDgFields
    {
    public:
        ArrayReal        t;
        ArrayReal        qx;
        ArrayReal        qy;
        ArrayReal        qz;

        ArrayReal       intT;
        ArrayReal       intQx;
        ArrayReal       intQy;
        ArrayReal       intQz;


        explicit HeatDgFields(Index bfCount): t(bfCount), qx(bfCount), qy(bfCount), qz(bfCount), intT(bfCount), intQx(bfCount), intQy(bfCount), intQz(bfCount) {}

        HeatDgFields(const HeatDgFields& cons) { *this = cons; }

        HeatDgFields& operator  = (const HeatDgFields& cons);
        HeatDgFields& operator  = (const Real& a);
        HeatDgFields& operator += (const HeatDgFields& cons);
        HeatDgFields& operator -= (const HeatDgFields& cons);
        HeatDgFields& operator *= (const Real& a);

        void normalize();

        inline Index size() const { return sizeof(Real)*t.size()*4; }

    };


    class DataDgHeat
    {
    public:
        HeatDgFields    flds;
        Cell           *cell;

        PrimHeat        p; 

        explicit DataDgHeat(Index bfCount): flds(bfCount), p(bfCount) {}

        void getVar(PrimHeat &p);

        PrimHeat getVar();

        Real getT(Point pt);
        Vector getGradT(Point pt);

        Real getTAvg();
        Vector getGradTAvg();

        //  UPDATE ON 06.07.2023 - получение консервативных переменных
        HeatDgFields& getFields();
        //  UPDATE ON 06.07.2023 - заполнение примитивных переменных консервативными
        void setFields(const PrimHeat &p);

        //  UPDATE ON 05.07.2023 - внесение температуры и тепловых потоков (из консервативных переменных) в переменную-буфер
        void getBuffer(Byte *);
        //  UPDATE ON 06.07.2023 - получение температуры и тепловых потоков (из консервативных переменных) из переменной-буфера
        void setBuffer(Byte *);

        Index size() { return flds.size(); }

        //  UPDATE ON 06.07.2023 - функции для получения количества элементов, названий и значений скалярных и векторных консервативных переменных
        Index getScalarFieldsCount();
        String getScalarFieldName(Index);
        Real getScalarFieldValue(Index);
        Index getVectorFieldsCount();
        String getVectorFieldName(Index);
        Vector getVectorFieldValue(Index);

    };



}
#endif //CHARM_3D_V2_DATADGHEAT_H