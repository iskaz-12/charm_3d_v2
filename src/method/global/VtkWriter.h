/*
 *     ________  _____    ____  __  ___    _____ ____
 *    / ____/ / / /   |  / __ \/  |/  /   |__  // __ \  __    __
 *   / /   / /_/ / /| | / /_/ / /|_/ /     /_ </ / / /_/ /___/ /_
 *  / /___/ __  / ___ |/ _, _/ /  / /    ___/ / /_/ /_  __/_  __/
 *  \____/_/ /_/_/  |_/_/ |_/_/  /_/____/____/_____/ /_/   /_/
 *
 */


#ifndef CHARM_3D_V2_VTKWRITER_H
#define CHARM_3D_V2_VTKWRITER_H

#include "global.h"
#include "Method.h"

namespace charm {

    class VtkWriter {
    public:
        VtkWriter(Method* method);

        void write(Index step);

    protected:
        Method* method;
    };

}


#endif //CHARM_3D_V2_VTKWRITER_H
