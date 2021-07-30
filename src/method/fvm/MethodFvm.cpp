/*
 *     ________  _____    ____  __  ___    _____ ____
 *    / ____/ / / /   |  / __ \/  |/  /   |__  // __ \  __    __
 *   / /   / /_/ / /| | / /_/ / /|_/ /     /_ </ / / /_/ /___/ /_
 *  / /___/ __  / ___ |/ _, _/ /  / /    ___/ / /_/ /_  __/_  __/
 *  \____/_/ /_/_/  |_/_/ |_/_/  /_/____/____/_____/ /_/   /_/
 *
 */


/**
 * Created by zhrv on 18.11.2020.
 * @author R.V.Zhalnin, zhalnin@gmail.com
 */

#include <FluxFvmHllc.h>
#include <iostream>
#include <Parallel.h>
#include <Log.h>
#include "MethodFvm.h"
#include "VtkWriter.h"
#include "MethodException.h"
#include "ConfigFvm.h"

namespace charm {

    void MethodFvm::init() {
        Mesh& mesh = Config::getMesh();
        Index cN = Config::getMesh().getCellsCountWithGhost();
        Index compCount = Config::getCompCount();
        matId.resize(cN);
        ru.resize(cN);
        rv.resize(cN);
        rw.resize(cN);
        re.resize(cN);
        rc.resize(cN, ArrayReal(compCount));
        ru.shrink_to_fit();
        rv.shrink_to_fit();
        rw.shrink_to_fit();
        re.shrink_to_fit();
        rc.shrink_to_fit();

        ruInt.resize(cN);
        rvInt.resize(cN);
        rwInt.resize(cN);
        reInt.resize(cN);
        rcInt.resize(cN, ArrayReal(compCount));

        rcInt.shrink_to_fit();
        ruInt.shrink_to_fit();
        rvInt.shrink_to_fit();
        rwInt.shrink_to_fit();
        reInt.shrink_to_fit();

        gradP.resize(cN);
        gradU.resize(cN);
        gradV.resize(cN);
        gradW.resize(cN);
        gradRC.resize(cN, ArrayVector(compCount));

        gradP.shrink_to_fit();
        gradU.shrink_to_fit();
        gradV.shrink_to_fit();
        gradW.shrink_to_fit();
        gradRC.shrink_to_fit();

        for (Index ic = 0; ic < cN; ic++) {
            Cell &cell = mesh.getCell(ic);
            auto reg = Config::getRegion(cell.tag);
            Prim p(compCount);
            p.matId = reg->matId;
            p.v = reg->v;
            p.t = reg->t;
            p.p = reg->p;
            p.c.assign(reg->c.begin(), reg->c.end());
            p.eos(Material::EOS_T_P_TO_R_CZ_E);
            p.eTot = p.e + 0.5 * p.v.sqr();
            setCons(ic, p);
        }

        flux = new FluxFvmHllc(); ///< @todo @todo

        exchangeFields();

        vtkWriter = new VtkWriter(this);

        save();
    }


