//
// Created by zhrv on 19.11.2020.
//

#ifndef CHARM_3D_V2_DATA_H
#define CHARM_3D_V2_DATA_H

#include <Point.h>
#include "global.h"
#include "Material.h"

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

        Cons& operator = (const Cons& cons);

        void normalize();

    };

    class Data {
    public:
        Cons    c;
        Vector  grav;

        explicit Data(Index compCount): c(compCount) {}
        
        void getPrim(Prim &p);
        Prim getPrim();
        Cons& getCons();
        void setCons(const Prim &p);
    };

}
#endif //CHARM_3D_V2_DATA_H
