/*
 *     ________  _____    ____  __  ___    _____ ____
 *    / ____/ / / /   |  / __ \/  |/  /   |__  // __ \  __    __
 *   / /   / /_/ / /| | / /_/ / /|_/ /     /_ </ / / /_/ /___/ /_
 *  / /___/ __  / ___ |/ _, _/ /  / /    ___/ / /_/ /_  __/_  __/
 *  \____/_/ /_/_/  |_/_/ |_/_/  /_/____/____/_____/ /_/   /_/
 *
 */


//
// Created by zhrv on 14.12.2020.
//

#include <Parallel.h>
#include "MethodFvmLMCh.h"
#include "FluxFvmLMChLF.h"
#include "VtkWriter.h"
#include "MethodFvmLMChException.h"

namespace charm {



    MethodFvmLMCh::MethodFvmLMCh(Config* conf): Method(conf) {
        flux = nullptr;
    }


    void MethodFvmLMCh::init() {
        Mesh& mesh = Config::getMesh();

        Index cN = mesh.getCellsCountWithGhost();
        Index compCount = Config::getCompCount();
        ru.resize(cN);
        rv.resize(cN);
        rw.resize(cN);
        rh.resize(cN);
        rc.resize(cN);
        ruInt.resize(cN);
        rvInt.resize(cN);
        rwInt.resize(cN);
        rhInt.resize(cN);
        rcInt.resize(cN);
        d.resize(cN);
        p.resize(cN);
        matId.resize(cN);

        for (Index ic = 0; ic < cN; ic++) {
            rc[ic].resize(compCount);
            rcInt[ic].resize(compCount);
            d[ic].resize(compCount);

            Cell &cell = mesh.getCell(ic);
            Region* reg = Config::getRegion(cell.tag);
            Prim prim(compCount);
            prim.matId = reg->matId;
            prim.v = reg->v;
            prim.t = reg->t;
            prim.p0 = reg->p;
            prim.p = 0.0;
            prim.c.assign(reg->c.begin(), reg->c.end());
            prim.eos(Material::EOS_LOW_MACH_T_P_TO_R_CZ_E);
            prim.eTot = prim.e + 0.5 * prim.v.sqr();

            setCons(ic, prim);
        }

        gradP.resize(cN, {0, 0, 0});
        gradT.resize(cN, {0, 0, 0});
        gradU.resize(cN, {0, 0, 0});
        gradV.resize(cN, {0, 0, 0});
        gradW.resize(cN, {0, 0, 0});
        gradC.resize(cN, ArrayVector(compCount, {0, 0, 0}));
        gradH.resize(cN, ArrayVector(compCount, {0, 0, 0}));

        fld.resize(cN);
        vecFld.resize(cN);
        rhsP.resize(cN);
        S.resize(cN);


        flux = new FluxFvmLMChLF(); ///< @todo @todo

        Parallel::exchange(ru);
        Parallel::exchange(rv);
        Parallel::exchange(rw);
        Parallel::exchange(rh);
//        Parallel::exchange(rc);   // TODO

        vtkWriter = new VtkWriter(this);

        save();

    }


    void MethodFvmLMCh::run() {

        while (true) {
            calcGrad();
            calcChem();
            calcAdv();
            calcGrad();
            calcPress();
            Config::get().t += Config::get().dt;
            Config::get().timestep++;

            save();

            if (Config::get().timestep % Config::get().logPeriod == 0) {
                std::cout << "STEP: " << Config::get().timestep << std::endl;
            }

            if (Config::get().t >= Config::get().time) {
                break;
            }
        }
    }


    void MethodFvmLMCh::done() {
    }


    void MethodFvmLMCh::save() {
        if (Config::get().timestep % Config::get().writePeriod == 0) {
            vtkWriter->write(Config::get().timestep);
        }
    }


    Real MethodFvmLMCh::calcDt() {
        Config& conf = Config::get();
        Mesh& mesh = Config::getMesh();
        Real dt = conf.dt;
        if (conf.cfl > 0) {
            for (Index iCell = 0; iCell < mesh.getCellsCount(); iCell++) {
                Prim prim = getPrim(iCell);
                dt = std::min(dt, conf.cfl * mesh.getCell(iCell).volume / (prim.v.length()));
            }
            Parallel::min(&dt);
        }
        return dt;
    }


    MethodFvmLMCh::~MethodFvmLMCh() {
//        integrals.clear();

    }


