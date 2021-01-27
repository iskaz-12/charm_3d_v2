//
// Created by zhrv on 13.11.2020.
//

#ifndef CHARM_3D_V2_MESHREADER_H
#define CHARM_3D_V2_MESHREADER_H
#include "global.h"
#include "Mesh.h"
#include "Config.h"
#include "metis.h"

namespace charm {
    class MeshReader {
    public:
        static Ptr<MeshReader> create(Ptr<Config> config);

        virtual Ptr<Mesh> read() = 0;

        void decomp(Ptr<Mesh> mesh);


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

        void buildProcMesh(Ptr<Mesh> mesh, const Array<idx_t> &epart, ProcMesh &procMesh);


    protected:
        //Mesh* mesh;
        String fileName;
        Ptr<Config> config;
    };
}

#endif //CHARM_3D_V2_MESHREADER_H
