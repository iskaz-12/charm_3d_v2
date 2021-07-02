/*
 *     ________  _____    ____  __  ___    _____ ____
 *    / ____/ / / /   |  / __ \/  |/  /   |__  // __ \  __    __
 *   / /   / /_/ / /| | / /_/ / /|_/ /     /_ </ / / /_/ /___/ /_
 *  / /___/ __  / ___ |/ _, _/ /  / /    ___/ / /_/ /_  __/_  __/
 *  \____/_/ /_/_/  |_/_/ |_/_/  /_/____/____/_____/ /_/   /_/
 *
 */


#ifndef CHARM_3D_V2_CONFIGEXCEPTION_H
#define CHARM_3D_V2_CONFIGEXCEPTION_H

#include <global.h>

#include <utility>
#include "MethodException.h"

namespace charm {
    class ConfigException : public Exception {
    public:
        explicit ConfigException(String message): Exception(std::move(message)) {}
    };
}

#endif //CHARM_3D_V2_CONFIGEXCEPTION_H
