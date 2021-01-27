//
// Created by zhrv on 19.11.2020.
//

#ifndef CHARM_3D_V2_DATAFVM_H
#define CHARM_3D_V2_DATAFVM_H

#include <Point.h>
#include "global.h"
#include "Material.h"
#include <cmath>
#include <Data.h>
#include "Prim.h"
#include "mpi.h"


namespace charm {


    class DataFvm : public Data {
    public:

        class Cons
        {
        public:
            Real          ru;
            Real          rv;
            Real          rw;
            Real          re;
            ArrayReal     rc;
            Index         matId;

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
        };



    public:
        Cons    c;

        explicit DataFvm(Index compCount);
        
        void getPrim(Prim &p);
        Prim getPrim();
        Cons& getCons();
        void setCons(const Prim &p);
        void getBuffer(Byte[]) override;
        void setBuffer(Byte[]) override;
        Index size() const override;
        Index getScalarFieldsCount() override;
        String getScalarFieldName(Index) override;
        Real getScalarFieldValue(Index) override;
        Index getVectorFieldsCount() override;
        String getVectorFieldName(Index) override;
        Vector getVectorFieldValue(Index) override;
    };

}
#endif //CHARM_3D_V2_DATAFVM_H
