/*
 *     ________  _____    ____  __  ___    _____ ____
 *    / ____/ / / /   |  / __ \/  |/  /   |__  // __ \  __    __
 *   / /   / /_/ / /| | / /_/ / /|_/ /     /_ </ / / /_/ /___/ /_
 *  / /___/ __  / ___ |/ _, _/ /  / /    ___/ / /_/ /_  __/_  __/
 *  \____/_/ /_/_/  |_/_/ |_/_/  /_/____/____/_____/ /_/   /_/
 *
 */

#include "Parallel.h"
#include "MethodFvmLMCh.h"


namespace charm {

    void MethodFvmLMCh::calcAdv() {
        Mesh &mesh = Config::getMesh();
        Index cN = mesh.getCellsCount();
        Index compCount = Config::getCompCount();

        zeroIntegrals();

        calcDiff();
        calcVisc();
        calcHeat();
        calcConv();


        Real dt = calcDt();

        for (Index ic = 0; ic < cN; ic++) {
            Real cfl = dt / mesh.getCell(ic).volume;
            ru[ic] -= ruInt[ic] * cfl;
            rv[ic] -= rvInt[ic] * cfl;
            rw[ic] -= rwInt[ic] * cfl;
            rh[ic] -= rhInt[ic] * cfl;
            for (Index j = 0; j < compCount; j++) {
                rc[ic][j] -= rcInt[ic][j] * cfl;
            }
        }

        Parallel::exchange(ru);
        Parallel::exchange(rv);
        Parallel::exchange(rw);
        Parallel::exchange(rh);
//        Parallel::exchange(rc); // TODO

    }


}
