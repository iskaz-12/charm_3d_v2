
#ifndef CHARM_3D_V2_MESH_H
#define CHARM_3D_V2_MESH_H
#include "global.h"
#include "Point.h"
#include "Cell.h"
#include "Face.h"

class Mesh {
    std::vector<Point>      nodes;
    std::vector<Cell>       cells;
    std::vector<Face>       faces;
};


#endif //CHARM_3D_V2_MESH_H
