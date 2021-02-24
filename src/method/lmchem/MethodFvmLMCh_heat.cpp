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

    void MethodFvmLMCh::calcHeat() {
        Mesh& mesh = Config::getMesh();
        Index fN = mesh.getFacesCount();

        for (Index iFace = 0; iFace < fN; iFace++) {
            Face &face = mesh.getFace(iFace);
            Vector n = face.n;
            if (face.cells.size() == 1) {
//                if (instanceof<BoundaryConditionWallNoSlip>(face.bnd.get())) {
                if (face.bnd->type == BoundaryCondition::WALL_NO_SLIP) {
                    Index c1 = face.cells[0];
                    Prim p1 = getPrim(c1);
                    Vector _gradT = gradT[c1];

                    Vector l = face.center;
                    l -= mesh.getCell(c1).center;
                    Real ll = l.length();

                    Vector qt = _gradT;
                    qt *= -p1.kp;
                    Real qn = scalarProd(qt, n); // @todo проверить знаки

                    rh[c1] += qn;
                }
            }
            else {
                Index c1 = face.cells[0];
                Index c2 = face.cells[1];
                Prim p1 = getPrim(c1);
                Prim p2 = getPrim(c2);
                Vector gradT1 = gradT[c1];
                Vector gradT2 = gradT[c2];
                Real vol1 = mesh.getCell(c1).volume;
                Real vol2 = mesh.getCell(c2).volume;

                gradT1 *= vol1*p1.kp; // @todo проверить вычисление KP
                gradT2 *= vol2*p2.kp; // @todo проверить вычисление KP

                Vector qt = gradT1;
                qt += gradT2;
                qt /= (vol1 + vol2);
                qt *= face.area;

                Real qn = scalarProd(qt, n);

                rh[c1] += qn;
                rh[c2] -= qn;
            }
        }
    }

}