    Prim MethodFvmLMCh::getPrim(Index iCell) {
        Index compCount = Config::getCompCount();
        Prim prim(compCount);
        prim.matId = matId[iCell];
        prim.r = 0.;
        for (Index i = 0; i < compCount; i++) {
            prim.r += rc[iCell][i];
        }
        prim.v.x = ru[iCell] / prim.r;
        prim.v.y = rv[iCell] / prim.r;
        prim.v.z = rw[iCell] / prim.r;
        prim.h = rh[iCell] / prim.r;
        prim.p0 = p0;
        prim.p  = p[iCell];

        //prim.c.resize(cCount);
        for (Index i = 0; i < compCount; i++) {
            prim.c[i] = rc[iCell][i] / prim.r;
            if (prim.c[i] < 0.) prim.c[i] = 0.; // @todo
            if (prim.c[i] > 1.) prim.c[i] = 1.;
        }
        prim.eos(Material::EOS_LOW_MACH_R_TO_T_E);
        prim.eTot = prim.e + 0.5 * prim.v2();
        prim.ml = 0.;
        prim.kp = 0.;
        Real sm = 0.;
        for (Index i = 0; i < compCount; i++) {
            Component* comp = Config::getComponent(i);
            Real cm = prim.c[i]/comp->m;
            sm += cm;
            prim.ml += cm * comp->calcMl(prim.t);
            prim.kp += prim.c[i] * comp->calcKp(prim.t);
        }
        prim.ml /= sm;
        prim.shrink();
        return prim;
    }


    void MethodFvmLMCh::setCons(Index iCell, const Prim &prim) {
        Index count = prim.c.size();
        matId[iCell] = prim.matId;
        p0 = prim.p0;
        p[iCell] = prim.p;
        ru[iCell] = prim.r * prim.v.x;
        rv[iCell] = prim.r * prim.v.y;
        rw[iCell] = prim.r * prim.v.z;
        rh[iCell] = prim.r * prim.h;
        rc.resize(count);
        for (Index j = 0; j < count; j++) {
            rc[iCell][j] = prim.r * prim.c[j];
        }
        rc[iCell].shrink_to_fit();
    }


    Index MethodFvmLMCh::getScalarFieldsCount() {
        return 6 + Config::getCompCount();
    }

    String MethodFvmLMCh::getScalarFieldName(Index iFld) {
        if (iFld == 0) {
            return "Density";
        } else if (iFld == 1) {
            return "Pressure";
        } else if (iFld == 2) {
            return "Energy";
        } else if (iFld == 3) {
            return "Tot_Energy";
        } else if (iFld == 4) {
            return "Temperature";
        } else if (iFld == 5) {
            return "Mach_num";
        }
        for (Index i = 0; i < Config::getCompCount(); i++) {
            if (iFld == 6 + i) {
                return "C_" + Config::getComponent(i)->name;
            }
        }

        throw Exception("Wrong field number");
    }

    Real MethodFvmLMCh::getScalarFieldValue(Index i, Index iFld) {
        Prim prim = getPrim(i);
        if (iFld == 0) {
            return prim.r;
        } else if (iFld == 1) {
            return prim.p + prim.p0;
        } else if (iFld == 2) {
            return prim.e;
        } else if (iFld == 3) {
            return prim.eTot;
        } else if (iFld == 4) {
            return prim.t;
        } else if (iFld == 5) {
            return prim.v.length() / prim.cz;
        }
        for (Index i = 0; i < Config::getCompCount(); i++) {
            if (iFld == 6 + i) {
                return prim.c[i];
            }
        }

        throw Exception("Wrong field number");
    }

    Index MethodFvmLMCh::getVectorFieldsCount() {
        return 1;
    }

    String MethodFvmLMCh::getVectorFieldName(Index) {
        return "Velosity";
    }

    Vector MethodFvmLMCh::getVectorFieldValue(Index i, Index iFld) {
        Prim p = getPrim(i);
        return p.v;
    }

    void MethodFvmLMCh::zeroIntegrals() {
        Mesh &mesh = Config::getMesh();
        Index cN = mesh.getCellsCount();
        Index compCount = Config::getCompCount();

        for (Index ic = 0; ic < cN; ic++) {
            ruInt[ic]  = 0.;
            rvInt[ic]  = 0.;
            rwInt[ic]  = 0.;
            rhInt[ic]  = 0.;
            for (Index j = 0; j < compCount; j++) {
                rcInt[ic][j]  = 0.;
            }
        }
    }


}
