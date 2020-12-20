/*
 *     ________  _____    ____  __  ___    _____ ____
 *    / ____/ / / /   |  / __ \/  |/  /   |__  // __ \  __    __
 *   / /   / /_/ / /| | / /_/ / /|_/ /     /_ </ / / /_/ /___/ /_
 *  / /___/ __  / ___ |/ _, _/ /  / /    ___/ / /_/ /_  __/_  __/
 *  \____/_/ /_/_/  |_/_/ |_/_/  /_/____/____/_____/ /_/   /_/
 *
 */


//
// Created by zhrv on 15.12.2020.
//

#ifndef CHARM_3D_V2_DATAFVMLMCH_H
#define CHARM_3D_V2_DATAFVMLMCH_H

#include <Data.h>
#include <Prim.h>

namespace charm {

    class DataFvmLMCh : public Data{
    public:

        class Cons
        {
        protected:
            Real          ru;
            Real          rv;
            Real          rw;
            Real          rh;
            ArrayReal     rc;

        public:
            explicit Cons(Index compCount);
            Cons(const Cons& cons);
            inline void shrink();
            Cons& operator  = (const Cons& cons);
            Cons& operator  = (const Real& a);
            Cons& operator += (const Cons& cons);
            Cons& operator -= (const Cons& cons);
            Cons& operator *= (const Real& a);
            void normalize();
            inline Index size() const;

            friend class DataFvmLMCh;
            friend class FluxFvmLMChLF;
        };


    public:
        Cons        c;
        Real        p0;
        Real        p;
        Index       matId;

        Vector          gradT;
        Vector          gradU;
        Vector          gradV;
        Vector          gradW;
        Array<Vector>   gradC;
        Array<Vector>   gradH;

        explicit DataFvmLMCh(Index compCount): c(compCount) {}

        void getPrim(Prim &p);
        Prim getPrim();
        Cons& getCons();
        void setCons(const Prim &prim);
        void getBuffer(Byte *) override;
        void setBuffer(Byte *) override;
        inline Index size() const override;

        Index getScalarFieldsCount() override;
        String getScalarFieldName(Index) override;
        Real getScalarFieldValue(Index) override;
        Index getVectorFieldsCount() override;
        String getVectorFieldName(Index) override;
        Vector getVectorFieldValue(Index) override;
    };

}

#endif //CHARM_3D_V2_DATAFVMLMCH_H
