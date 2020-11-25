/**
 * @author R.V.Zhalnin zhalnin@gmail.com
 */


#ifndef CHARM_3D_V2_CELL_H
#define CHARM_3D_V2_CELL_H

#include "global.h"
#include "Point.h"


namespace charm {

    class Mesh;

    class Cell {
    public:
        Cell() = default;

        Cell(const Cell &c) {
            *this = c;
        }

        ~Cell() = default;

        Cell& operator = (const Cell &c) {
            id              = c.id;
            nodesInd.assign(c.nodesInd.begin(), c.nodesInd.end());
            facesInd.assign(c.facesInd.begin(), c.facesInd.end());
            type            = c.type;
            tag             = c.tag;
            volume          = c.volume;
            center          = c.center;
            gp.assign(c.gp.begin(), c.gp.end());
            gw.assign(c.gw.begin(), c.gw.end());
            dh              = c.dh;
            flag            = c.flag;
            return *this;
        }

        Index               id;
        ArrayIndex          nodesInd;
        ArrayIndex          facesInd;
        Int                 type;
        Int                 tag;
        Real                volume;
        Point               center;
        Points              gp;
        ArrayReal           gw;
        Point               dh;
        Uint                flag;

        friend class Mesh;

        static const Uint CELL_FLAG_GOOD        = 0x000000;
        static const Uint CELL_FLAG_BAD         = 0x000001;
        static const Uint CELL_FLAG_LIM         = 0x000002;

    };
}

#endif //CHARM_3D_V2_CELL_H
