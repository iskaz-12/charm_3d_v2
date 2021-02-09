//
// Created by zhrv on 20.11.2020.
//

#ifndef CHARM_3D_V2_FLUXFVMHLLC_H
#define CHARM_3D_V2_FLUXFVMHLLC_H

#include <global.h>
#include "Prim.h"
#include "FluxFvm.h"

namespace charm {


    class FluxFvmHllc : public FluxFvm {
    public:
        virtual void calc(const Prim &prim1, const Prim &prim2, Real &ro, Real &ru, Real &rv, Real &rw, Real &re, const Vector &n);
    private:
        void x_1(const Prim &prim1, const Prim &prim2, Real &ro, Real &ru, Real &rv, Real &rw, Real &re);
    };
}

#endif //CHARM_3D_V2_FLUXFVMHLLC_H
