/*
 *     ________  _____    ____  __  ___    _____ ____
 *    / ____/ / / /   |  / __ \/  |/  /   |__  // __ \  __    __
 *   / /   / /_/ / /| | / /_/ / /|_/ /     /_ </ / / /_/ /___/ /_
 *  / /___/ __  / ___ |/ _, _/ /  / /    ___/ / /_/ /_  __/_  __/
 *  \____/_/ /_/_/  |_/_/ |_/_/  /_/____/____/_____/ /_/   /_/
 *
 */

#include <cassert>
#include "MethodFvmLMCh.h"

namespace charm {

    void MethodFvmLMCh::opLaplace(ArrayReal &out, ArrayReal &in) {
        assert(
                in.size() == out.size() &&
                in.size() == mesh->getCellsCountWithGhost() &&
                "Size of arrays must be equal to cells count"
                );

//        ArrayVector gradIn(mesh->getCellsCountWithGhost(), {0., 0., 0.});

        opGrad(vecFld, in);


        for (Index iCell = 0; iCell < mesh->getCellsCountWithGhost(); iCell++) {
            Prim prim = data[iCell].getPrim();
            vecFld[iCell] /= prim.r;
        }

        opDiv(out, vecFld);

    }


    void MethodFvmLMCh::opCopy(ArrayReal &dest, ArrayReal &src) {
        assert( src.size() == mesh->getCellsCountWithGhost() &&
                        src.size() == dest.size() &&
                        "Size of array must be equal to cells count"
                );

        for (Index i = 0; i < mesh->getCellsCount(); i++) {
            dest[i] = src[i];
        }
        exchange(dest);
    }


    void MethodFvmLMCh::opMult(ArrayReal &a, Real b) {
        assert( a.size() == mesh->getCellsCountWithGhost() &&
                "Size of array must be equal to cells count"
                );

        for (Index i = 0; i < mesh->getCellsCount(); i++) {
            a[i] *= b;
        }
        exchange(a);
    }


    Real MethodFvmLMCh::opScProd(ArrayReal &a, ArrayReal &b) {
        assert( a.size() == b.size() &&
                a.size() == mesh->getCellsCountWithGhost() &&
                "Size of arrays must be equal to cells count"
                );

        Real s = 0.;
        for (Index i = 0; i < mesh->getCellsCount(); i++) {
            s += a[i]*b[i];
        }

        Parallel::sum(&s);
        return s;
    }


    void MethodFvmLMCh::opAdd(ArrayReal &a, ArrayReal &b) {
        assert( a.size() == b.size() &&
                a.size() == mesh->getCellsCountWithGhost() &&
                "Size of arrays must be equal to cells count"
        );
        for (Index i = 0; i < mesh->getCellsCount(); i++) {
            a[i] += b[i];
        }
        exchange(a);
    }


    void MethodFvmLMCh::opSub(ArrayReal &a, ArrayReal &b) {
        assert( a.size() == b.size() &&
                a.size() == mesh->getCellsCountWithGhost() &&
                "Size of arrays must be equal to cells count"
        );
        for (Index i = 0; i < mesh->getCellsCount(); i++) {
            a[i] -= b[i];
        }
        exchange(a);
    }


    Real MethodFvmLMCh::opNorm(ArrayReal &a) {
        assert( a.size() == mesh->getCellsCountWithGhost() &&
                "Size of arrays must be equal to cells count"
        );
        return sqrt(opScProd(a, a));
    }


    void MethodFvmLMCh::opGrad(ArrayVector &out, ArrayReal &in) {
        assert(
                in.size() == out.size() &&
                in.size() == mesh->getCellsCountWithGhost() &&
                "Size of arrays must be equal to cells count"
        );

        for (auto &item : out) {
            item.x = item.y = item.z = 0.;
        }

        for (Index iFace = 0; iFace < mesh->getFacesCount(); iFace++) {
            Face &face = mesh->getFace(iFace);
            Vector n = face.n;
            bool isBnd = face.cells.size() == 1;
            Index c1 = face.cells[0];
            Index c2;
            Real p1 = in[c1];
            Real p2;

            Real vol1 = mesh->getCellVolume(c1);
            Real vol2;

            if (isBnd) {
                p2 = p1; // @todo ?????
                vol2 = vol1;
            } else {
                c2 = face.cells[1];
                p2 = in[c2];
                vol2 = mesh->getCellVolume(c2);
            }

            Real s = face.area / (vol1 + vol2);
            vol1 *= s;
            vol2 *= s;

            Vector vP(n);

            vP *= vol1 * p1   + vol2 * p2;
            out[c1] += vP;
            if (!isBnd) {
                out[c2] -= vP;
            }
        }

        for (Index iCell = 0; iCell < mesh->getCellsCount(); iCell++) {
            Real vol = mesh->getCellVolume(iCell);
            out[iCell] /= vol;
        }

        exchange(out);


    }


    void MethodFvmLMCh::opDiv(ArrayReal &out, ArrayVector &in) {
        assert(
                in.size() == out.size() &&
                in.size() == mesh->getCellsCountWithGhost() &&
                "Size of arrays must be equal to cells count"
        );

        for (Index iCell = 0; iCell < mesh->getCellsCountWithGhost(); iCell++) {
            out[iCell] = 0.;
        }

        for (Index iFace = 0; iFace < mesh->getFacesCount(); iFace++) {
            Face &face = mesh->getFace(iFace);
            Vector n = face.n;
            bool isBnd = face.cells.size() == 1;
            Index c1 = face.cells[0];
            Index c2;
            Real gn1 = scalarProd(in[c1], n);
            Real gn2;


            Real vol1 = mesh->getCellVolume(c1);//cells[c1].volume;
            Real vol2;

            if (isBnd) {
                gn2 = gn1; // @todo ?????
                vol2 = vol1;
            } else {
                c2 = face.cells[1];
                gn2 = scalarProd(in[c2], n);
                vol2 = mesh->getCellVolume(c2);//cells[c2].volume;
            }

            Real s = face.area / (vol1 + vol2);
            vol1 *= s;
            vol2 *= s;

            Real gn = vol1 * gn1 + vol2 * gn2;

            out[c1] += gn;
            if (!isBnd) {
                out[c2] -= gn;
            }
        }

//        for (Index iCell = 0; iCell < mesh->getCellsCount(); iCell++) {
//            Real vol = mesh->getCellVolume(iCell);
//            out[iCell] /= vol;
//        }

        exchange(out);
    }

}

