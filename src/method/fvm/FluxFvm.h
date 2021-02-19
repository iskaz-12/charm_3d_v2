//
// Created by zhrv on 20.11.2020.
//

#ifndef CHARM_3D_V2_FLUXFVM_H
#define CHARM_3D_V2_FLUXFVM_H

#include <Point.h>
#include <Prim.h>

namespace charm {
    class FluxFvm {
    public:
        virtual void calc(const Prim &prim1, const Prim &prim2, Real &ru, Real &rv, Real &rw, Real &re, ArrayReal &rc, const Vector &n) = 0;
    };
}

#endif //CHARM_3D_V2_FLUXFVM_H
