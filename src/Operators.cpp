/**
 * Created by zhrv on 09.02.2021.
 * @author R.V.Zhalnin, zhalnin@gmail.com
 */

#include "Operators.h"
#include "Config.h"
#include "Parallel.h"

namespace charm {
    void Operators::copy(ArrayReal &dest, ArrayReal &src) {
        Mesh& mesh = Config::getMesh();

        assert( src.size() == mesh.getCellsCountWithGhost() &&
                src.size() == dest.size() &&
                "Size of array must be equal to cells count"
        );

        for (Index i = 0; i < mesh.getCellsCount(); i++) {
            dest[i] = src[i];
        }
        Parallel::exchange(dest);
    }


    void Operators::mult(ArrayReal &a, Real b) {
        Mesh& mesh = Config::getMesh();

        assert( a.size() == mesh.getCellsCountWithGhost() &&
                "Size of array must be equal to cells count"
        );

        for (Index i = 0; i < mesh.getCellsCount(); i++) {
            a[i] *= b;
        }
        Parallel::exchange(a);
    }


    Real Operators::scProd(ArrayReal &a, ArrayReal &b) {
        Mesh& mesh = Config::getMesh();

        assert( a.size() == b.size() &&
                a.size() == mesh.getCellsCountWithGhost() &&
                "Size of arrays must be equal to cells count"
        );

        Real s = 0.;
        for (Index i = 0; i < mesh.getCellsCount(); i++) {
            s += a[i]*b[i];
        }

        Parallel::sum(&s);
        return s;
    }


    void Operators::add(ArrayReal &a, ArrayReal &b) {
        Mesh& mesh = Config::getMesh();

        assert( a.size() == b.size() &&
                a.size() == mesh.getCellsCountWithGhost() &&
                "Size of arrays must be equal to cells count"
        );
        for (Index i = 0; i < mesh.getCellsCount(); i++) {
            a[i] += b[i];
        }
        Parallel::exchange(a);
    }


    void Operators::sub(ArrayReal &a, ArrayReal &b) {
        Mesh& mesh = Config::getMesh();

        assert( a.size() == b.size() &&
                a.size() == mesh.getCellsCountWithGhost() &&
                "Size of arrays must be equal to cells count"
        );
        for (Index i = 0; i < mesh.getCellsCount(); i++) {
            a[i] -= b[i];
        }
        Parallel::exchange(a);
    }


    Real Operators::norm(ArrayReal &a) {
        Mesh& mesh = Config::getMesh();

        assert( a.size() == mesh.getCellsCountWithGhost() &&
                "Size of arrays must be equal to cells count"
        );
        return sqrt(scProd(a, a));
    }


    void Operators::div(charm::ArrayReal &out, charm::ArrayVector &in) {
        Mesh& mesh = Config::getMesh();

        assert(
                in.size() == out.size() &&
                in.size() == mesh.getCellsCountWithGhost() &&
                "Size of arrays must be equal to cells count"
        );

        for (Index iCell = 0; iCell < mesh.getCellsCountWithGhost(); iCell++) {
            out[iCell] = 0.;
        }

        for (Index iFace = 0; iFace < mesh.getFacesCount(); iFace++) {
            Face &face = mesh.getFace(iFace);
            Vector n = face.n;
            bool isBnd = face.cells.size() == 1;
            Index c1 = face.cells[0];
            Index c2;
            Real gn1 = scalarProd(in[c1], n);
            Real gn2;


            Real vol1 = mesh.getCellVolume(c1);//cells[c1].volume;
            Real vol2;

            if (isBnd) {
                gn2 = gn1; // @todo ?????
                vol2 = vol1;
            } else {
                c2 = face.cells[1];
                gn2 = scalarProd(in[c2], n);
                vol2 = mesh.getCellVolume(c2);//cells[c2].volume;
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

        Parallel::exchange(out);

    }

    void Operators::grad(ArrayVector &out, ArrayReal &in) {
        Mesh& mesh = Config::getMesh();

        assert(
                in.size() == out.size() &&
                in.size() == mesh.getCellsCountWithGhost() &&
                "Size of arrays must be equal to cells count"
        );

        for (auto &item : out) {
            item.x = item.y = item.z = 0.;
        }

        for (Index iFace = 0; iFace < mesh.getFacesCount(); iFace++) {
            Face &face = mesh.getFace(iFace);
            Vector n = face.n;
            bool isBnd = face.cells.size() == 1;
            Index c1 = face.cells[0];
            Index c2;
            Real p1 = in[c1];
            Real p2;

            Real vol1 = mesh.getCellVolume(c1);
            Real vol2;

            if (isBnd) {
                p2 = p1; // @todo ?????
                vol2 = vol1;
            } else {
                c2 = face.cells[1];
                p2 = in[c2];
                vol2 = mesh.getCellVolume(c2);
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

        for (Index iCell = 0; iCell < mesh.getCellsCount(); iCell++) {
            Real vol = mesh.getCellVolume(iCell);
            out[iCell] /= vol;
        }

        Parallel::exchange(out);


    }
}