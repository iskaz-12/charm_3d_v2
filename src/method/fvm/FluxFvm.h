//
// Created by zhrv on 20.11.2020.
//

#ifndef CHARM_3D_V2_FLUXFVM_H
#define CHARM_3D_V2_FLUXFVM_H

#include <Point.h>
#include "DataFvm.h"

namespace charm {
    class FluxFvm {
    public:
        virtual void calc(const Prim &prim1, const Prim &prim2, DataFvm::Cons &flux, const Vector &n) = 0;
    };
}

#endif //CHARM_3D_V2_FLUXFVM_H
