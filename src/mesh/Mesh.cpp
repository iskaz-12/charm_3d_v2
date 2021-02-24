#include "global.h"
#include "Mesh.h"
#include "MeshException.h"
#include <cmath>
#include <set>
#include <cassert>


namespace charm{

    const Index Mesh::FTV_QUAD[6][4] =
            {{ 0, 2, 4, 6 },
             { 1, 3, 5, 7 },
             { 0, 1, 4, 5 },
             { 2, 3, 6, 7 },
             { 0, 1, 2, 3 },
             { 4, 5, 6, 7 }};

    const Index Mesh::FTV_TET[4][3] =
            {{ 0, 1, 2 },
             { 1, 2, 3 },
             { 0, 1, 3 },
             { 0, 2, 3 }};

    const Index Mesh::FTV_PRISM[5][4] =
            {{ 0, 1, 2, 100 },
             { 3, 4, 5, 100 },
             { 0, 1, 3, 4   },
             { 1, 2, 4, 5   },
             { 0, 2, 3, 5   }};

    const Index Mesh::FTV_PYR[5][4] =
            {{ 0, 1, 3, 2   },
             { 0, 1, 4, 100 },
             { 0, 3, 4, 100 },
             { 1, 2, 4, 100 },
             { 2, 3, 4, 100 }};


    const Index HEX_TO_TET[6][4] =
            {{0, 1, 4, 6},
             {1, 5, 4, 6},
             {1, 5, 6, 7},
             {1, 3, 6, 7},
             {1, 3, 2, 6},
             {0, 1, 2, 6}};

    const Index PRISM_TO_TET[3][4] =
            {{0, 1, 2, 4},
             {0, 3, 2, 4},
             {5, 3, 2, 4}};

    const Index PYR_TO_TET[2][4] =
            {{0, 1, 3, 4},
             {1, 3, 2, 4}};


    Real calcTetVolume(Points &pt) {
        assert(pt.size() == 4 && "Points count must be equal to 4.");

        Vector v1, v2, v3;
        v1 = pt[1]; v1 -= pt[0];
        v2 = pt[2]; v2 -= pt[0];
        v3 = pt[3]; v3 -= pt[0];
        return fabs(mixProd(v1, v2, v3)) / 6.;
    }


    Point calcTetCenter(Points &pt) {
        assert(pt.size() == 4 && "Points count must be equal to 4.");

        Vector v1 = {0., 0., 0.};
        v1 += pt[0];
        v1 += pt[1];
        v1 += pt[2];
        v1 += pt[3];
        v1 /= 4.;
        return v1;
    }


    Point calcTriCenter(Points &pt) {
        assert(pt.size() == 3 && "Points count must be equal to 3.");

        Vector v1 = {0., 0., 0.};
        v1 += pt[0];
        v1 += pt[1];
        v1 += pt[2];
        v1 /= 3.;
        return v1;
    }


    Real calcTriArea(Points &pt, Vector &n) {
        assert(pt.size() == 3 && "Points count must be equal to 3.");

        Vector v1, v2;
        v1 = pt[1]; v1 -= pt[0];
        v2 = pt[2]; v2 -= pt[0];
        n = vectorProd(v1, v2);
        Real area = n.length();
        n /= area;
        return fabs(area) / 2.;
    }


    Mesh::Mesh() {
        nCount = nCountGhost = 0;
        fCount = fCountGhost = 0;
        cCount = cCountGhost = 0;
    }


