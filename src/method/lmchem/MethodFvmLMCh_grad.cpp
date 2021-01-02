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

    void MethodFvmLMCh::calcGrad() {
        Index compCount = Config::getCompCount();

        for (auto &d : data) {
            d.gradT = 0.;
            d.gradU = 0.;
            d.gradV = 0.;
            d.gradW = 0.;
            d.gradC.resize(compCount, {0., 0., 0.});
            d.gradH.resize(compCount, {0., 0., 0.});
        }

        for (Index iFace = 0; iFace < mesh->fCount; iFace++) {
            Face &face = mesh->faces[iFace];
            Vector n = face.n;
            bool isBnd = face.cells.size() == 1;
            Index c1 = face.cells[0];
            Index c2;
            Prim p1 = data[c1].getPrim();
            Prim p2(compCount);

            Real vol1 = mesh->cells[c1].volume;
            Real vol2;

            if (isBnd) {
                face.bnd->calc(p1, p2, face.n);
                vol2 = vol1;
            } else {
                c2 = face.cells[1];
                p2 = data[c2].getPrim();
                vol2 = mesh->cells[c2].volume;
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
                Component *comp = Config::getComponent(i);
                vC[i] *= vol1 * p1.c[i]           + vol2 * p2.c[i];
                vH[i] *= vol1 * comp->calcH(p1.t) + vol2 * comp->calcH(p2.t);
            }


            data[c1].gradT += vT;
            data[c1].gradU += vU;
            data[c1].gradV += vV;
            data[c1].gradW += vW;
            for (Index i = 0; i < compCount; i++) {
                data[c1].gradC[i] += vC[i];
                data[c1].gradH[i] += vH[i];
            }
            if (!isBnd) {
                data[c2].gradT -= vT;
                data[c2].gradU -= vU;
                data[c2].gradV -= vV;
                data[c2].gradW -= vW;
                for (Index i = 0; i < compCount; i++) {
                    data[c2].gradC[i] -= vC[i];
                    data[c2].gradH[i] -= vH[i];
                }
            }
        }

        for (Index iCell = 0; iCell < mesh->cCount; iCell++) {
            Real vol = mesh->cells[iCell].volume;
            data[iCell].gradT /= vol;
            data[iCell].gradU /= vol;
            data[iCell].gradV /= vol;
            data[iCell].gradW /= vol;
            for (Index i = 0; i < compCount; i++) {
                data[iCell].gradC[i] /= vol;
                data[iCell].gradH[i] /= vol;
            }
        }

        exchange(); // @todo only gradients

    }

}
