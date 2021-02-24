//
// Created by zhrv on 19.11.2020.
//

#ifndef CHARM_3D_V2_METHODEXCEPTION_H
#define CHARM_3D_V2_METHODEXCEPTION_H

#include <global.h>

namespace charm {
    class MethodException : public Exception {
    public:
        MethodException(String msg): Exception(msg) {}
    };
}

#endif //CHARM_3D_V2_METHODEXCEPTION_H
