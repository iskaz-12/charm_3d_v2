//
// Created by zhrv on 19.11.2020.
//

#ifndef CHARM_3D_V2_DATAFVM_H
#define CHARM_3D_V2_DATAFVM_H

#include <Point.h>
#include "global.h"
#include "Material.h"
#include <cmath>
#include <method/Data.h>
#include "mpi.h"


namespace charm {

    class Prim
    {
    public:
        Real          r;             ///< density
        Vector        v;             ///< velosity
        Real          e;             ///< energy
        Real          eTot;          ///< total energy
        Real          p;             ///< pressure
        Real          t;             ///< temperature
        Real          cz;            ///< sound velosity
        Real          gam;
        Real          cp;
        Real          cv;
        Real          m;
        Index         matId;
        ArrayReal     c;             ///< concentrations


        Prim() = delete;

        Prim(Index compCount): c(compCount) {}

        Prim(const Prim& prim) { *this = prim; }


        /**
         * 
         * @return 
         */
        inline Real v2() const { return v.sqr(); }

        /**
         *
         */
        inline void shrink() { c.shrink_to_fit(); }

        Prim& operator = (const Prim &p);

        void eos(Material::EosFlag flag);

        inline bool isNaN() {
            bool res = false;
            res |= std::isnan(r);
            res |= std::isnan(e);
            res |= std::isnan(eTot);
            res |= std::isnan(p);
            res |= std::isnan(t);
            res |= std::isnan(cz);
            res |= std::isnan(gam);
            res |= std::isnan(cp);
            res |= std::isnan(cv);
            res |= std::isnan(m);
            res |= std::isnan(v.x);
            res |= std::isnan(v.y);
            res |= std::isnan(v.z);
            for (auto ci : c) {
                res |= std::isnan(ci);
            }
            return res;
        }
    };

    class Cons
    {
    public:
        Real          ru;
        Real          rv;
        Real          rw;
        Real          re;
        ArrayReal     rc;
        Index         matId;


        explicit Cons(Index compCount): ru(0.), rv(0.), rw(0.), re(0.), rc(compCount), matId(0) {}

        Cons(const Cons& cons) { *this = cons; }

        inline void shrink() { rc.shrink_to_fit(); }

        Cons& operator  = (const Cons& cons);
        Cons& operator  = (const Real& a);
        Cons& operator += (const Cons& cons);
        Cons& operator -= (const Cons& cons);
        Cons& operator *= (const Real& a);

        void normalize();

        inline Index size() const { return sizeof(Real)*(4+rc.size())+sizeof(Index); }

    };

    class DataFvm : public Data {
    public:
        Cons    c;

        explicit DataFvm(Index compCount): c(compCount) {}
        
        void getPrim(Prim &p);
        Prim getPrim();
        Cons& getCons();
        void setCons(const Prim &p);
        void getBuffer(Byte *) override;
        void setBuffer(Byte *) override;
        Index size() override { return c.size(); }

        Index getScalarFieldsCount() override;
        String getScalarFieldName(Index) override;
        Real getScalarFieldValue(Index) override;
        Index getVectorFieldsCount() override;
        String getVectorFieldName(Index) override;
        Vector getVectorFieldValue(Index) override;
    };

}
#endif //CHARM_3D_V2_DATAFVM_H
