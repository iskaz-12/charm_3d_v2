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
         // UPDATE ON 19.06.2023 - сокращение памяти под массив концентраций
        inline void shrink() { c.shrink_to_fit(); }

        Prim& operator = (const Prim &p);

        //  UPDATE ON 24.06.2023 - получение уравнения состояния
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

        // UPDATE ON 22.06.2023 - сокращение памяти под массив концентраций (для случая консервативных переменных)
        inline void shrink() { rc.shrink_to_fit(); }

        Cons& operator = (const Cons& cons);

        //  UPDATE ON 24.06.2023 - нормализация консервативных переменных (малые значения (меньше eps) приравниваются к 0)
        void normalize();

    };

    class Data {
    public:
        Cons    c;
        Vector  grav;

        explicit Data(Index compCount): c(compCount) {}
        
        //  UPDATE ON 24.06.2023 - получение примитивных переменных по консервативным
        void getPrim(Prim &p);
        Prim getPrim();

        //  UPDATE ON 24.06.2023 - возвращение консервативных переменных
        Cons& getCons();
        //  UPDATE ON 24.06.2023 - получение консервативных переменных по примитивным
        void setCons(const Prim &p);
    };

}
#endif //CHARM_3D_V2_DATA_H
