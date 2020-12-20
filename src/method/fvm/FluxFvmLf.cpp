//
// Created by zhrv on 22.11.2020.
//

#include "FluxFvmLf.h"
#include "Config.h"

namespace charm{

    using Cons = DataFvm::Cons;

    void FluxFvmLf::calc(const Prim &_prim1, const Prim &_prim2, Cons &flux, const Vector &n) {
        
        Index cCount = Config::getCompCount();
        Index  fCount = 4+cCount;
        ArrayReal vn(2);
        Array<ArrayReal> ff(fCount, ArrayReal(2));
        Array<ArrayReal> uu(fCount, ArrayReal(2));
        Array<Real*> q(fCount);

        Array<std::shared_ptr<Prim>> prim;
        prim.push_back(std::make_shared<Prim>(_prim1));
        prim.push_back(std::make_shared<Prim>(_prim2));


        q[0] = &flux.ru;
        q[1] = &flux.rv;
        q[2] = &flux.rw;
        q[3] = &flux.re;
        for (Index i = 4; i < fCount; i++) {
            q[i] = &(flux.rc[i-4]);
        }


        for (Index i = 0; i < 2; i++) {
            vn[i] = scalarProd(prim[i]->v, n);//prim[i]->v.x*n[0]+prim[i]->v.y*n[1]+prim[i]->v.z*n[2];

            uu[0][i] = prim[i]->r*prim[i]->v.x;
            uu[1][i] = prim[i]->r*prim[i]->v.y;
            uu[2][i] = prim[i]->r*prim[i]->v.z;
            uu[3][i] = prim[i]->r*prim[i]->eTot;

            ff[0][i] = prim[i]->r*vn[i]*prim[i]->v.x+prim[i]->p*n[0];
            ff[1][i] = prim[i]->r*vn[i]*prim[i]->v.y+prim[i]->p*n[1];
            ff[2][i] = prim[i]->r*vn[i]*prim[i]->v.z+prim[i]->p*n[2];
            ff[3][i] = (prim[i]->r*prim[i]->eTot+prim[i]->p)*vn[i];

            for (Index j = 4; j < fCount; j++) {
                uu[j][i] = prim[i]->r*prim[i]->c[j-4];
                ff[j][i] = prim[i]->r*vn[i]*prim[i]->c[j-4];
            }

        }

        Real alpha = std::max(fabs(vn[0])+prim[0]->cz, fabs(vn[1])+prim[1]->cz);

        for (Index i = 0; i < fCount; i++) {
            *(q[i]) = 0.5*( ff[i][1]+ff[i][0]-alpha*(uu[i][1]-uu[i][0]) );
        }

    }
}