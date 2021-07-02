/*
 *     ________  _____    ____  __  ___    _____ ____
 *    / ____/ / / /   |  / __ \/  |/  /   |__  // __ \  __    __
 *   / /   / /_/ / /| | / /_/ / /|_/ /     /_ </ / / /_/ /___/ /_
 *  / /___/ __  / ___ |/ _, _/ /  / /    ___/ / /_/ /_  __/_  __/
 *  \____/_/ /_/_/  |_/_/ |_/_/  /_/____/____/_____/ /_/   /_/
 *
 */


#ifndef CHARM_3D_V2_FLUXFVMHLLC_H
#define CHARM_3D_V2_FLUXFVMHLLC_H

#include "global.h"
#include "Prim.h"
#include "FluxFvm.h"

namespace charm {


    class FluxFvmHllc : public FluxFvm {
    public:
        virtual void calc(const Prim &prim1, const Prim &prim2, Real &ru, Real &rv, Real &rw, Real &re, ArrayReal &ro, const Vector &n);
    private:
        void x_1(const Prim &prim1, const Prim &prim2, Real &ru, Real &rv, Real &rw, Real &re, ArrayReal &ro);
    };
}

#endif //CHARM_3D_V2_FLUXFVMHLLC_H
