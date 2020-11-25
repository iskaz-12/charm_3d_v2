//
// Created by zhrv on 13.11.2020.
//

#include "MeshReader.h"
#include "MeshReaderMsh22ASCII.h"
namespace charm {
    MeshReader *MeshReader::create(Config *config) {
        // @todo
        return new MeshReaderMsh22ASCII(config);
    }
}