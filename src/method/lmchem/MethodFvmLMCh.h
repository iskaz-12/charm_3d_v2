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
#include "DataFvmLMCh.h"
#include "FluxFvmLMCh.h"

namespace charm {

    class MethodFvmLMCh : public Method {
    public:
        Array<DataFvmLMCh>          data;
        Array<DataFvmLMCh::Cons>    integrals;
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

        Data* getData(Index iCell) override;

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

        using Method::exchange;

        void save();

        Real calcDt();

    protected:
        void opLaplace(ArrayReal &out, ArrayReal &in);
        Real opScProd(ArrayReal &a, ArrayReal &b);

        void opCopy(ArrayReal &dest, ArrayReal &src);
        void opSub(ArrayReal &a, ArrayReal &b); //< a -= b;
        void opAdd(ArrayReal &a, ArrayReal &b); //< a += b;

        void opMult(ArrayReal &a, Real b);
        Real opNorm(ArrayReal &a);

        void opGrad(ArrayVector &out, ArrayReal &in);
        void opDiv(ArrayReal &out, ArrayVector &in);
    };

}

#endif //CHARM_3D_V2_METHODFVMLMCH_H
