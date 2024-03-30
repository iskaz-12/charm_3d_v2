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

        gp.assign(face.gp.begin(), face.gp.end());
        gw.assign(face.gw.begin(), face.gw.end());

        gj = face.gj;

        return *this;
    }

}