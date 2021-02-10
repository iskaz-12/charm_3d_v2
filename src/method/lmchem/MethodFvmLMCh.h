/*
 *     ________  _____    ____  __  ___    _____ ____
 *    / ____/ / / /   |  / __ \/  |/  /   |__  // __ \  __    __
 *   / /   / /_/ / /| | / /_/ / /|_/ /     /_ </ / / /_/ /___/ /_
 *  / /___/ __  / ___ |/ _, _/ /  / /    ___/ / /_/ /_  __/_  __/
 *  \____/_/ /_/_/  |_/_/ |_/_/  /_/____/____/_____/ /_/   /_/
 *
 */


//
// Created by zhrv on 14.12.2020.
//

#ifndef CHARM_3D_V2_METHODFVMLMCH_H
#define CHARM_3D_V2_METHODFVMLMCH_H

#include <Method.h>
#include "FluxFvmLMCh.h"

namespace charm {

    class MethodFvmLMCh : public Method {
    public:
        ArrayReal           ru;
        ArrayReal           rv;
        ArrayReal           rw;
        ArrayReal           rh;
        Array<ArrayReal>    rc;

        Real                p0;
        ArrayReal           p;
        Array<ArrayReal>    d;
        ArrayIndex          matId;

        ArrayReal           ruInt;
        ArrayReal           rvInt;
        ArrayReal           rwInt;
        ArrayReal           rhInt;
        Array<ArrayReal>    rcInt;

        FluxFvmLMCh*                flux;


        ArrayReal               fld;
        ArrayVector             vecFld;
        ArrayReal               rhsP;
        ArrayReal               S;

        ArrayVector             gradP;
        ArrayVector             gradT;
        ArrayVector             gradU;
        ArrayVector             gradV;
        ArrayVector             gradW;
        Array<ArrayVector>      gradC;
        Array<ArrayVector>      gradH;

        explicit MethodFvmLMCh(Config* conf);
        ~MethodFvmLMCh();

        void init() override;

        void run() override;

        void done() override;


        void calcChem();
        void calcAdv();
        void calcGrad();
        void calcConv();
        void calcDiff();
        void calcVisc();
        void calcHeat();
        void calcPress();
        void calcS();
        void correctVelosities();
        void caldDiffCoeff();


        void save();

        Real calcDt();

        Prim getPrim(Index) override;
        void setCons(Index iCell, const Prim &p) override;

        Index getScalarFieldsCount() override;
        String getScalarFieldName(Index) override;
        Real getScalarFieldValue(Index, Index) override;
        Index getVectorFieldsCount() override;
        String getVectorFieldName(Index) override;
        Vector getVectorFieldValue(Index, Index) override;
    protected:
        void opLaplace(ArrayReal &out, ArrayReal &in);

        void zeroIntegrals();
    };

}

#endif //CHARM_3D_V2_METHODFVMLMCH_H
