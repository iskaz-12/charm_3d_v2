//
// Created by zhrv on 17.02.2023.
//

#ifndef CHARM_3D_V2_DATADGHEAT_H
#define CHARM_3D_V2_DATADGHEAT_H

#include <Point.h>
#include "global.h"
#include "Material.h"
#include <cmath>
#include <method/Data.h>
#include <Cell.h>
#include "mpi.h"


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



        PrimHeat& operator  = (const PrimHeat& var);
        PrimHeat& operator  = (const Real& a);
        PrimHeat& operator += (const PrimHeat& var);
        PrimHeat& operator -= (const PrimHeat& var);
        PrimHeat& operator *= (const Real& a);

        void normalize();

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

        HeatDgFields& operator  = (const HeatDgFields& cons);
        HeatDgFields& operator  = (const Real& a);
        HeatDgFields& operator += (const HeatDgFields& cons);
        HeatDgFields& operator -= (const HeatDgFields& cons);
        HeatDgFields& operator *= (const Real& a);

        void normalize();

        inline Index size() const { return sizeof(Real)*t.size()*4; }

    };


    class DataDgHeat : public Data {
    public:
        HeatDgFields    flds;
        Cell           *cell;

        explicit DataDgHeat(Index bfCount): flds(bfCount) {}

        void getPrim(Point pt, PrimHeat &p);
        PrimHeat getPrim(Point pt);

        Real getT(Point pt);
        Vector getGradT(Point pt);

        Real getTAvg();
        Vector getGradTAvg();

        double getF(int i, Point pt);

        double getDfDx(int i, Point pt);
        double getDfDy(int i, Point pt);
        double getDfDz(int i, Point pt);

        HeatDgFields& getFields();
        void setFields(const PrimHeat &p);
        void getBuffer(Byte *) override;
        void setBuffer(Byte *) override;
        Index size() override { return flds.size(); }

        Index getScalarFieldsCount() override;
        String getScalarFieldName(Index) override;
        Real getScalarFieldValue(Index) override;
        Index getVectorFieldsCount() override;
        String getVectorFieldName(Index) override;
        Vector getVectorFieldValue(Index) override;

    };



}
#endif //CHARM_3D_V2_DATADGHEAT_H
