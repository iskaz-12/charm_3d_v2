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
            dh              = c.dh;     //  UPDATE ON 18.06.2023 - размеры ячейки
            flag            = c.flag;


            //  ---08.03.2024---
            A.assign(c.A.begin(), c.A.end());
            invA.assign(c.invA.begin(), c.invA.end());

            //  ---22.01.24---
            //  Добавляем инициализацию
            /* t.assign(c.t.begin(), c.t.end());
            qx.assign(c.qx.begin(), c.qx.end());
            qy.assign(c.qy.begin(), c.qy.end());
            qz.assign(c.qz.begin(), c.qz.end());           
            A.assign(c.A.begin(), c.A.end());
            invA.assign(c.invA.begin(), c.invA.end());
            intT.assign(c.intT.begin(), c.intT.end());
            intQx.assign(c.intQx.begin(), c.intQx.end());
            intQy.assign(c.intQy.begin(), c.intQy.end());
            intQz.assign(c.intQz.begin(), c.intQz.end());   */

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

        //  ---22.01.24---
        // Добавляем матрицу масс A
        ArrayReal  A;
        ArrayReal  invA;


        friend class Mesh;

        static const Uint CELL_FLAG_GOOD        = 0x000000;
        static const Uint CELL_FLAG_BAD         = 0x000001;
        static const Uint CELL_FLAG_LIM         = 0x000002;

    };
}

#endif //CHARM_3D_V2_CELL_H
