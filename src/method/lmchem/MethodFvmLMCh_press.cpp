/*
 *     ________  _____    ____  __  ___    _____ ____
 *    / ____/ / / /   |  / __ \/  |/  /   |__  // __ \  __    __
 *   / /   / /_/ / /| | / /_/ / /|_/ /     /_ </ / / /_/ /___/ /_
 *  / /___/ __  / ___ |/ _, _/ /  / /    ___/ / /_/ /_  __/_  __/
 *  \____/_/ /_/_/  |_/_/ |_/_/  /_/____/____/_____/ /_/   /_/
 *
 */

#include <Parallel.h>
#include <Operators.h>
#include "MethodFvmLMCh.h"
#include "ConfigFvmLMCh.h"


namespace charm {


    void MethodFvmLMCh::calcS() {
        Mesh& mesh = Config::getMesh();
        Index fN = mesh.getFacesCount();


        Index lN = mesh.getCellsCount();
        Index gN = mesh.getCellsCountWithGhost();
        ArrayReal tmp1(gN, 0);

        for (Index i = 0; i < gN; i++) {
            S[i] = 0.;
        }

        for (Index iFace = 0; iFace < fN; iFace++) {
            Face &face = mesh.getFace(iFace);
            Vector n = face.n;
            if (face.cells.size() == 1) {
                if (face.bnd->type == BoundaryCondition::WALL_NO_SLIP) {
                    Index c1 = face.cells[0];
                    Prim p1 = getPrim(c1);
                    Vector _gradT = gradT[c1];

                    Vector qt = _gradT;
                    qt *= -p1.kp;
                    qt *= face.area;
                    Real qn = scalarProd(qt, n); // @todo проверить знаки
                    S[c1] += qn/(p1.r*p1.cp*p1.t);


                }
            }
            else {
                Index c1 = face.cells[0];
                Index c2 = face.cells[1];
                Prim p1 = getPrim(c1);
                Prim p2 = getPrim(c2);
                Vector gradT1 = gradT[c1];
                Vector gradT2 = gradT[c2];
                Real vol1 = mesh.getCell(c1).volume;
                Real vol2 = mesh.getCell(c2).volume;

                gradT1 *= vol1*p1.kp; // @todo проверить вычисление KP
                gradT2 *= vol2*p2.kp; // @todo проверить вычисление KP

                Vector qt = gradT1;
                qt += gradT2;
                qt /= (vol1 + vol2);
                qt *= face.area;

                Real qn = scalarProd(qt, n);
                S[c1] += qn/(p1.r*p1.cp*p1.t);
                S[c2] -= qn/(p2.r*p2.cp*p2.t);
            }
        }

        for (Index m = 0; m < Config::getCompCount(); m++) {
            Component* comp = Config::getComponent(m);
            for (Index i = 0; i < gN; i++) {
                tmp1[i] = 0.;
            }
            for (Index iFace = 0; iFace < fN; iFace++) {
                Face &face = mesh.getFace(iFace);
                Vector n = face.n;
                if (face.cells.size() == 1) {
                    if (face.bnd->type == BoundaryCondition::WALL_NO_SLIP) {
                        Index c1 = face.cells[0];
                        Prim p1 = getPrim(c1);

                        Vector qt = gradC[c1][m];
                        qt *= d[c1][m];
                        qt *= p1.r;
                        qt *= face.area;
                        Real qn = scalarProd(qt, n); // @todo проверить знаки
                        qn *= p1.m/comp->m/p1.r;
                        tmp1[c1] += qn;
                    }
                } else {
                    Index c1 = face.cells[0];
                    Index c2 = face.cells[1];
                    Prim p1 = getPrim(c1);
                    Prim p2 = getPrim(c2);
                    Vector gradC1 = gradC[c1][m];
                    Vector gradC2 = gradC[c2][m];
                    Real vol1 = mesh.getCell(c1).volume;
                    Real vol2 = mesh.getCell(c2).volume;

                    gradC1 *= vol1 * d[c1][m] * p1.r;
                    gradC2 *= vol2 * d[c2][m] * p2.r;

                    Vector qt = gradC1;
                    qt += gradC2;
                    qt /= (vol1 + vol2);
                    qt *= face.area;

                    Real qn = scalarProd(qt, n);
                    tmp1[c1] += p1.m * qn / (p1.r * comp->m);
                    tmp1[c2] -= p2.m * qn / (p2.r * comp->m);
                }
            }
            for (Index i = 0; i < lN; i++) {
                Prim prim = getPrim(i);
                //S[i] += d[i][m]*scalarProd(gradC[i][m], gradH[i][m]) / (prim.cp * prim.t);

                S[i] += tmp1[i];
            }
        }
    }




