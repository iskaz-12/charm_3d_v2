//
// Created by zhrv on 13.11.2020.
//

#ifndef CHARM_3D_V2_MESHREADER_H
#define CHARM_3D_V2_MESHREADER_H
#include "global.h"
#include "Mesh.h"
#include "Config.h"


namespace charm {
    class MeshReader {
    public:
        static MeshReader *create(Config *config);  //  UPDATE ON 18.06.2023 - создание сетки по содержимому файла

        virtual Mesh* read() = 0;

    protected:
        //Mesh* mesh;
        String fileName;
        Config *config;
    };
}

#endif //CHARM_3D_V2_MESHREADER_H
