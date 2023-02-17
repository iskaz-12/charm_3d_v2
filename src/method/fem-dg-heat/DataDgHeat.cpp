//
// Created by zhrv on 17.02.2023.
//

#include "DataDgHeat.h"
#include "Config.h"
#include "MethodException.h"
#include "ConfigFemDgHeat.h"

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

    PrimHeat &PrimHeat::operator+=(const PrimHeat &flds) {
        t += flds.t;
        qx += flds.qx;
        qy += flds.qy;
        qz += flds.qz;
        return *this;
    }

    PrimHeat &PrimHeat::operator-=(const PrimHeat &flds) {
        t -= flds.t;
        qx -= flds.qx;
        qy -= flds.qy;
        qz -= flds.qz;
        return *this;
    }

    PrimHeat &PrimHeat::operator*=(const Real &a) {
        t *= a;
        qx *= a;
        qy *= a;
        qz *= a;
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
    PrimHeat& DataDgHeat::getVar() {
        return flds;
    }

    /**
     *
     * @param p
     */
    void DataDgHeat::setVar(const PrimHeat &p) {
        flds = p;
    }

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

    void DataDgHeat::setFields(const PrimHeat &p) {

    }

    Real DataDgHeat::getT(Point pt) {

    }

    Vector DataDgHeat::getGradT(Point pt) {

    }

    Real DataDgHeat::getTAvg() {

    }

    Vector DataDgHeat::getGradTAvg() {

    }

    double DataDgHeat::getF(int i, int iCell, Point pt) {
        return 0;
    }
    //	Переделала эту функцию в соответствии с обозначениями из charm_3d_v2
    double DataDgHeat::getF(int i, Point pt)
    {
        if (ConfigFemDgHeat::baseFuncCount == 4)
        {
            switch (i) {
                case 0:
                    return 1.0;
                case 1:
                    return (pt.x - cell->center.x) / cell->dh[0];
                case 2:
                    return (pt.y - cell->center.y) / cell->dh[1];
                case 3:
                    return (pt.z - cell->center.z) / cell->dh[2];
            }

        }

        else {
            printf("ERROR: wrong basic functions count!\n");
            exit(1);
        }
    }


    double DataDgHeat::getDfDx(int i, int iCell, Point pt)
    {
        if (ConfigFemDgHeat::baseFuncCount == 4)
        {
            switch(i) {
                case 0:
                    return 0.0;
                case 1:
                    return 1.0 / cell->dh[0];
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


    double DataDgHeat::getDfDy(int i, int iCell, Point pt)
    {
        if (ConfigFemDgHeat::baseFuncCount == 4)
        {
            switch(i) {
                case 0:
                    return 0.0;
                case 1:
                    return 0.0;
                case 2:
                    return 1.0 / cell->dh[1];
                case 3:
                    return 0.0;
            }
        }

        else {
            printf("ERROR: wrong basic functions count!\n");
            exit(1);
        }
    }


    double DataDgHeat::getDfDz(int i, int iCell, Point pt)
    {
        if (ConfigFemDgHeat::baseFuncCount == 4)
        {
            switch(i) {
                case 0:
                    return 0.0;
                case 1:
                    return 0.0;
                case 2:
                    return 0.0;
                case 3:
                    return 1.0 / cell->dh[2];
            }
        }

        else {
            printf("ERROR: wrong basic functions count!\n");
            exit(1);
        }
    }

}
