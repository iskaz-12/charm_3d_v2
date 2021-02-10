/*
 *     ________  _____    ____  __  ___    _____ ____
 *    / ____/ / / /   |  / __ \/  |/  /   |__  // __ \  __    __
 *   / /   / /_/ / /| | / /_/ / /|_/ /     /_ </ / / /_/ /___/ /_
 *  / /___/ __  / ___ |/ _, _/ /  / /    ___/ / /_/ /_  __/_  __/
 *  \____/_/ /_/_/  |_/_/ |_/_/  /_/____/____/_____/ /_/   /_/
 *
 */

#include <Parallel.h>
#include "MethodFvmLMCh.h"


namespace charm {

    void MethodFvmLMCh::calcGrad() {
        Mesh& mesh = Config::getMesh();
        Index lN = mesh.getCellsCount();
        Index gN = mesh.getCellsCountWithGhost();
        Index fN = mesh.getFacesCount();
        Index compCount = Config::getCompCount();

        for (Index i = 0; i < gN; i++) {
            gradT[i] = 0.;
            gradU[i] = 0.;
            gradV[i] = 0.;
            gradW[i] = 0.;
            gradC[i].resize(compCount, {0., 0., 0.});
            gradH[i].resize(compCount, {0., 0., 0.});
        }

        for (Index iFace = 0; iFace < fN; iFace++) {
            Face &face = mesh.getFace(iFace);
            Vector n = face.n;
            bool isBnd = face.cells.size() == 1;
            Index c1 = face.cells[0];
            Index c2;
            Prim p1 = getPrim(c1);
            Prim p2(compCount);

            Real vol1 = mesh.getCell(c1).volume;
            Real vol2;

            if (isBnd) {
                face.bnd->calc(p1, p2, face.n);
                vol2 = vol1;
            } else {
                c2 = face.cells[1];
                p2 = getPrim(c2);
                vol2 = mesh.getCell(c2).volume;
            }

            Real s = face.area / (vol1 + vol2);
            vol1 *= s;
            vol2 *= s;

            Vector vT(n), vU(n), vV(n), vW(n);
            Array<Vector> vC(compCount, n);
            Array<Vector> vH(compCount, n);


            vT *= vol1 * p1.t   + vol2 * p2.t;
            vU *= vol1 * p1.v.x + vol2 * p2.v.x;
            vV *= vol1 * p1.v.y + vol2 * p2.v.y;
            vW *= vol1 * p1.v.z + vol2 * p2.v.z;
            for (Index i = 0; i < compCount; i++) {
                Component* comp = Config::getComponent(i);
                vC[i] *= vol1 * p1.c[i]           + vol2 * p2.c[i];
                vH[i] *= vol1 * comp->calcH(p1.t) + vol2 * comp->calcH(p2.t);
            }


            gradT[c1] += vT;
            gradU[c1] += vU;
            gradV[c1] += vV;
            gradW[c1] += vW;
            for (Index i = 0; i < compCount; i++) {
                gradC[c1][i] += vC[i];
                gradH[c1][i] += vH[i];
            }
            if (!isBnd) {
                gradT[c2] -= vT;
                gradU[c2] -= vU;
                gradV[c2] -= vV;
                gradW[c2] -= vW;
                for (Index i = 0; i < compCount; i++) {
                    gradC[c2][i] -= vC[i];
                    gradH[c2][i] -= vH[i];
                }
            }
        }

        for (Index iCell = 0; iCell < lN; iCell++) {
            Real vol = mesh.getCell(iCell).volume;
            gradT[iCell] /= vol;
            gradU[iCell] /= vol;
            gradV[iCell] /= vol;
            gradW[iCell] /= vol;
            for (Index i = 0; i < compCount; i++) {
                gradC[iCell][i] /= vol;
                gradH[iCell][i] /= vol;
            }
        }

        Parallel::exchange(gradT);
        Parallel::exchange(gradU);
        Parallel::exchange(gradV);
        Parallel::exchange(gradW);

        Parallel::exchange(gradC);
        Parallel::exchange(gradH);
    }

}
