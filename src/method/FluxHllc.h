//
// Created by zhrv on 20.11.2020.
//

#ifndef CHARM_3D_V2_FLUXHLLC_H
#define CHARM_3D_V2_FLUXHLLC_H

#include <global.h>
#include "Data.h"
#include "Flux.h"

namespace charm {


    class FluxHllc : public Flux {
    public:
        virtual void calc(const Prim &prim1, const Prim &prim2, Cons &flux, const Vector &n);
    private:
        void x_1(const Prim &prim1, const Prim &prim2, Cons &flux);
    };
}

#endif //CHARM_3D_V2_FLUXHLLC_H