    void Mesh::calcGeom() {
        for (auto &c : cells) {
            Points vertices = cellGetVertices(c);

            if (c.type == Cell::CELL_TYPE_HEXAHEDRON) {
                c.center = {0., 0., 0.};
                c.volume = 0.;
                for (auto &ti: HEX_TO_TET) {
                    Points tet;
                    for (auto &p : ti) {
                        tet.push_back(vertices[p]);
                    }
                    Point tc = calcTetCenter(tet);
                    Real vol = calcTetVolume(tet);
                    tc *= vol;
                    c.center += tc;
                    c.volume += vol;
                }
                c.center /= c.volume;
            }
            else if (c.type == Cell::CELL_TYPE_PRISM) {
                c.center = {0., 0., 0.};
                c.volume = 0.;
                for (auto &ti: PRISM_TO_TET) {
                    Points tet;
                    for (auto &p : ti) {
                        tet.push_back(vertices[p]);
                    }
                    Point tc = calcTetCenter(tet);
                    Real vol = calcTetVolume(tet);
                    tc *= vol;
                    c.center += tc;
                    c.volume += vol;
                }
                c.center /= c.volume;
            }
            else if (c.type == Cell::CELL_TYPE_PYRAMID) {
                c.center = {0., 0., 0.};
                c.volume = 0.;
                for (auto &ti: PYR_TO_TET) {
                    Points tet;
                    for (auto &p : ti) {
                        tet.push_back(vertices[p]);
                    }
                    Point tc = calcTetCenter(tet);
                    Real vol = calcTetVolume(tet);
                    tc *= vol;
                    c.center += tc;
                    c.volume += vol;
                }
                c.center /= c.volume;
            }
            else if (c.type == Cell::CELL_TYPE_TETRAHEDRON) {
                c.center = calcTetCenter(vertices);
                c.volume = calcTetVolume(vertices);
            }
            else {
                throw MeshException("Mesh geometry: not realized cell type.");
            }
        }

        for (auto &face : faces) {
            Points fv = faceGetVertices(face);
            if (face.type == Face::FACE_TYPE_QUADRANGLE) {
                face.center = {0., 0., 0.};
                Points pt(3);
                face.area = 0.;
                for (Int i = 0; i < 2; i++) {
                    for (Int j = 0; j < 3; j++) {
                        pt[j] = fv[i+j];
                    }
                    Real a = calcTriArea(pt, face.n);
                    Point ct = calcTriCenter(pt);
                    ct *= a;
                    face.center += ct;
                    face.area += a;
                }
                face.center /= face.area;

                Vector p0 = cells[face.cells[0]].center;
                p0 -= face.center;
                if (scalarProd(p0, face.n) > 0) {
                    face.n *= -1.;
                }
            }
            else if (face.type == Face::FACE_TYPE_TRIANGLE) {
                face.center = calcTriCenter(fv);
                face.area = calcTriArea(fv, face.n);

                Vector p0 = cells[face.cells[0]].center;
                p0 -= face.center;
                if (scalarProd(p0, face.n) > 0) {
                    face.n *= -1.;
                }
            }
            else {
                throw MeshException("Mesh geometry: not realized face type.");
            }
        }
    }

    Points Mesh::cellGetVertices(Cell &c) {
        Points vertices;
        for (auto i : c.nodesInd) {
            vertices.push_back(nodes[i]);
        }
        return vertices;
    }

    Points Mesh::cellGetVertices(Index ic) {
        return cellGetVertices(cells[ic]);
    }

    void Mesh::cellHexCalcGpAtPoint(Points vertices, Point ref_p, Point &gp, Real &gj) {
        Index                 ix, iy, iz, vindex;
        Real              wx[2], wy[2], wz[2];
        Real              xfactor, yfactor;
        Real              jacob[3][3], a;

        wx[0] = 1.-ref_p[0];
        wx[1] = 1.+ref_p[0];
        wy[0] = 1.-ref_p[1];
        wy[1] = 1.+ref_p[1];
        wz[0] = 1.-ref_p[2];
        wz[1] = 1.+ref_p[2];
        gp[0] = gp[1] = gp[2] = 0.;
        vindex = 0;
        for (iz = 0; iz < 2; iz++) {
            for (iy = 0; iy < 2; iy++) {
                yfactor = wz[iz]*wy[iy];
                for (ix = 0; ix < 2; ix++) {
                    xfactor = yfactor*wx[ix]/8.;
                    gp[0] += xfactor * vertices[vindex][0];
                    gp[1] += xfactor * vertices[vindex][1];
                    gp[2] += xfactor * vertices[vindex][2];
                    vindex++;
                }
            }
        }

        // d/dksi
        wx[0] = -1.;
        wx[1] =  1.;
        jacob[0][0] = jacob[1][0] = jacob[2][0] = 0.;
        vindex = 0;
        for (iz = 0; iz < 2; iz++) {
            for (iy = 0; iy < 2; iy++) {
                yfactor = wz[iz]*wy[iy];
                for (ix = 0; ix < 2; ix++) {
                    xfactor = yfactor*wx[ix]/8.;
                    jacob[0][0] += xfactor * vertices[vindex][0];
                    jacob[1][0] += xfactor * vertices[vindex][1];
                    jacob[2][0] += xfactor * vertices[vindex][2];
                    vindex++;
                }
            }
        }

        // d/deta
        wx[0] = 1.-ref_p[0];
        wx[1] = 1.+ref_p[0];
        wy[0] = -1.;
        wy[1] =  1.;
        jacob[0][1] = jacob[1][1] = jacob[2][1] = 0.;
        vindex = 0;
        for (iz = 0; iz < 2; iz++) {
            for (iy = 0; iy < 2; iy++) {
                yfactor = wz[iz]*wy[iy];
                for (ix = 0; ix < 2; ix++) {
                    xfactor = yfactor*wx[ix]/8.;
                    jacob[0][1] += xfactor * vertices[vindex][0];
                    jacob[1][1] += xfactor * vertices[vindex][1];
                    jacob[2][1] += xfactor * vertices[vindex][2];
                    vindex++;
                }
            }
        }

        // d/dmu
        wy[0] = 1.-ref_p[1];
        wy[1] = 1.+ref_p[1];
        wz[0] = -1.;
        wz[1] =  1.;
        jacob[0][2] = jacob[1][2] = jacob[2][2] = 0.;
        vindex = 0;
        for (iz = 0; iz < 2; iz++) {
            for (iy = 0; iy < 2; iy++) {
                yfactor = wz[iz]*wy[iy];
                for (ix = 0; ix < 2; ix++) {
                    xfactor = yfactor*wx[ix]/8.;
                    jacob[0][2] += xfactor * vertices[vindex][0];
                    jacob[1][2] += xfactor * vertices[vindex][1];
                    jacob[2][2] += xfactor * vertices[vindex][2];
                    vindex++;
                }
            }
        }

        gj = matr3Det(jacob);
        //CHARM_ASSERT(*gj != 0.);
    }


