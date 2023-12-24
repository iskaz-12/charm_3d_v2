
#include "Mesh.h"
#include <cmath>
#include <set>
#include <iostream>
#include <ostream>

namespace charm{

    //  UPDATE ON 28.10.2023 - матрица перевода вершин граней в вершины ячейки
    const Index Mesh::ftv[6][4] =
            {{ 0, 2, 4, 6 },
             { 1, 3, 5, 7 },
             { 0, 1, 4, 5 },
             { 2, 3, 6, 7 },
             { 0, 1, 2, 3 },
             { 4, 5, 6, 7 }};


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

        //  UPDATE ON 13.06.2023 - точки на гранях куба
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
            c.center = unitCubeToReal(vertices, 0.5, 0.5, 0.5);

            Real sqrt3 = 1./sqrt(3.);

            //  UPDATE ON 14.06.2023 - коэффициенты для кубического мастер-элемента
            //  см. https://ru.wikipedia.org/wiki/Список_квадратурных_формул
            Point t[8] = {
                    {-sqrt3, -sqrt3, -sqrt3},
                    { sqrt3, -sqrt3, -sqrt3},
                    {-sqrt3,  sqrt3, -sqrt3},
                    { sqrt3,  sqrt3, -sqrt3},
                    {-sqrt3, -sqrt3,  sqrt3},
                    { sqrt3, -sqrt3,  sqrt3},
                    {-sqrt3,  sqrt3,  sqrt3},
                    { sqrt3,  sqrt3,  sqrt3}
            };

