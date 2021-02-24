//
// Created by zhrv on 13.11.2020.
//

#ifndef CHARM_3D_V2_MESHREADERMSH22ASCII_H
#define CHARM_3D_V2_MESHREADERMSH22ASCII_H
#include <fstream>
#include "MeshReader.h"
namespace charm {

    struct Patch {
        String name;
        Int id;
        Int dim;
    };


    /**
     * Reader for GMSH format of mesh file (ver.2.2).
     * See gmsh.info
     */
    class MeshReaderMsh22ASCII : public MeshReader {
    public:
        MeshReaderMsh22ASCII(Config* config);

        virtual Mesh* read();

    protected:

    private:

        /*
         * Read a line from a file. Obtained from:
         * http://stackoverflow.com/questions/314401/
         * how-to-read-a-line-from-the-console-in-c/314422#314422
         *
         * Using this avoids a dependence on IEEE Std 1003.1-2008 (``POSIX.1'') for the
         * getline function.
         */
        static String getlineUpper(std::fstream &stream);

        Index               patchesCount = 0;       ///< Number of boundary patches.
        Array<Patch>        patches;

    };

}
#endif //CHARM_3D_V2_MESHREADERMSH22ASCII_H
