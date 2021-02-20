/**
 * Created by zhrv on 09.02.2021.
 * @author R.V.Zhalnin, zhalnin@gmail.com
 */

#ifndef CHARM_3D_V2_LOG_H
#define CHARM_3D_V2_LOG_H

#include "global.h"
#include "Parallel.h"

namespace charm {
class Log {
    public:
        static void print(const String &str) { if (Parallel::isRoot()) std::cout << str; }
    };

}

#endif //CHARM_3D_V2_LOG_H
