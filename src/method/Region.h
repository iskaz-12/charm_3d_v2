/**
 * Created by zhrv on 18.11.2020.
 * @author R.V.Zhalnin, zhalnin@gmail.com
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
