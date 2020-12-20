//
// Created by zhrv on 15.12.2020.
//

#ifndef CHARM_3D_V2_PRIM_H
#define CHARM_3D_V2_PRIM_H

#include <Point.h>
#include "Material.h"

namespace charm {

    class Prim
    {
    public:
        Real          r;             ///< density
        Vector        v;             ///< velosity
        Real          e;             ///< energy
        Real          eTot;          ///< total energy
        Real          h;             ///< energy
        Real          hTot;          ///< total energy
        Real          p0;            ///< pressure
        Real          p;             ///< pressure
        Real          t;             ///< temperature
        Real          cz;            ///< sound velosity
        Real          gam;
        Real          cp;
        Real          cv;
        Real          m;
        Real          ml;
        Real          kp;
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

        Prim& operator = (const Prim &prim) {
            r           = prim.r;
            v           = prim.v;
            e           = prim.e;
            eTot        = prim.eTot;
            h           = prim.h;
            p0          = prim.p0;
            this->p     = prim.p;
            t           = prim.t;
            cz          = prim.cz;
            gam         = prim.gam;
            cp          = prim.cp;
            cv          = prim.cv;
            m           = prim.m;
            matId       = prim.matId;
            c.assign(prim.c.begin(), prim.c.end());

            return *this;
        }

        void eos(Material::EosFlag flag);

        inline bool isNaN() {
            bool res = false;
            res |= std::isnan(r);
            res |= std::isnan(e);
            res |= std::isnan(eTot);
            res |= std::isnan(h);
            res |= std::isnan(hTot);
            res |= std::isnan(p0);
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

}

#endif //CHARM_3D_V2_PRIM_H
