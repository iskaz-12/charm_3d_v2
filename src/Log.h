/*
 *     ________  _____    ____  __  ___    _____ ____
 *    / ____/ / / /   |  / __ \/  |/  /   |__  // __ \  __    __
 *   / /   / /_/ / /| | / /_/ / /|_/ /     /_ </ / / /_/ /___/ /_
 *  / /___/ __  / ___ |/ _, _/ /  / /    ___/ / /_/ /_  __/_  __/
 *  \____/_/ /_/_/  |_/_/ |_/_/  /_/____/____/_____/ /_/   /_/
 *
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
