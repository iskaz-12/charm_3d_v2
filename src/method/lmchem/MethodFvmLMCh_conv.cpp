/*
 *     ________  _____    ____  __  ___    _____ ____
 *    / ____/ / / /   |  / __ \/  |/  /   |__  // __ \  __    __
 *   / /   / /_/ / /| | / /_/ / /|_/ /     /_ </ / / /_/ /___/ /_
 *  / /___/ __  / ___ |/ _, _/ /  / /    ___/ / /_/ /_  __/_  __/
 *  \____/_/ /_/_/  |_/_/ |_/_/  /_/____/____/_____/ /_/   /_/
 *
 */

#include "MethodFvmLMCh.h"


namespace charm{

    void MethodFvmLMCh::calcConv(){
        Mesh& mesh = Config::getMesh();
        Index compCount = Config::getCompCount();
        Index fN = mesh.getFacesCount();

        for (Index iFace = 0; iFace < fN; iFace++) {
            Face &face = mesh.getFace(iFace);
            bool isBnd = face.cells.size() == 1;
            Index c1 = face.cells[0];
            Index c2;
            Prim p1 = getPrim(c1);
            Prim p2(compCount);

            if (isBnd) {
                face.bnd->calc(p1, p2, face.n);
            } else {
                c2 = face.cells[1];
                p2 = getPrim(c2);
            }

            Real flxU, flxV, flxW, flxH;
            ArrayReal flxC(compCount);

            flux->calc(p1, p2, flxU, flxV, flxW, flxH, flxC, face.n);
            ruInt[c1] += flxU * face.area;
            rvInt[c1] += flxV * face.area;
            rwInt[c1] += flxW * face.area;
            rhInt[c1] += flxH * face.area;
            for (Index i = 0; i < compCount; i++) {
                rcInt[c1][i] += flxC[i] * face.area;
            }
            if (!isBnd) {
                ruInt[c2] -= flxU * face.area;
                rvInt[c2] -= flxV * face.area;
                rwInt[c2] -= flxW * face.area;
                rhInt[c2] -= flxH * face.area;
                for (Index i = 0; i < compCount; i++) {
                    rcInt[c2][i] -= flxC[i] * face.area;
                }
            }
        }

    }

}