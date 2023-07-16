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


namespace charm {

    class PrimHeat
    {
    public:
        Real        t; 
        Real        qx;
        Real        qy;
        Real        qz;


        PrimHeat() {}


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


    class HeatDgFields
    {
    public:
        ArrayReal        t;
        ArrayReal        qx;
        ArrayReal        qy;
        ArrayReal        qz;


        explicit HeatDgFields(Index bfCount): t(bfCount), qx(bfCount), qy(bfCount), qz(bfCount) {}

        HeatDgFields(const HeatDgFields& cons) { *this = cons; }

        //  UPDATE ON 13.07.2023 - пока пробую убрать переопределение данных операторов, чтобы избежать ошибок...
        //  HeatDgFields& operator  = (const HeatDgFields& cons);
        //  HeatDgFields& operator  = (const Real& a);
        //  HeatDgFields& operator += (const HeatDgFields& cons);
        //  HeatDgFields& operator -= (const HeatDgFields& cons);
        //  HeatDgFields& operator *= (const Real& a);

        void normalize();

        inline Index size() const { return sizeof(Real)*t.size()*4; }

    };

    //  UPDATE ON 12.07.2023 - чтобы исправить ошибку, пробуем сделать так, чтобы класс DataDgHeat не наследовал класс Data

    //  class DataDgHeat : public Data {
    class DataDgHeat
    {
    public:
        HeatDgFields    flds;
        Cell           *cell;

        //  

        explicit DataDgHeat(Index bfCount): flds(bfCount) {}

        //  UPDATE ON 05.07.2023 - получение примитивных переменных из консервативных
        //  Лучше поменять название на getVar, как в cpp-файле...
        //  Пока параметр Point не применяем...
        //  void getPrim(Point pt, PrimHeat &p);
        //  PrimHeat getPrim(Point pt);

        //  UPDATE ON 13.07.2023 - пока комментирую функции getVar и setVar
        //  void getVar(PrimHeat &p);

        //  UPDATE ON 05.07.2023 - меняю название getPrim на getVar...
        //  PrimHeat getPrim();
        //  PrimHeat getVar();

        //  UPDATE ON 05.07.2023 - заполнение консервативных переменных примитивными
        //  (НЕ БЫЛО ЭТОЙ ФУНКЦИИ в DataDgHeat.h)
        //  void setVar(const PrimHeat &p);

        Real getT(Point pt);
        Vector getGradT(Point pt);

        Real getTAvg();
        Vector getGradTAvg();

        //  UPDATE ON 08.07.2023 - пробую переделать функции ниже по образцу из nummeth2019
        //  ИТОГ - оставляю, как было
        //  UPDATE ON 08.07.2023 - меняю double на Real
        //  UPDATE ON 12.07.2023 - возникает ошибка, добавляю индекс ячейки в аргументы функций ниже
        //  UPDATE ON 12.07.2023 - также пробую добавить в качестве аргумента Mesh mesh
        //  Real getF(int i, Int iCell, Point pt);
        //Real getF(int i, Int iCell, Point pt, Mesh mesh);
        
        //  Real getDfDx(int i, Int iCell, Point pt);
        //Real getDfDx(int i, Int iCell, Point pt, Mesh mesh);
        //  Real getDfDy(int i, Int iCell, Point pt);
        //Real getDfDy(int i, Int iCell, Point pt, Mesh mesh);
        //  Real getDfDz(int i, Int iCell, Point pt);
        //Real getDfDz(int i, Int iCell, Point pt, Mesh mesh);

        //  UPDATE ON 06.07.2023 - получение консервативных переменных
        HeatDgFields& getFields();
        //  UPDATE ON 06.07.2023 - заполнение примитивных переменных консервативными
        void setFields(const PrimHeat &p);

        //  UPDATE ON 12.07.2023 - убираю пометку override

        //  UPDATE ON 05.07.2023 - внесение температуры и тепловых потоков (из консервативных переменных) в переменную-буфер
        //  void getBuffer(Byte *) override;
        void getBuffer(Byte *);
        //  UPDATE ON 06.07.2023 - получение температуры и тепловых потоков (из консервативных переменных) из переменной-буфера
        //  void setBuffer(Byte *) override;
        void setBuffer(Byte *);
        //  Index size() override { return flds.size(); }
        Index size() { return flds.size(); }

        //  UPDATE ON 06.07.2023 - функции для получения количества элементов, названий и значений скалярных и векторных консервативных переменных
        //  Index getScalarFieldsCount() override;
        Index getScalarFieldsCount();
        //  String getScalarFieldName(Index) override;
        String getScalarFieldName(Index);
        //  Real getScalarFieldValue(Index) override;
        Real getScalarFieldValue(Index);
        //  Index getVectorFieldsCount() override;
        Index getVectorFieldsCount();
        //  String getVectorFieldName(Index) override;
        String getVectorFieldName(Index);
        //  Vector getVectorFieldValue(Index) override;
        Vector getVectorFieldValue(Index);

    };



}
#endif //CHARM_3D_V2_DATADGHEAT_H