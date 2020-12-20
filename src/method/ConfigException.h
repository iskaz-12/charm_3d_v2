//
// Created by zhrv on 19.11.2020.
//

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
