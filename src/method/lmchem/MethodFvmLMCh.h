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
        FluxFvmLMCh                *flux;


        explicit MethodFvmLMCh(Config *conf);
        ~MethodFvmLMCh();

        void init() override;

        void run() override;

        void done() override;

        Data *getData(Index iCell) override;

        void calcChem();
        void calcAdv();
        void calcGrad();
        void calcConv();
        void calcDiff();
        void calcVisc();
        void calcPress();
        void correctVelosities();

        void save();

        Real calcDt();


    };

}

#endif //CHARM_3D_V2_METHODFVMLMCH_H
