
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

    const Index Mesh::FTV_PIR[5][4] =
            {{ 0, 1, 3, 2   },
             { 0, 1, 4, 100 },
             { 0, 3, 4, 100 },
             { 1, 2, 4, 100 },
             { 2, 3, 4, 100 }};


    Mesh::Mesh() {
        nCount = nCountGhost = 0;
        fCount = fCountGhost = 0;
        cCount = cCountGhost = 0;
    }


    Point Mesh::unitCubeToReal(Points vertices, Real x, Real y, Real z) {
        Point res = {0, 0, 0};
        Real wx[2], wy[2], wz[2];
        wx[1] = x;
        wx[0] = 1.-wx[1];
        wy[1] = y;
        wy[0] = 1.-wy[1];
        wz[1] = z;
        wz[0] = 1.-wz[1];
        Index n = 0;
        for (Index iz : {0, 1}) {
            for (Index iy : {0, 1}) {
                Real ay = wz[iz]*wy[iy];
                for (Index ix : {0, 1}) {
                    Real ax = ay*wx[ix];
                    Point &node = vertices[n++];
                    res.x += ax*node.x;
                    res.y += ax*node.y;
                    res.z += ax*node.z;
                }
            }
        }

        return res;
    }


    void Mesh::calcGeom() {
        std::set<Index> doneFaces;
        Real l  = 1, l2 = l / 2.;;
        const Point fc[6] = {
                {0, l2, l2},
                {l, l2, l2},
                {l2, 0, l2},
                {l2, l, l2},
                {l2, l2, 0},
                {l2, l2, l},
        };

        for (auto &c : cells) {
            Points vertices = cellGetVertices(c);
            
            if (c.type == Cell::CELL_TYPE_HEXAHEDRON) {

                c.center = unitCubeToReal(vertices, 0.5, 0.5, 0.5);

                Real sqrt3 = 1. / sqrt(3.);
                Point t[8] = {
                        {-sqrt3, -sqrt3, -sqrt3},
                        {sqrt3,  -sqrt3, -sqrt3},
                        {-sqrt3, sqrt3,  -sqrt3},
                        {sqrt3,  sqrt3,  -sqrt3},
                        {-sqrt3, -sqrt3, sqrt3},
                        {sqrt3,  -sqrt3, sqrt3},
                        {-sqrt3, sqrt3,  sqrt3},
                        {sqrt3,  sqrt3,  sqrt3}
                };

                for (auto &i : t) {
                    Point gp;
                    Real gw;
                    cellCalcGpAtPoint(vertices, i, gp, gw);
                    c.gp.push_back(gp);
                    c.gw.push_back(gw);
                }

                for (Index fi = 0; fi < 6; fi++) {
                    Index iFace = c.facesInd[fi];
                    if (doneFaces.find(iFace) == doneFaces.end()) {
                        doneFaces.insert(iFace);
                        Face &face = faces[iFace];
                        face.center = unitCubeToReal(vertices, fc[fi].x, fc[fi].y, fc[fi].z);

                        Real nl;
                        Point v[2];
                        Points fv = cellGetFaceVertices(c, fi);

                        v[0] = fv[1];
                        v[0] -= fv[0];
                        v[1] = fv[2];
                        v[1] -= fv[0];


                        face.n = vectorProd(v[0], v[1]);
                        nl = face.n.length();
                        face.n /= nl;

                        Vector p0 = cells[face.cells[0]].center;
                        p0 -= face.center;
                        if (scalarProd(p0, face.n) > 0) {
                            face.n *= -1.;
                        }

                        Point pt[3];
                        Real a = 2.0 / 3.0;
                        Real b = 1.0 / 6.0;


                        for (Index i = 0; i < 6; i++) {
                            face.gw.push_back(1. / 6.);
                        }

                        face.gp.resize(6); // @todo
                        for (Index i = 0; i < 2; i++) {
                            for (Index j = 0; j < 3; j++) {
                                pt[j] = fv[i + j];
                            }

                            Real a1 = pt[0][0] - pt[2][0];
                            Real a2 = pt[1][0] - pt[2][0];
                            Real a3 = pt[2][0];
                            Real b1 = pt[0][1] - pt[2][1];
                            Real b2 = pt[1][1] - pt[2][1];
                            Real b3 = pt[2][1];
                            Real c1 = pt[0][2] - pt[2][2];
                            Real c2 = pt[1][2] - pt[2][2];
                            Real c3 = pt[2][2];

                            Real E = a1 * a1 + b1 * b1 + c1 * c1;
                            Real F = a2 * a2 + b2 * b2 + c2 * c2;
                            Real G = a1 * a2 + b1 * b2 + c1 * c2;

                            Real gj = sqrt(E * F - G * G);
                            for (Index j = 0; j < 3; j++) {
                                face.gw[3 * i + j] *= gj;
                            }

                            face.gp[3 * i + 0][0] = a * a1 + b * a2 + a3;
                            face.gp[3 * i + 0][1] = a * b1 + b * b2 + b3;
                            face.gp[3 * i + 0][2] = a * c1 + b * c2 + c3;

                            face.gp[3 * i + 1][0] = b * a1 + a * a2 + a3;
                            face.gp[3 * i + 1][1] = b * b1 + a * b2 + b3;
                            face.gp[3 * i + 1][2] = b * c1 + a * c2 + c3;

                            face.gp[3 * i + 2][0] = b * a1 + b * a2 + a3;
                            face.gp[3 * i + 2][1] = b * b1 + b * b2 + b3;
                            face.gp[3 * i + 2][2] = b * c1 + b * c2 + c3;
                        }
                        face.area = 0.;
                        for (auto gw: face.gw) {
                            face.area += gw;
                        }
                    }
                }
            }
            else if (c.type == Cell::CELL_TYPE_TETRAHEDRON) {

                c.center = {0, 0, 0};
                for (auto &v : vertices) {
                    c.center += v;
                }
                c.center /= 4.;


                Real a = 0.585410196624969;
                Real b = 0.138196601125011;
                Real x, y, z;
                Real a1 = vertices[0].x - vertices[3].x;	Real a2 = vertices[1].x - vertices[3].x;	Real a3 = vertices[2].x - vertices[3].x;	Real a4 = vertices[3].x;
                Real b1 = vertices[0].y - vertices[3].y;	Real b2 = vertices[1].y - vertices[3].y;	Real b3 = vertices[2].y - vertices[3].y;	Real b4 = vertices[3].y;
                Real c1 = vertices[0].z - vertices[3].z;	Real c2 = vertices[1].z - vertices[3].z;	Real c3 = vertices[2].z - vertices[3].z;	Real c4 = vertices[3].z;

                Real fJ = ::fabs(a1 * (b2*c3 - b3*c2) - a2 * (b1 * c3 - b3*c1) + a3 * (b1 * c2 - b2 * c1));
                fJ *= 0.25;
                fJ /= 6.;
                c.gw.resize(4, fJ);
                c.gp.resize(4);

                // 1 точка
                x = a * a1 + b * a2 + b * a3 + a4;
                y = a * b1 + b * b2 + b * b3 + b4;
                z = a * c1 + b * c2 + b * c3 + c4;

                c.gp[0] = {x, y, z};

                // 2 точка
                x = b * a1 + a * a2 + b * a3 + a4;
                y = b * b1 + a * b2 + b * b3 + b4;
                z = b * c1 + a * c2 + b * c3 + c4;

                c.gp[1] = {x, y, z};

                // 3 точка
                x = b * a1 + b * a2 + a * a3 + a4;
                y = b * b1 + b * b2 + a * b3 + b4;
                z = b * c1 + b * c2 + a * c3 + c4;

                c.gp[2] = {x, y, z};

                // 4 точка
                x = b * a1 + b * a2 + b * a3 + a4;
                y = b * b1 + b * b2 + b * b3 + b4;
                z = b * c1 + b * c2 + b * c3 + c4;

                c.gp[3] = {x, y, z};

                for (Index fi = 0; fi < 4; fi++) {
                    Index iFace = c.facesInd[fi];
                    if (doneFaces.find(iFace) == doneFaces.end()) {
                        doneFaces.insert(iFace);
                        Face &face = faces[iFace];


                        Real nl;
                        Point v[2];
                        Points fv = cellGetFaceVertices(c, fi);

                        face.center = {0, 0, 0};
                        for (auto &pt : fv) {
                            face.center += pt;
                        }
                        face.center /= 3.;


                        v[0] = fv[1];
                        v[0] -= fv[0];
                        v[1] = fv[2];
                        v[1] -= fv[0];


                        face.n = vectorProd(v[0], v[1]);
                        nl = face.n.length();
                        face.n /= nl;

                        Vector p0 = cells[face.cells[0]].center;
                        p0 -= face.center;
                        if (scalarProd(p0, face.n) > 0) {
                            face.n *= -1.;
                        }

                        Points &pt = vertices;
                        a = 2.0 / 3.0;
                        b = 1.0 / 6.0;


                        for (Index i = 0; i < 3; i++) {
                            face.gw.push_back(1. / 6.);
                        }

                        face.gp.resize(3); // @todo

                        a1 = pt[0][0] - pt[2][0];
                        a2 = pt[1][0] - pt[2][0];
                        a3 = pt[2][0];
                        b1 = pt[0][1] - pt[2][1];
                        b2 = pt[1][1] - pt[2][1];
                        b3 = pt[2][1];
                        c1 = pt[0][2] - pt[2][2];
                        c2 = pt[1][2] - pt[2][2];
                        c3 = pt[2][2];

                        Real E = a1 * a1 + b1 * b1 + c1 * c1;
                        Real F = a2 * a2 + b2 * b2 + c2 * c2;
                        Real G = a1 * a2 + b1 * b2 + c1 * c2;

                        Real gj = sqrt(E * F - G * G);
                        for (Index j = 0; j < 3; j++) {
                            face.gw[j] *= gj;
                        }

                        face.gp[0][0] = a * a1 + b * a2 + a3;
                        face.gp[0][1] = a * b1 + b * b2 + b3;
                        face.gp[0][2] = a * c1 + b * c2 + c3;

                        face.gp[1][0] = b * a1 + a * a2 + a3;
                        face.gp[1][1] = b * b1 + a * b2 + b3;
                        face.gp[1][2] = b * c1 + a * c2 + c3;

                        face.gp[2][0] = b * a1 + b * a2 + a3;
                        face.gp[2][1] = b * b1 + b * b2 + b3;
                        face.gp[2][2] = b * c1 + b * c2 + c3;

                        face.area = 0.;
                        for (auto gw: face.gw) {
                            face.area += gw;
                        }
                    }
                }
            }

            c.volume = 0.;
            for (auto gw : c.gw) {
                c.volume += gw;
            }


            Point vmin, vmax;
            for (Index j = 0; j < 3; j++) {
                vmin[j] = vertices[0][j];
                vmax[j] = vertices[0][j];
            }
            for (Index i = 1; i < 8; i++) {
                for (Index j = 0; j < 3; j++) {
                    if (vmin[j] > vertices[i][j]) vmin[j] = vertices[i][j];
                    if (vmax[j] < vertices[i][j]) vmax[j] = vertices[i][j];
                }
            }
            for (Index j = 0; j < 3; j++) {
                c.dh[j] = vmax[j] - vmin[j];
            }

        }
        assert (doneFaces.size() == faces.size() && "Wrong faces number.");
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

    void Mesh::cellCalcGpAtPoint(Points vertices, Point ref_p, Point &gp, Real &gj) {
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
        Points vertices;
        Face &f = faces[c.facesInd[fi]];
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