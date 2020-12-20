//
// Created by zhrv on 22.11.2020.
//

#ifndef CHARM_3D_V2_FLUXFVMLF_H
#define CHARM_3D_V2_FLUXFVMLF_H

#include "FluxFvm.h"

namespace charm {

    class FluxFvmLf: public FluxFvm {
    public:
        virtual void calc(const Prim &prim1, const Prim &prim2, DataFvm::Cons &flux, const Vector &n);

    };

}

#endif //CHARM_3D_V2_FLUXFVMLF_H