    void MethodFvm::run() {
        Mesh& mesh = Config::getMesh();
        Index compCount = Config::getCompCount();
        while (true) {
            calcGrad();
            zeroIntegrals();
            for (Index iFace = 0; iFace < mesh.getFacesCount(); iFace++) {
                Face &face = mesh.getFace(iFace);
                Point fc = face.center;
                bool isBnd = face.cells.size() == 1;
                Index c1 = face.cells[0];
                Index c2;
                Prim p1 = reconstruct(c1, fc);
                Prim p2(compCount);

                if (isBnd) {
                    face.bnd->calc(p1, p2, face.n);
                } else {
                    c2 = face.cells[1];
                    p2 = reconstruct(c2, fc);
                }

                Real flxU, flxV, flxW, flxE;
                ArrayReal flxC(compCount);

                flux->calc(p1, p2, flxU, flxV, flxW, flxE, flxC, face.n);
                flxU *= face.area;
                flxV *= face.area;
                flxW *= face.area;
                flxE *= face.area;
                for (Index j = 0; j < compCount; j++) {
                    flxC[j] *= face.area;
                }

                ruInt[c1] += flxU;
                rvInt[c1] += flxV;
                rwInt[c1] += flxW;
                reInt[c1] += flxE;
                for (Index j = 0; j < compCount; j++) {
                    rcInt[c1][j] += flxC[j];
                }
                if (!isBnd) {
                    ruInt[c2] -= flxU;
                    rvInt[c2] -= flxV;
                    rwInt[c2] -= flxW;
                    reInt[c2] -= flxE;
                    for (Index j = 0; j < compCount; j++) {
                        rcInt[c2][j] -= flxC[j];
                    }
                }
            }

            Real dt = calcDt();
            Index cN = mesh.getCellsCount();
            for (Index iCell = 0; iCell < cN; iCell++) {
                Real cfl = dt / mesh.getCell(iCell).volume;

                ru[iCell] -= NORM_REAL(cfl * ruInt[iCell]);
                rv[iCell] -= NORM_REAL(cfl * rvInt[iCell]);
                rw[iCell] -= NORM_REAL(cfl * rwInt[iCell]);
                re[iCell] -= NORM_REAL(cfl * reInt[iCell]);
                for (Index iComp = 0; iComp < compCount; iComp++) {
                    rc[iCell][iComp] -= NORM_REAL(cfl * rcInt[iCell][iComp]);
                }
            }

            exchangeFields();

            Config::get().t += Config::get().dt;
            Config::get().timestep++;

            save();

            if (Config::get().timestep % Config::get().logPeriod == 0) {
                std::stringstream ss;
                ss << "STEP: " << Config::get().timestep << std::endl;
                Log::print(ss.str());
            }

            if (Config::get().t >= Config::get().maxTime) {
                break;
            }
        }
    }


    void MethodFvm::done() {}


    void MethodFvm::zeroIntegrals() {
        Mesh& mesh = Config::getMesh();
        Index cN = mesh.getCellsCount();
        Index compCount = Config::getCompCount();
        for (Index i = 0; i < cN; i++) {
            ruInt[i] = 0.;
            rvInt[i] = 0.;
            rwInt[i] = 0.;
            reInt[i] = 0.;
            for (Index j = 0; j < compCount; j++) {
                rcInt[i][j] = 0.;
            }
        }
    }

    Real MethodFvm::calcDt() {
        Config& conf = Config::get();
        Mesh& mesh = Config::getMesh();
        Real dt = conf.dt;
        if (conf.cfl > 0) {
            for (Index iCell = 0; iCell < mesh.getCellsCount(); iCell++) {
                Prim p = getPrim(iCell);
                dt = std::min(dt, conf.cfl * mesh.getCell(iCell).volume / (p.v.length() + p.cz));
            }
        } else {
            dt = conf.dt;
        }
        return dt;
    }


    void MethodFvm::save() {
        Config& conf = Config::get();
        if (conf.timestep % conf.writePeriod == 0) {
            vtkWriter->write(conf.timestep);
        }
    }


    Index MethodFvm::getScalarFieldsCount() {
        return 6+Config::getCompCount();
    }

    String MethodFvm::getScalarFieldName(Index iFld) {
        assert(iFld < getScalarFieldsCount() && "");
        if (iFld == 0) {
            return "Density";
        }
        else if (iFld == 1) {
            return "Pressure";
        }
        else if (iFld == 2) {
            return "Energy";
        }
        else if (iFld == 3) {
            return "Tot_Energy";
        }
        else if (iFld == 4) {
            return "Temperature";
        }
        else if (iFld == 5) {
            return "Mach_num";
        }
        for (Index i = 0; i < Config::getCompCount(); i++) {
            if (iFld == 6 + i) {
                return "C_" + Config::getComponent(i)->name;
            }
        }

        throw Exception("Wrong field number");
    }

