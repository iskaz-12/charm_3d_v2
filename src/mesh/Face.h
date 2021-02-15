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
//        Points                  gp;
//        ArrayReal               gw;
        Int                     type;           // тип грани (внутр., гранич.)
        BoundaryCondition*      bnd = nullptr;

        friend class Mesh;

    public:
        /**
         *  Triangle:
         *
         *   v
         *   ^
         *   |
         *   2
         *   |`\
         *   |  `\
         *   |    `\
         *   |      `\
         *   |        `\
         *   0----------1 --> u
         */
        static const Uint FACE_TYPE_TRIANGLE   = 2;

        /**
         *
         *  Quadrangle:
         *
         *       v
         *       ^
         *       |
         *   3-----------2
         *   |     |     |
         *   |     |     |
         *   |     +---- | --> u
         *   |           |
         *   |           |
         *   0-----------1
         */
        static const Uint FACE_TYPE_QUADRANGLE = 3;
    };

}
#endif //CHARM_3D_V2_FACE_H
