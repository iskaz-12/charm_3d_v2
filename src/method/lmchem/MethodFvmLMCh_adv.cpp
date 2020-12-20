/*
 *     ________  _____    ____  __  ___    _____ ____
 *    / ____/ / / /   |  / __ \/  |/  /   |__  // __ \  __    __
 *   / /   / /_/ / /| | / /_/ / /|_/ /     /_ </ / / /_/ /___/ /_
 *  / /___/ __  / ___ |/ _, _/ /  / /    ___/ / /_/ /_  __/_  __/
 *  \____/_/ /_/_/  |_/_/ |_/_/  /_/____/____/_____/ /_/   /_/
 *
 */

#include "MethodFvmLMCh.h"


namespace charm {
    using Cons = DataFvmLMCh::Cons;

    void MethodFvmLMCh::calcAdv() {



        for (auto &i : integrals) {
            i = 0.;
        }

        calcDiff();
        calcVisc();
        calcHeat();
        calcConv();


        Real dt = calcDt();

        for (Index ic = 0; ic < mesh->cCount; ic++) {
            integrals[ic].normalize();
            Real cfl = dt / mesh->cells[ic].volume;
            integrals[ic] *= cfl;
            data[ic].c -= integrals[ic];
        }

        exchange();

    }


}