            for (auto & i : t) {
                Point gp;
                Real gw;
                cellCalcGpAtPoint(vertices, i, gp, gw);
                c.gp.push_back(gp);
                c.gw.push_back(gw);
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

            for (Index fi = 0; fi < 6; fi++) {
                Index iFace = c.facesInd[fi];
                if (doneFaces.find(iFace) == doneFaces.end()) {
                    doneFaces.insert(iFace);
                    Face &face = faces[iFace];
                    face.center = unitCubeToReal(vertices, fc[fi].x, fc[fi].y, fc[fi].z);

                    Real nl;
                    Point v[2];
                    Points fv = cellGetFaceVertices(c, fi);

                    //  ---23.12.2023---
                    /* for (auto fv_: fv) {
                        std::cout<<fv_[0]<<std::endl;
                    }
                    std::cout<<"---------------"<<std::endl; */

                    v[0] = fv[1]; v[0] -= fv[0];
                    v[1] = fv[2]; v[1] -= fv[0];


                    face.n = vectorProd(v[0], v[1]);
                    nl = face.n.length();
                    face.n /= nl;

                    //  ---24.12.2023---
                    std::cout<<face.cells.size()<<std::endl;
                    std::cout<<face.cells[1]<<std::endl;
                    std::cout<<"---------------"<<std::endl;


                    Vector p0 = cells[face.cells[0]].center;
                    p0 -= face.center;
                    if (scalarProd(p0, face.n) > 0) {
                        face.n *= -1.;
                    }

                    //  UPDATE ON ---23.12.2023---
                    //  В коде charm_3d используется pt[3][3]... Проверить, заработает ли!!!
                    //  Ничего не изменилось!!!

                    //  Point  pt[3];
                    Real  pt[3][3];

                    //  UPDATE ON 18.06.2023 - коэффициенты для формулы Гаусса для треугольных (???) граней
                    //  Скорее всего, одна четырёхугольная грань разбивается на 2 треугольных

                    //  ---23.12.2023---
                    //  СДЕЛАТЬ АНАЛОГ cellCalcGpAtPoint для faces!!!
                    // Стало работать ещё хуже, чем было...


                    Real	a = 2.0/3.0;
                    Real	b = 1.0/6.0;


                    for (Index i = 0; i < 6; i++) {
                        face.gw.push_back(1./6.);
                    }

                    face.gp.resize(6); // @todo
                    for (Index  i = 0; i < 2; i++) {
                        for (Index j = 0; j < 3; j++) {
                            //  UPDATE ON ---23.12.2023---
                            for (Index k = 0; k < 3; k++) {
                                pt[j][k] = fv[i + j][k];
                            }
                            // pt[j] = fv[i + j];
                        }

                        Real a1 = pt[0][0] - pt[2][0];	Real a2 = pt[1][0] - pt[2][0];	Real a3 = pt[2][0];
                        Real b1 = pt[0][1] - pt[2][1];	Real b2 = pt[1][1] - pt[2][1];	Real b3 = pt[2][1];
                        Real c1 = pt[0][2] - pt[2][2];	Real c2 = pt[1][2] - pt[2][2];	Real c3 = pt[2][2];

                        Real E = a1*a1+b1*b1+c1*c1;
                        Real F = a2*a2+b2*b2+c2*c2;
                        Real G = a1*a2+b1*b2+c1*c2;

                        Real gj = sqrt(E*F-G*G);
                        for (Index j = 0; j < 3; j++) {
                            face.gw[3 * i + j] *= gj;
                        }

                        face.gp[3*i+0][0] = a * a1 + b * a2 + a3;
                        face.gp[3*i+0][1] = a * b1 + b * b2 + b3;
                        face.gp[3*i+0][2] = a * c1 + b * c2 + c3;

                        face.gp[3*i+1][0] = b * a1 + a * a2 + a3;
                        face.gp[3*i+1][1] = b * b1 + a * b2 + b3;
                        face.gp[3*i+1][2] = b * c1 + a * c2 + c3;

                        face.gp[3*i+2][0] = b * a1 + b * a2 + a3;
                        face.gp[3*i+2][1] = b * b1 + b * b2 + b3;
                        face.gp[3*i+2][2] = b * c1 + b * c2 + c3;

                        //  ---24.12.2023---
                        face.gj = gj;
                    }
                    face.area = 0.;
                    for (auto gw: face.gw) {
                        face.area += gw;
                    }

                    //  ---24.12.2023---
                    for (Index i = 0; i < 6; i++) {
                        face.gw[i] = 1.0/6.0;
                    }


                    /* // Real v[8][3];
                    Real ref_gp[6][4][3] = {
                            {
                                    {-1., -sqrt3, -sqrt3},
                                    {-1.,  sqrt3, -sqrt3},
                                    {-1., -sqrt3,  sqrt3},
                                    {-1.,  sqrt3,  sqrt3}
                            },
                            {
                                    { 1., -sqrt3, -sqrt3},
                                    { 1.,  sqrt3, -sqrt3},
                                    { 1., -sqrt3,  sqrt3},
                                    { 1.,  sqrt3,  sqrt3}
                            },
                            {
                                    {-sqrt3, -1., -sqrt3},
                                    { sqrt3, -1., -sqrt3},
                                    {-sqrt3, -1.,  sqrt3},
                                    { sqrt3, -1.,  sqrt3}
                            },
                            {
                                    {-sqrt3,  1., -sqrt3},
                                    { sqrt3,  1., -sqrt3},
                                    {-sqrt3,  1.,  sqrt3},
                                    { sqrt3,  1.,  sqrt3}
                            },
                            {
                                    {-sqrt3, -sqrt3, -1.},
                                    { sqrt3, -sqrt3, -1.},
                                    {-sqrt3,  sqrt3, -1.},
                                    { sqrt3,  sqrt3, -1.}
                            },
                            {
                                    {-sqrt3, -sqrt3,  1.},
                                    { sqrt3, -sqrt3,  1.},
                                    {-sqrt3,  sqrt3,  1.},
                                    { sqrt3,  sqrt3,  1.}
                            }
                    };

                    //  charm_quad_get_vertices(p4est, q, treeid, v);

                    for (int i = 0; i < 4; i++) {

                        Point gp;
                        Real gw;

                        Point point_;
                        point_[0] = ref_gp[fi][i][0];
                        point_[1] = ref_gp[fi][i][1];
                        point_[2] = ref_gp[fi][i][2];
                        
                        faceCalcGpAtPoint(fv, fi, i, point_, gp, gw);

                        face.gp.push_back(gp);
                        face.gw.push_back(gw);

                        //face.gw[i] = 1.;
                    }

                    face.area = 0.;
                    for (auto gw: face.gw) {
                        face.area += gw;
                    }

                    //std::cout << face.gp[0][0] << std::endl; */


                }
                
            }

        }
        if (doneFaces.size() != faces.size()) {
            throw Exception("Wrong faces number.");
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


    //  ---23.12.2023---
    //  Метод взят из charm_3d
    void Mesh::faceCalcGpAtPoint(Points vertices, Int face, Int i_gp, Point ref_p, Point &gp, Real &gj)
    {
        Index                 ix, iy, iz, vindex, i, j;
        Real              wx[2], wy[2], wz[2];
        Real              xfactor, yfactor;
        Real              jacob[2][2], a;
        Real              v[4][3];
        Real              sqrt3 = 1./sqrt(3.);
        int                 ind[2];
        const int ftv[6][4] =
                {{ 0, 2, 4, 6 },
                { 1, 3, 5, 7 },
                { 0, 1, 4, 5 },
                { 2, 3, 6, 7 },
                { 0, 1, 2, 3 },
                { 4, 5, 6, 7 }};
        Real ref_gp[4][3] =
                {{-sqrt3, -sqrt3},
                { sqrt3, -sqrt3},
                {-sqrt3,  sqrt3},
                { sqrt3,  sqrt3}};


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

        for (i = 0; i < 4; i++) {
            for (j = 0; j < 3; j++) {
                v[i][j] = vertices[ftv[face][i]][j];
            }
        }

        if (face == 0 || face == 1) {
            ind[0] = 1;
            ind[1] = 2;
        }
        else if (face == 2 || face == 3) {
            ind[0] = 0;
            ind[1] = 2;
        }
        else if (face == 4 || face == 5) {
            ind[0] = 0;
            ind[1] = 1;
        }

        // d/dksi
        wx[0] = -1.;
        wx[1] =  1.;
        wy[0] = 1.-ref_gp[i_gp][1];
        wy[1] = 1.+ref_gp[i_gp][1];
        jacob[0][0] = jacob[1][0] = 0.;
        vindex = 0;
        for (iy = 0; iy < 2; iy++) {
            yfactor = wy[iy];
            for (ix = 0; ix < 2; ix++) {
                xfactor = yfactor*wx[ix]/4.;
                jacob[0][0] += xfactor * v[vindex][ind[0]];
                jacob[1][0] += xfactor * v[vindex][ind[1]];
                vindex++;
            }
        }

        // d/deta
        wx[0] = 1.-ref_gp[i_gp][0];
        wx[1] = 1.+ref_gp[i_gp][0];
        wy[0] = -1.;
        wy[1] =  1.;
        jacob[0][1] = jacob[1][1] = 0.;
        vindex = 0;
        for (iy = 0; iy < 2; iy++) {
            yfactor = wy[iy];
            for (ix = 0; ix < 2; ix++) {
                xfactor = yfactor*wx[ix]/4.;
                jacob[0][1] += xfactor * v[vindex][ind[0]];
                jacob[1][1] += xfactor * v[vindex][ind[1]];
                vindex++;
            }
        }


        gj = jacob[0][0]*jacob[1][1]-jacob[0][1]*jacob[1][0];
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


}