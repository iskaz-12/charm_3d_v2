/*
 *     ________  _____    ____  __  ___    _____ ____
 *    / ____/ / / /   |  / __ \/  |/  /   |__  // __ \  __    __
 *   / /   / /_/ / /| | / /_/ / /|_/ /     /_ </ / / /_/ /___/ /_
 *  / /___/ __  / ___ |/ _, _/ /  / /    ___/ / /_/ /_  __/_  __/
 *  \____/_/ /_/_/  |_/_/ |_/_/  /_/____/____/_____/ /_/   /_/
 *
 */

#include "MethodFvmLMCh.h"


namespace charm{
    using Cons = DataFvmLMCh::Cons;

    void MethodFvmLMCh::calcConv(){
        Index compCount = Config::getCompCount();

        for (Index iFace = 0; iFace < mesh->fCount; iFace++) {
            Face &face = mesh->faces[iFace];
            bool isBnd = face.cells.size() == 1;
            Index c1 = face.cells[0];
            Index c2;
            Prim p1 = data[c1].getPrim();
            Prim p2(compCount);

            if (isBnd) {
                face.bnd->calc(p1, p2, face.n);
            } else {
                c2 = face.cells[1];
                p2 = data[c2].getPrim();
            }

            Cons flx(compCount);

            flux->calc(p1, p2, flx, face.n);
            flx *= face.area;
            integrals[c1] += flx;
            if (!isBnd) {
                integrals[c2] -= flx;
            }
        }

    }

}