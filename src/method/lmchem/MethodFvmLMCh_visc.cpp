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
        for (Index iFace = 0; iFace < mesh->getFacesCount(); iFace++) {
            Face &face = mesh->getFace(iFace);
            Vector n = face.n;
            if (face.cells.size() == 1) {
//                if (instanceof<std::shared_ptr<BoundaryConditionWallNoSlip>>(face.bnd.get())) {
                if (face.bnd->type == BoundaryCondition::WALL_NO_SLIP) {
                    Index c1 = face.cells[0];
                    Prim p1 = data[c1].getPrim();

                    Real un = scalarProd(p1.v, n);
                    Vector vn = n;
                    vn *= un;
                    Vector vt = p1.v;
                    vt -= vn;

                    Vector l = face.center;
                    l -= mesh->getCell(c1).center;
                    Real ll = l.length();

                    Real qu = -p1.ml*vt[0]/ll;
                    Real qv = -p1.ml*vt[1]/ll;
                    Real qw = -p1.ml*vt[2]/ll;

                    integrals[c1].ru -= qu;
                    integrals[c1].rv -= qv;
                    integrals[c1].rw -= qw;
                }
            }
            else {
                Index c1 = face.cells[0];
                Index c2 = face.cells[1];
                Prim p1 = data[c1].getPrim();
                Prim p2 = data[c2].getPrim();

                Real tt1 = (-2. / 3.) * p1.ml * (gradU[c1].x + gradV[c1].y + gradW[c1].z);
                Real txx1 = tt1 + 2. * p1.ml * gradU[c1].x;
                Real tyy1 = tt1 + 2. * p1.ml * gradV[c1].y;
                Real tzz1 = tt1 + 2. * p1.ml * gradW[c1].z;
                Real txy1 = p1.ml * (gradU[c1].y + gradV[c1].x);
                Real txz1 = p1.ml * (gradU[c1].z + gradW[c1].x);
                Real tyz1 = p1.ml * (gradW[c1].y + gradV[c1].z);

                Real tt2 = (-2. / 3.) * p2.ml * (gradU[c2].x + gradV[c2].y + gradW[c2].z);
                Real txx2 = tt2 + 2. * p2.ml * gradU[c2].x;
                Real tyy2 = tt2 + 2. * p2.ml * gradV[c2].y;
                Real tzz2 = tt2 + 2. * p2.ml * gradW[c2].z;
                Real txy2 = p2.ml * (gradU[c2].y + gradV[c2].x);
                Real txz2 = p2.ml * (gradU[c2].z + gradW[c2].x);
                Real tyz2 = p2.ml * (gradW[c2].y + gradV[c2].z);

                Real tu1 = txx1 * n.x + txy1 * n.y + txz1 * n.z;
                Real tv1 = txy1 * n.x + tyy1 * n.y + tyz1 * n.z;
                Real tw1 = txz1 * n.x + tyz1 * n.y + tzz1 * n.z;

                Real tu2 = txx2 * n.x + txy2 * n.y + txz2 * n.z;
                Real tv2 = txy2 * n.x + tyy2 * n.y + tyz2 * n.z;
                Real tw2 = txz2 * n.x + tyz2 * n.y + tzz2 * n.z;

                Real vol1 = mesh->getCell(c1).volume;
                Real vol2 = mesh->getCell(c2).volume;
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
