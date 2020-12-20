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

    void MethodFvmLMCh::calcVisc() {
        Index compCount = Config::getCompCount();

        for (Index iFace = 0; iFace < mesh->fCount; iFace++) {
            Face &face = mesh->faces[iFace];
            Vector n = face.n;
            if (face.cells.size() == 1) {
                continue; // @todo for wall
            }
            else {
                Index c1 = face.cells[0];
                Index c2 = face.cells[1];
                Prim p1 = data[c1].getPrim();
                Prim p2 = data[c2].getPrim();

                Real tt1 = (-2. / 3.) * p1.ml * (data[c1].gradU.x + data[c1].gradV.y + data[c1].gradW.z);
                Real txx1 = tt1 + 2. * p1.ml * data[c1].gradU.x;
                Real tyy1 = tt1 + 2. * p1.ml * data[c1].gradV.y;
                Real tzz1 = tt1 + 2. * p1.ml * data[c1].gradW.z;
                Real txy1 = p1.ml * (data[c1].gradU.y + data[c1].gradV.x);
                Real txz1 = p1.ml * (data[c1].gradU.z + data[c1].gradW.x);
                Real tyz1 = p1.ml * (data[c1].gradW.y + data[c1].gradV.z);

                Real tt2 = (-2. / 3.) * p2.ml * (data[c2].gradU.x + data[c2].gradV.y + data[c2].gradW.z);
                Real txx2 = tt2 + 2. * p2.ml * data[c2].gradU.x;
                Real tyy2 = tt2 + 2. * p2.ml * data[c2].gradV.y;
                Real tzz2 = tt2 + 2. * p2.ml * data[c2].gradW.z;
                Real txy2 = p2.ml * (data[c2].gradU.y + data[c2].gradV.x);
                Real txz2 = p2.ml * (data[c2].gradU.z + data[c2].gradW.x);
                Real tyz2 = p2.ml * (data[c2].gradW.y + data[c2].gradV.z);

                Real tu1 = txx1 * n.x + txy1 * n.y + txz1 * n.z;
                Real tv1 = txy1 * n.x + tyy1 * n.y + tyz1 * n.z;
                Real tw1 = txz1 * n.x + tyz1 * n.y + tzz1 * n.z;

                Real tu2 = txx2 * n.x + txy2 * n.y + txz2 * n.z;
                Real tv2 = txy2 * n.x + tyy2 * n.y + tyz2 * n.z;
                Real tw2 = txz2 * n.x + tyz2 * n.y + tzz2 * n.z;

                Real vol1 = mesh->cells[c1].volume;
                Real vol2 = mesh->cells[c2].volume;
                Real vols = vol1 + vol2;
                Real tu = (vol1 * tu1 + vol2 * tu2) / vols;
                Real tv = (vol1 * tv1 + vol2 * tv2) / vols;
                Real tw = (vol1 * tw1 + vol2 * tw2) / vols;

                integrals[c1].ru -= tu;
                integrals[c1].rv -= tv;
                integrals[c1].rw -= tw;

                integrals[c2].ru += tu;
                integrals[c2].rv += tv;
                integrals[c2].rw += tw;
            }
        }
    }

}
