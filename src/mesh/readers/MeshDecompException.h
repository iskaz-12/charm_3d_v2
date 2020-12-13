//
// Created by zhrv on 13.12.2020.
//

#ifndef CHARM_3D_V2_MESHDECOMPEXCEPTION_H
#define CHARM_3D_V2_MESHDECOMPEXCEPTION_H

namespace charm {

    class MeshDecompException : public Exception {
    public:
        MeshDecompException(String msg) : Exception(msg) {}
    };

}

#endif //CHARM_3D_V2_MESHDECOMPEXCEPTION_H
