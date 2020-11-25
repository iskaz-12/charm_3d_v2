/**
 * Created by zhrv on 23.11.2020.
 * @author R.V.Zhalnin, zhalnin@gmail.com
 */

#ifndef CHARM_3D_V2_EOSEXCEPTION_H
#define CHARM_3D_V2_EOSEXCEPTION_H

#include <global.h>

namespace charm {
    class EosException: public Exception {
    public:
        EosException(String msg): Exception(msg) {}
    };
}

#endif //CHARM_3D_V2_EOSEXCEPTION_H
