//
// Created by zhrv on 20.11.2020.
//

#ifndef CHARM_3D_V2_FLUXFVMHLLC_H
#define CHARM_3D_V2_FLUXFVMHLLC_H

#include <global.h>
#include "DataFvm.h"
#include "FluxFvm.h"

namespace charm {


    class FluxFvmHllc : public FluxFvm {
    public:
        virtual void calc(const Prim &prim1, const Prim &prim2, DataFvm::Cons &flux, const Vector &n);
    private:
        void x_1(const Prim &prim1, const Prim &prim2, DataFvm::Cons &flux);
    };
}

#endif //CHARM_3D_V2_FLUXFVMHLLC_H