    Real MethodFvm::getScalarFieldValue(Index iCell, Index iFld) {
        assert(iFld < getScalarFieldsCount() && "");
        Prim p = getPrim(iCell);
        if (iFld == 0) {
            return p.r;
        }
        else if (iFld == 1) {
            return p.p;
        }
        else if (iFld == 2) {
            return p.e;
        }
        else if (iFld == 3) {
            return p.eTot;
        }
        else if (iFld == 4) {
            return p.t;
        }
        else if (iFld == 5) {
            return p.v.length()/p.cz;
        }
        for (Index i = 0; i < Config::getCompCount(); i++) {
            if (iFld == 6 + i) {
                return p.c[i];
            }
        }

        throw Exception("Wrong field number");
    }

    Index MethodFvm::getVectorFieldsCount() {
        return 1;
    }

    String MethodFvm::getVectorFieldName(Index  iFld) {
        return "Velosity";
    }

    Vector MethodFvm::getVectorFieldValue(Index i, Index  iFld) {
        Prim p = getPrim(i);
        return p.v;
    }

    Prim MethodFvm::getPrim(Index iCell) {
        Index compCount = Config::getCompCount();
        Prim prim(compCount);
        prim.matId = matId[iCell];
        prim.r = 0.;
        for (Index i = 0; i < compCount; i++) {
            prim.r += rc[iCell][i];
        }
        prim.v.x   = ru[iCell] / prim.r;
        prim.v.y   = rv[iCell] / prim.r;
        prim.v.z   = rw[iCell] / prim.r;
        prim.eTot  = re[iCell] / prim.r;
        prim.e     = prim.eTot - 0.5 * prim.v2();
        if (prim.e < EPS) {
            throw MethodException("p.e < EPS");
        }

        for (Index i = 0; i < compCount; i++) {
            prim.c[i] = rc[iCell][i] / prim.r;
            if (prim.c[i] < 0.) prim.c[i] = 0.; // @todo
            if (prim.c[i] > 1.) prim.c[i] = 1.;
        }
        prim.eos(Material::EOS_R_E_TO_P_CZ_T);
        prim.shrink();
        return prim;
    }


    void MethodFvm::setCons(Index iCell, const Prim &prim) {
        Index compCount = Config::getCompCount();
        assert(prim.c.size() == compCount && "[MethodFvm::setCons()]: Components count of 'prim' is not equal to global value");
        matId[iCell] = prim.matId;
        ru[iCell] = prim.r * prim.v.x;
        rv[iCell] = prim.r * prim.v.y;
        rw[iCell] = prim.r * prim.v.z;
        re[iCell] = prim.r * (prim.e + 0.5 * prim.v2());
        for (Index j = 0; j < compCount; j++) {
            rc[iCell][j] = prim.r * prim.c[j];
        }
    }


