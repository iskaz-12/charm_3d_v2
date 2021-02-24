/*
 *     ________  _____    ____  __  ___    _____ ____
 *    / ____/ / / /   |  / __ \/  |/  /   |__  // __ \  __    __
 *   / /   / /_/ / /| | / /_/ / /|_/ /     /_ </ / / /_/ /___/ /_
 *  / /___/ __  / ___ |/ _, _/ /  / /    ___/ / /_/ /_  __/_  __/
 *  \____/_/ /_/_/  |_/_/ |_/_/  /_/____/____/_____/ /_/   /_/
 *
 */


/*
 * Created by zhrv on 19.12.2020.
 */

#include <Config.h>
#include "FluxFvmLMChLF.h"

namespace charm {

    void FluxFvmLMChLF::calc(const Prim &_prim1, const Prim &_prim2, Real &ru, Real &rv, Real &rw, Real &rh, ArrayReal &rc, const Vector &n) {

        Index cCount = Config::getCompCount();
        Index fCount = 4 + cCount;
        ArrayReal vn(2);
        Array<ArrayReal> ff(fCount, ArrayReal(2));
        Array<ArrayReal> uu(fCount, ArrayReal(2));
        Array<Real *> q(fCount);

        Array<std::shared_ptr<Prim>> prim;
        prim.push_back(std::make_shared<Prim>(_prim1));
        prim.push_back(std::make_shared<Prim>(_prim2));


        q[0] = &ru;
        q[1] = &rv;
        q[2] = &rw;
        q[3] = &rh;
        for (Index i = 4; i < fCount; i++) {
            q[i] = &(rc[i - 4]);
        }


        for (Index i = 0; i < 2; i++) {
            vn[i] = scalarProd(prim[i]->v, n);//prim[i]->v.x*n[0]+prim[i]->v.y*n[1]+prim[i]->v.z*n[2];

            uu[0][i] = prim[i]->r * prim[i]->v.x;
            uu[1][i] = prim[i]->r * prim[i]->v.y;
            uu[2][i] = prim[i]->r * prim[i]->v.z;
            uu[3][i] = prim[i]->r * prim[i]->h;

            ff[0][i] = prim[i]->r * vn[i] * prim[i]->v.x;
            ff[1][i] = prim[i]->r * vn[i] * prim[i]->v.y;
            ff[2][i] = prim[i]->r * vn[i] * prim[i]->v.z;
            ff[3][i] = prim[i]->r * vn[i] * prim[i]->h;

            for (Index j = 4; j < fCount; j++) {
                uu[j][i] = prim[i]->r * prim[i]->c[j - 4];
                ff[j][i] = prim[i]->r * vn[i] * prim[i]->c[j - 4];
            }

        }

        Real alpha = std::max(fabs(vn[0]), fabs(vn[1]));

        for (Index i = 0; i < fCount; i++) {
            *(q[i]) = 0.5 * (ff[i][1] + ff[i][0] - alpha * (uu[i][1] - uu[i][0]));
        }


    }
}