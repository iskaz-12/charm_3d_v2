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

    void MethodFvmLMCh::calcHeat() {
        for (Index iFace = 0; iFace < mesh->fCount; iFace++) {
            Face &face = mesh->faces[iFace];
            Vector n = face.n;
            if (face.cells.size() == 1) {
                if (instanceof<BoundaryConditionWallNoSlip>(face.bnd)) {
                    Index c1 = face.cells[0];
                    Prim p1 = data[c1].getPrim();
                    Vector gradT = data[c1].gradT;

                    Vector l = face.center;
                    l -= mesh->cells[c1].center;
                    Real ll = l.length();

                    Vector qt = gradT;
                    qt *= -p1.kp;
                    Real qn = scalarProd(qt, n); // @todo проверить знаки

                    integrals[c1].rh += qn;
                }
            }
            else {
                Index c1 = face.cells[0];
                Index c2 = face.cells[1];
                Prim p1 = data[c1].getPrim();
                Prim p2 = data[c2].getPrim();
                Vector gradT1 = data[c1].gradT;
                Vector gradT2 = data[c2].gradT;
                Real vol1 = mesh->cells[c1].volume;
                Real vol2 = mesh->cells[c2].volume;

                gradT1 *= vol1*p1.kp; // @todo проверить вычисление KP
                gradT2 *= vol2*p2.kp; // @todo проверить вычисление KP

                Vector qt = gradT1;
                qt += gradT2;
                qt /= (vol1 + vol2);
                qt *= face.area;

                Real qn = scalarProd(qt, n);

                integrals[c1].rh += qn;
                integrals[c2].rh -= qn;
            }
        }
    }

}