    Points Mesh::cellGetFaceVertices(Cell &c, Index fi) {
        return faceGetVertices(faces[c.facesInd[fi]]);
    }


    Points Mesh::faceGetVertices(Face &f) {
        Points vertices;
        for (auto i : f.nodesInd) {
            vertices.push_back(nodes[i]);
        }
        return vertices;
    }


    Points Mesh::cellGetFaceVertices(Index ic, Index fi) {
        return cellGetFaceVertices(cells[ic], fi);
    }


    ArrayIndex Mesh::cellGetNeighIndexes(Index ci) {
        Cell &cell = cells[ci];
        ArrayIndex res;
        for (auto fi : cell.facesInd) {
            Face &face = faces[fi];
            if (face.cells.size() == 2) {
                if (face.cells[0] != ci) {
                    res.push_back(face.cells[0]);
                }
                else {
                    res.push_back(face.cells[1]);
                }
            }
        }
        return res;
    }


    Mesh::FileType Mesh::getfileTypeByStr(String str)
    {
        if (str == "gmsh_msh") {
            return Mesh::GMSH_MSH;
        }
        else if (str == "gmsh_unv") {
            return Mesh::GMSH_UNV;
        }
        else if (str == "gmsh_inp") {
            return Mesh::GMSH_INP;
        }
        else {
            return Mesh::UNKNOWN;
        }
    }

    void Mesh::assign(const Mesh &msh) {
        this->nCount    = msh.nCount;
        this->nCountGhost  = msh.nCountGhost;
        this->nodes.assign(msh.nodes.begin(), msh.nodes.end());

        this->fCount    = msh.fCount;
        this->fCountGhost  = msh.fCountGhost;
        this->faces.assign(msh.faces.begin(), msh.faces.end());

        this->cCount    = msh.cCount;
        this->cCountGhost  = msh.cCountGhost;
        this->cells.assign(msh.cells.begin(), msh.cells.end());

        this->recvCount.assign(msh.recvCount.begin(), msh.recvCount.end());
        this->recvShift.assign(msh.recvShift.begin(), msh.recvShift.end());
        this->sendInd.resize(msh.sendInd.size());
        for (Index i = 0; i < this->sendInd.size(); i++) {
            this->sendInd[i].assign(msh.sendInd[i].begin(), msh.sendInd[i].end());
        }
    }

    Real Mesh::getCellVolume(Index iCell) {
        return getCell(iCell).volume;
    }

    Cell &Mesh::getCell(Index iCell) {
        assert(iCell < cCountGhost && "Wrong cell number for getCell().");
        return cells[iCell];
    }

    Point &Mesh::getNode(Index iNode) {
        assert(iNode < nCountGhost && "Wrong node number for getNode().");
        return nodes[iNode];
    }

    Face &Mesh::getFace(Index iFace) {
        assert(iFace < fCountGhost && "Wrong face number for getFace().");
        return faces[iFace];
    }

    Real Mesh::getFaceArea(Index iFace) {
        return getFace(iFace).area;
    }


}