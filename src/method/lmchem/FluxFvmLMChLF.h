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

#ifndef CHARM_3D_V2_FLUXFVMLMCHLF_H
#define CHARM_3D_V2_FLUXFVMLMCHLF_H

#include "FluxFvmLMCh.h"

namespace charm {
    class FluxFvmLMChLF : public FluxFvmLMCh {
    public:
        virtual void calc(const Prim &prim1, const Prim &prim2, DataFvmLMCh::Cons &flux, const Vector &n);

    };
}

#endif //CHARM_3D_V2_FLUXFVMLMCHLF_H
