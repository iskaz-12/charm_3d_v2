/**
 * @author R.V.Zhalnin zhalnin@gmail.com
 */

#include <iostream>
#include "Face.h"

namespace charm{

    void Face::addCell(Int cId) {
        if (cells.size() < 2) {
            cells.push_back(cId);
        }
        else {
            std::cerr << "Can't add cell to face!" << std::endl;
            exit(1);
        }
    }


    Face& Face::operator = (const Face& face) {
        nodesInd.resize(face.nodesInd.size());
        nodesInd.assign(face.nodesInd.begin(), face.nodesInd.end());
        cells.assign(face.cells.begin(), face.cells.end());
        id          = face.id;
        tag         = face.tag;
        n           = face.n;
        area        = face.area;
        center      = face.center;
        type        = face.type;
        bnd         = face.bnd;

        //  UPDATE ON 13.06.2023
        //  Нужно восстановить gp и gw для РМГ
        gp.assign(face.gp.begin(), face.gp.end());
        gw.assign(face.gw.begin(), face.gw.end());

        //  ---24.12.2023---
        gj = face.gj;

        return *this;
    }

}