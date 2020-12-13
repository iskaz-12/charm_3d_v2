//
// Created by zhrv on 13.12.2020.
//

#ifndef CHARM_3D_V2_MESHREADEREXCEPTION_H
#define CHARM_3D_V2_MESHREADEREXCEPTION_H

namespace charm {

    class MeshReaderException : public Exception {
    public:
        MeshReaderException(String msg) : Exception(msg) {}
    };

}

#endif //CHARM_3D_V2_MESHREADEREXCEPTION_H
