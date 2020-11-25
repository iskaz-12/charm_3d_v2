//
// Created by zhrv on 22.11.2020.
//

#ifndef CHARM_3D_V2_FLUXLF_H
#define CHARM_3D_V2_FLUXLF_H

#include "Flux.h"

namespace charm {

    class FluxLf: public Flux {
    public:
        virtual void calc(const Prim &prim1, const Prim &prim2, Cons &flux, const Vector &n);

    };

}

#endif //CHARM_3D_V2_FLUXLF_H
