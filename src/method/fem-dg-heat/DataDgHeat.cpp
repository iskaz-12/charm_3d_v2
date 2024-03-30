//
// Created by zhrv on 17.02.2023.
//

//  UPDATE ON 04.07.2023 - реализация методов класса данных для решения задач теплопроводности

#include "DataDgHeat.h"
#include "Config.h"
#include "MethodException.h"
#include "ConfigFemDgHeat.h"

//  ---08.03.2024---
#include "MethodFemDgHeat.h"

//  ---18.01.24---
// Для избежания ошибок переполнения переделываю примитивные и консервативные переменные в РМГ по аналогии с fvm!!!

namespace charm {
    PrimHeat& PrimHeat::operator = (const PrimHeat &flds) {
        t           = flds.t;
        qx          = flds.qx;
        qy          = flds.qy;
        qz          = flds.qz;

        return *this;
    }

    void PrimHeat::normalize() {
        if (::fabs(t) < EPS) t = 0.;
        if (::fabs(qx) < EPS) qx = 0.;
        if (::fabs(qy) < EPS) qy = 0.;
        if (::fabs(qz) < EPS) qz = 0.;
    }

    PrimHeat &PrimHeat::operator += (const PrimHeat &flds) {
        t += flds.t;
        qx += flds.qx;
        qy += flds.qy;
        qz += flds.qz;
        return *this;
    }

    PrimHeat &PrimHeat::operator -= (const PrimHeat &flds) {
        t -= flds.t;
        qx -= flds.qx;
        qy -= flds.qy;
        qz -= flds.qz;
        return *this;
    }

    PrimHeat &PrimHeat::operator *= (const Real &a) {
        t *= a;
        qx *= a;
        qy *= a;
        qz *= a;
        return *this;
    }


    //  ---08.03.2024---
    HeatDgFields& HeatDgFields::operator = (const HeatDgFields &flds) {

        t.assign(flds.t.begin(), flds.t.end());
        qx.assign(flds.qx.begin(), flds.qx.end());
        qy.assign(flds.qy.begin(), flds.qy.end());
        qz.assign(flds.qz.begin(), flds.qz.end());

        intT.assign(flds.intT.begin(), flds.intT.end());
        intQx.assign(flds.intQx.begin(), flds.intQx.end());
        intQy.assign(flds.intQy.begin(), flds.intQy.end());
        intQz.assign(flds.intQz.begin(), flds.intQz.end());


        return *this;
    }


    /**
     *
     * @param p
     */
    void DataDgHeat::getVar(PrimHeat &p) {
        p = getVar();
    }


    /**
     *
     * @return
     */
    PrimHeat DataDgHeat::getVar() {

        //  ---08.03.2024---
        Index baseFuncCount = 4;

        PrimHeat p(1);
        
        p.t = 0.;
        p.qx = 0.;
        p.qy = 0.;
        p.qz = 0.;

        return p;
    }

    /**
     *
     * @param p
     */
    void DataDgHeat::getBuffer(Byte *buf) {
        Index shift = 0;
        for (Real t : flds.t) {
            *(Real *) (&buf[shift]) = t;
            shift += sizeof(Real);
        }
        for (Real q : flds.qx) {
            *(Real *) (&buf[shift]) = q;
            shift += sizeof(Real);
        }
        for (Real q : flds.qy) {
            *(Real *) (&buf[shift]) = q;
            shift += sizeof(Real);
        }
        for (Real q : flds.qz) {
            *(Real *) (&buf[shift]) = q;
            //shift += sizeof(Real);
        }
    }

    void DataDgHeat::setBuffer(Byte *buf) {
        Index shift = 0;
        for (Real t : flds.t) {
            t = *(Real *) (&buf[shift]);
            shift += sizeof(Real);
        }
        for (Real q : flds.qx) {
            q = *(Real *) (&buf[shift]);
            shift += sizeof(Real);
        }
        for (Real q : flds.qy) {
            q = *(Real *) (&buf[shift]);
            shift += sizeof(Real);
        }
        for (Real q : flds.qz) {
            q = *(Real *) (&buf[shift]);
            shift += sizeof(Real);
        }
    }

    Index DataDgHeat::getScalarFieldsCount() {
        return 1;
    }

    String DataDgHeat::getScalarFieldName(Index iFld) {
        if (iFld == 0) {
            return "T";
        }

        throw Exception("Wrong field number");
    }

    Real DataDgHeat::getScalarFieldValue(Index iFld) {
        if (iFld == 0) {
            return getTAvg();
        }

        throw Exception("Wrong field number");
    }

    Index DataDgHeat::getVectorFieldsCount() {
        return 1;
    }

    String DataDgHeat::getVectorFieldName(Index) {
        return "Grad_T";
    }

    Vector DataDgHeat::getVectorFieldValue(Index) {
        return getGradTAvg();
    }

    HeatDgFields &DataDgHeat::getFields() {
        return flds;
    }
    
    //  UPDATE ON 06.07.2023 - с этой функции и ниже идёт моя реализация
    //  UPDATE ON 12.07.2023 - setFields вызывает ошибки, поэтому пока комментирую
    //  void DataDgHeat::setFields(const PrimHeat &p) {
    //      p = flds;
    //  }

    //  UPDATE ON 07.07.2023 - в предыдущем варианте кода, возможно, есть подобие реализации данных функций.
    Real DataDgHeat::getT(Point pt) {

    }

    Vector DataDgHeat::getGradT(Point pt) {

    }

    Real DataDgHeat::getTAvg() {

    }

    Vector DataDgHeat::getGradTAvg() {

    }

}