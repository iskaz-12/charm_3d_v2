/*
 *     ________  _____    ____  __  ___    _____ ____
 *    / ____/ / / /   |  / __ \/  |/  /   |__  // __ \  __    __
 *   / /   / /_/ / /| | / /_/ / /|_/ /     /_ </ / / /_/ /___/ /_
 *  / /___/ __  / ___ |/ _, _/ /  / /    ___/ / /_/ /_  __/_  __/
 *  \____/_/ /_/_/  |_/_/ |_/_/  /_/____/____/_____/ /_/   /_/
 *
 */


#ifndef CHARM_3D_V2_REGION_H
#define CHARM_3D_V2_REGION_H
#include "Point.h"


namespace charm {

    class Region {
    public:
        String      name;
        Index       id;
        Index       matId;
        Vector      v;
        Real        t;
        Real        p;
        ArrayReal   c;
        Vector      grav;

    };


}

/*


typedef struct charm_reg
{
} charm_reg_t;



 */
#endif //CHARM_3D_V2_REGION_H