    void MethodFvmLMCh::calcPress() {
        Mesh& mesh = Config::getMesh();
        Index compCount = Config::getCompCount();
        Index lN = mesh.getCellsCount();
        Index gN = mesh.getCellsCountWithGhost();
        ArrayReal rk(gN, 0);
        ArrayReal rk_1(gN, 0);
        ArrayReal r_tilde(gN, 0);
        ArrayReal vk(gN, 0);
        ArrayReal vk_1(gN, 0);
        ArrayReal sk(gN, 0);
        ArrayReal tk(gN, 0);
        ArrayReal pk(gN, 0);
        ArrayReal pk_1(gN, 0);
        ArrayReal xk(gN, 0);
        ArrayReal xk_1(gN, 0);
        ArrayReal tmp1(gN, 0);

        Real rok, alphak, omegak, betak;
        Real rok_1, alphak_1, omegak_1;


        Real dt = calcDt();
        calcS();


        for (Index i = 0; i < lN; i++) {
            Prim prim = getPrim(i);
            rhsP[i]     = 0.;
            vecFld[i] = prim.v;
        }

        Parallel::exchange(vecFld);

        Operators::div(rhsP, vecFld);

        for (Index i = 0; i < lN; i++) {
            rhsP[i] -= S[i];
            rhsP[i] /= dt;
        }



        Real rhsNorm2 = Operators::scProd(rhsP, rhsP);
        Real eps2 = dynamic_cast<ConfigFvmLMCh&>(Config::get()).pressEps;
        eps2 *= eps2;

        for (Index i = 0; i < lN; i++) {
            xk[i] = 0.;//data[i].p;
        }
        Parallel::exchange(xk);

        Operators::copy(rk, rhsP);
        opLaplace(tmp1, xk);
        Operators::sub(rk, tmp1);

        Operators::copy(r_tilde, rk);
        rok = alphak = omegak = 1.;

        Index iterK = dynamic_cast<ConfigFvmLMCh&>(Config::get()).pressMaxIter;
        while (iterK--) {
            Operators::copy(xk_1, xk);
            Operators::copy(rk_1, rk);
            Operators::copy(pk_1, pk);
            Operators::copy(vk_1, vk);

            rok_1 = rok;
            alphak_1 = alphak;
            omegak_1 = omegak;

            rok = Operators::scProd(r_tilde, rk_1);
            betak = (rok/rok_1)*(alphak_1/omegak_1);

            Operators::copy(tmp1, vk_1);
            Operators::mult(tmp1, omegak_1);
            Operators::copy(pk, pk_1);
            Operators::sub(pk, tmp1);
            Operators::mult(pk, betak);
            Operators::add(pk, rk_1);

            opLaplace(vk, pk);

            alphak = rok/Operators::scProd(r_tilde, vk);

            Operators::copy(sk, vk);
            Operators::mult(sk, -alphak);
            Operators::add(sk, rk_1);

            opLaplace(tk, sk);

            omegak = Operators::scProd(tk, sk) / Operators::scProd(tk, tk);

            Operators::copy(tmp1, sk);
            Operators::mult(tmp1, omegak);
            Operators::add(xk, tmp1);
            Operators::copy(tmp1, pk);
            Operators::mult(tmp1, alphak);
            Operators::add(xk, tmp1);

            Operators::copy(rk, tk);
            Operators::mult(rk, -omegak);
            Operators::add(rk, sk);

            Real err2 = Operators::scProd(rk, rk)/rhsNorm2;
            if (err2 < eps2) {
                break;
            }
        }

        for (Index i = 0; i < gN; i++) {
            p[i] = xk[i];
        }

        correctVelosities();
    }


    void MethodFvmLMCh::opLaplace(ArrayReal &out, ArrayReal &in) {
        Mesh& mesh = Config::getMesh();
        Index gN = mesh.getCellsCountWithGhost();

        assert(
                in.size() == out.size() &&
                in.size() == mesh.getCellsCountWithGhost() &&
                "Size of arrays must be equal to cells count"
        );

        Operators::grad(vecFld, in);


        for (Index iCell = 0; iCell < gN; iCell++) {
            Prim prim = getPrim(iCell);
            vecFld[iCell] /= prim.r;
        }

        Operators::div(out, vecFld);

    }


    void MethodFvmLMCh::correctVelosities() {
        Mesh& mesh = Config::getMesh();
        Index compCount = Config::getCompCount();
        Index lN = mesh.getCellsCount();
        Index gN = mesh.getCellsCountWithGhost();
//        ArrayVector gradIn(mesh->getCellsCountWithGhost(), {0., 0., 0.});

        for (Index i = 0; i < lN; i++) {
            fld[i] = p[i];
        }
        Parallel::exchange(fld);

        Operators::grad(vecFld, fld);


    }


}
