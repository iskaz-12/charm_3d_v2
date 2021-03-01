/*
 *     ________  _____    ____  __  ___    _____ ____
 *    / ____/ / / /   |  / __ \/  |/  /   |__  // __ \  __    __
 *   / /   / /_/ / /| | / /_/ / /|_/ /     /_ </ / / /_/ /___/ /_
 *  / /___/ __  / ___ |/ _, _/ /  / /    ___/ / /_/ /_  __/_  __/
 *  \____/_/ /_/_/  |_/_/ |_/_/  /_/____/____/_____/ /_/   /_/
 *
 */

#ifndef CHARM_3D_V2_OPERATORS_H
#define CHARM_3D_V2_OPERATORS_H

#include <Mesh.h>
#include "global.h"

namespace charm {

    class Operators {
    public:
        static void div(ArrayReal &out, ArrayVector &in);
        static void grad(ArrayVector &out, ArrayReal &in);

        static void copy(ArrayReal &dest, ArrayReal &src);

        static void mult(ArrayReal &a, Real b);

        static Real scProd(ArrayReal &a, ArrayReal &b);

        static void add(ArrayReal &a, ArrayReal &b);

        static void sub(ArrayReal &a, ArrayReal &b);

        static Real norm(ArrayReal &a);
    };

}

#endif //CHARM_3D_V2_OPERATORS_H
