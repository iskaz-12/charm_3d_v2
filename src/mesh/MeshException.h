//
// Created by zhrv on 03.01.2021.
//

#ifndef CHARM_3D_V2_MESHEXCEPTION_H
#define CHARM_3D_V2_MESHEXCEPTION_H

#include <global.h>

namespace charm {
    class MeshException : public Exception {
    public:
        MeshException(String msg): Exception(msg) {}
    };
}

#endif //CHARM_3D_V2_MESHEXCEPTION_H
