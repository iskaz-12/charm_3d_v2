/**
 * @author R.V.Zhalnin zhalnin@gmail.com
 */

#ifndef CHARM_3D_V2_FACE_H
#define CHARM_3D_V2_FACE_H

#include <BoundaryCondition.h>
#include "global.h"
#include "Point.h"


namespace charm {

    class Mesh;

    class Face {
    public:
        Face() = default;

        ~Face() = default;

        Face(const Face &face) {
            *this = face;
        }

        Face& operator = (const Face& face);

        void addCell(Int cellId);




        Index                   id;
        ArrayIndex              nodesInd;
        ArrayIndex              cells;          // ячейка слева и справа
        Int                     tag = 0;
        Point                   n;              // нормаль к грани
        Real                    area;           // длина грани
        Point                   center;         // центр грани
        Points                  gp;
        ArrayReal               gw;
        Int                     type;           // тип грани (внутр., гранич.)
        Ptr<BoundaryCondition>  bnd = nullptr;

        friend class Mesh;
    };

}
#endif //CHARM_3D_V2_FACE_H