    void MethodFvm::calcGrad() {
        Mesh& mesh = Config::getMesh();
        Index lN = mesh.getCellsCount();
        Index gN = mesh.getCellsCountWithGhost();
        Index fN = mesh.getFacesCount();
        Index compCount = Config::getCompCount();

        for (Index i = 0; i < gN; i++) {
            gradP[i] = 0.;
            gradU[i] = 0.;
            gradV[i] = 0.;
            gradW[i] = 0.;
            for (Index iComp = 0; iComp < compCount; iComp++) {
                gradRC[i][iComp] = 0.;
            }
        }

        for (Index iFace = 0; iFace < fN; iFace++) {
            Face &face = mesh.getFace(iFace);
            Vector n = face.n;
            bool isBnd = face.cells.size() == 1;
            Index c1 = face.cells[0];
            Index c2;

            ArrayReal rc1(compCount), rc2(compCount);
            Prim p1 = getPrim(c1);
            Prim p2(compCount);

            for (Index i = 0; i < compCount; i++) {
                rc1[i] = rc[c1][i];
            }

            Real vol1 = mesh.getCell(c1).volume;
            Real vol2;

            if (isBnd) {
                face.bnd->calc(p1, p2, face.n);
                vol2 = vol1;
                for (Index i = 0; i < compCount; i++) {
                    rc2[i] = rc[c1][i];
                }
            } else {
                c2 = face.cells[1];
                p2 = getPrim(c2);
                vol2 = mesh.getCell(c2).volume;

                for (Index i = 0; i < compCount; i++) {
                    rc2[i] = rc[c2][i];
                }
            }

            Real s = face.area / (vol1 + vol2);
            vol1 *= s;
            vol2 *= s;

            Vector vP(n), vU(n), vV(n), vW(n);
            Array<Vector> vC(compCount, n);

            vP *= vol1 * p1.p   + vol2 * p2.p;
            vU *= vol1 * p1.v.x + vol2 * p2.v.x;
            vV *= vol1 * p1.v.y + vol2 * p2.v.y;
            vW *= vol1 * p1.v.z + vol2 * p2.v.z;
            for (Index iComp = 0; iComp < compCount; iComp++) {
                vC[iComp] *= vol1 * rc1[iComp] + vol2 * rc2[iComp];
            }

            gradP[c1] += vP;
            gradU[c1] += vU;
            gradV[c1] += vV;
            gradW[c1] += vW;
//            for (Index iComp = 0; iComp < compCount; iComp++) {
//                gradRC[c1][iComp] += vC[iComp];
//            }
            if (!isBnd) {
                gradP[c2] -= vP;
                gradU[c2] -= vU;
                gradV[c2] -= vV;
                gradW[c2] -= vW;
//                for (Index iComp = 0; iComp < compCount; iComp++) {
//                    gradRC[c2][iComp] -= vC[iComp];
//                }
            }
        }

        for (Index iCell = 0; iCell < lN; iCell++) {
            Real vol = mesh.getCell(iCell).volume;
            gradP[iCell] /= vol; gradP[iCell].normalize();
            gradU[iCell] /= vol; gradU[iCell].normalize();
            gradV[iCell] /= vol; gradV[iCell].normalize();
            gradW[iCell] /= vol; gradW[iCell].normalize();
//            for (Index iComp = 0; iComp < compCount; iComp++) {
//                gradRC[iCell][iComp] /= vol; gradRC[iCell][iComp].normalize();
//            }
        }

        exchangeGrads();
    }

    Prim MethodFvm::reconstruct(Index iCell, Point pt) {
        Prim prim = getPrim(iCell);
        if (dynamic_cast<ConfigFvm&>(Config::get()).useReconstruct) {
            Index compCount = Config::getCompCount();
            Point c = pt;
            c -= Config::getMesh().getCell(iCell).center;

            ArrayReal _rc(rc[iCell].begin(), rc[iCell].end());
            prim.r = 0.;
            for (Index i = 0; i < compCount; i++) {
                _rc[i]   += scalarProd(gradRC[iCell][i], c);
                prim.r += _rc[i];
            }
            for (Index i = 0; i < compCount; i++) {
                prim.c[i] = _rc[i] / prim.r;
                if (prim.c[i] < 0.) prim.c[i] = 0.; // @todo
                if (prim.c[i] > 1.) prim.c[i] = 1.;
            }

//            Real sc = 0.;
//            for (Index iComp = 0; iComp < compCount; iComp++) {
//                prim.c[iComp] += scalarProd(gradRC[iCell][iComp], c);
//                sc += prim.c[iComp];
//            }
//            for (Index iComp = 0; iComp < compCount; iComp++) {
//                prim.c[iComp] /= sc;
//            }
//

            prim.p   += scalarProd(gradP[iCell], c);
            prim.v.x += scalarProd(gradU[iCell], c);
            prim.v.y += scalarProd(gradV[iCell], c);
            prim.v.z += scalarProd(gradW[iCell], c);
            prim.eos(Material::EOS_R_P_TO_E_T);
            prim.eTot = prim.e + 0.5 * prim.v2();
        }
        return prim;
    }


    void MethodFvm::exchangeFields() {
        Parallel::exchange(ru);
        Parallel::exchange(rv);
        Parallel::exchange(rw);
        Parallel::exchange(re);
        Parallel::exchange(rc);

    }

    void MethodFvm::exchangeGrads() {
        Parallel::exchange(gradP);
        Parallel::exchange(gradU);
        Parallel::exchange(gradV);
        Parallel::exchange(gradW);
        Parallel::exchange(gradRC);
    }

}