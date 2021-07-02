/*
 *     ________  _____    ____  __  ___    _____ ____
 *    / ____/ / / /   |  / __ \/  |/  /   |__  // __ \  __    __
 *   / /   / /_/ / /| | / /_/ / /|_/ /     /_ </ / / /_/ /___/ /_
 *  / /___/ __  / ___ |/ _, _/ /  / /    ___/ / /_/ /_  __/_  __/
 *  \____/_/ /_/_/  |_/_/ |_/_/  /_/____/____/_____/ /_/   /_/
 *
 */


#ifndef CHARM_3D_V2_MESHREADER_H
#define CHARM_3D_V2_MESHREADER_H
#include "global.h"
#include "Mesh.h"
#include "method/global/Config.h"
#include "metis.h"

namespace charm {
    class MeshReader {
    public:
        static MeshReader* create(Config* config);

        virtual Mesh* read() = 0;

        void decomp(Mesh* mesh);


    protected:

        struct ProcMesh
        {
            Index cCount, cCountEx;
            Index fCount, fCountEx;
            Index nCount, nCountEx;

            ArrayIndex              gCells, gFaces, gNodes;
            std::map<Index, Index>  lCells, lFaces, lNodes;

            ArrayIndex          recvCount;
            Array<ArrayIndex>   sendInd;
        };

        void buildProcMesh(Mesh* mesh, const Array<idx_t> &epart, ProcMesh &procMesh);


    protected:
        //Mesh* mesh;
        String fileName;
        Config* config;
    };
}

#endif //CHARM_3D_V2_MESHREADER_H
