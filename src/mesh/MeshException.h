/*
 *     ________  _____    ____  __  ___    _____ ____
 *    / ____/ / / /   |  / __ \/  |/  /   |__  // __ \  __    __
 *   / /   / /_/ / /| | / /_/ / /|_/ /     /_ </ / / /_/ /___/ /_
 *  / /___/ __  / ___ |/ _, _/ /  / /    ___/ / /_/ /_  __/_  __/
 *  \____/_/ /_/_/  |_/_/ |_/_/  /_/____/____/_____/ /_/   /_/
 *
 */


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
