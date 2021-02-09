/**
 * Created by zhrv on 18.11.2020.
 * @author R.V.Zhalnin, zhalnin@gmail.com
 */

#include <FluxFvmHllc.h>
#include <iomanip>
#include <fstream>
#include <iostream>
#include <Parallel.h>
#include "MethodFvm.h"
#include "VtkWriter.h"
#include "MethodException.h"

namespace charm {

    void MethodFvm::init() {
        Mesh& mesh = Config::getMesh();
        Index cN = Config::getMesh().getCellsCountWithGhost();
        Index compCount = Config::getCompCount();
        ro.resize(cN);
        ru.resize(cN);
        rv.resize(cN);
        rw.resize(cN);
        re.resize(cN);
        ro.shrink_to_fit();
        ru.shrink_to_fit();
        rv.shrink_to_fit();
        rw.shrink_to_fit();
        re.shrink_to_fit();

        roInt.resize(cN);
        ruInt.resize(cN);
        rvInt.resize(cN);
        rwInt.resize(cN);
        reInt.resize(cN);
        roInt.shrink_to_fit();
        ruInt.shrink_to_fit();
        rvInt.shrink_to_fit();
        rwInt.shrink_to_fit();
        reInt.shrink_to_fit();

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

        Parallel::exchange(ro);
        Parallel::exchange(ru);
        Parallel::exchange(rv);
        Parallel::exchange(rw);
        Parallel::exchange(re);

        vtkWriter = new VtkWriter(this);

        save();
    }


    void MethodFvm::run() {
        Mesh& mesh = Config::getMesh();
        while (true) {
            Index compCount = Config::getCompCount();


            seroIntegrals();
            for (Index iFace = 0; iFace < mesh.getFacesCount(); iFace++) {
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

                Real flxR, flxU, flxV, flxW, flxE;

                flux->calc(p1, p2, flxR, flxU, flxV, flxW, flxE, face.n);
                flxR *= face.area;
                flxU *= face.area;
                flxV *= face.area;
                flxW *= face.area;
                flxE *= face.area;
                roInt[c1] += flxR;
                ruInt[c1] += flxU;
                rvInt[c1] += flxV;
                rwInt[c1] += flxW;
                reInt[c1] += flxE;
                if (!isBnd) {
                    roInt[c2] += flxR;
                    ruInt[c2] += flxU;
                    rvInt[c2] += flxV;
                    rwInt[c2] += flxW;
                    reInt[c2] += flxE;
                }
            }

            Real dt = calcDt();
            for (Index ic = 0; ic < mesh.getCellsCount(); ic++) {
                Real cfl = dt / mesh.getCell(ic).volume;

                ro[ic] -= cfl*roInt[ic];
                ru[ic] -= cfl*roInt[ic];
                rv[ic] -= cfl*roInt[ic];
                rw[ic] -= cfl*roInt[ic];
                re[ic] -= cfl*roInt[ic];
             }

            Parallel::exchange(ro);
            Parallel::exchange(ru);
            Parallel::exchange(rv);
            Parallel::exchange(rw);
            Parallel::exchange(re);

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


    void MethodFvm::done() {

    }

    void MethodFvm::seroIntegrals() {
        Mesh& mesh = Config::getMesh();
        for (Index i = 0; i < mesh.getCellsCount(); i++) {
            roInt[i] = 0.;
            ruInt[i] = 0.;
            rvInt[i] = 0.;
            rwInt[i] = 0.;
            reInt[i] = 0.;
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
        return 5 + Config::getCompCount();
    }

    String MethodFvm::getScalarFieldName(Index iFld) {
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
            if (iFld == 5+i) {
                return "C_"+Config::getComponent(i)->name;
            }
        }

        throw Exception("Wrong field number");
    }

    Real MethodFvm::getScalarFieldValue(Index iFld) {
        Prim p = getPrim(iFld);
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
            if (iFld == 5+i) {
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

    Vector MethodFvm::getVectorFieldValue(Index  iFld) {
        Prim p = getPrim(iFld);
        return p.v;
    }

    Prim MethodFvm::getPrim(Index iFld) {
        Index cCount = Config::getCompCount();
        Prim p(cCount);
        p.matId = matId[iFld];
        p.r     = ro[iFld];
        p.v.x   = ru[iFld]/p.r;
        p.v.y   = rv[iFld]/p.r;
        p.v.z   = rw[iFld]/p.r;
        p.eTot  = re[iFld]/p.r;
        p.e     = p.eTot-0.5*p.v2();
        if (p.e < EPS) {
            throw MethodException("p.e < EPS");
        }

        p.c.resize(1);
        p.c[0] = 1.;
        p.eos(Material::EOS_R_E_TO_P_CZ_T);
        p.shrink();
        return p;
    }

    void MethodFvm::setCons(Index i, Prim &p) {
        Index count = p.c.size();
        matId[i] = p.matId;
        ro[i] = p.r;
        ru[i] = p.r * p.v.x;
        rv[i] = p.r * p.v.y;
        rw[i] = p.r * p.v.z;
        re[i] = p.r * (p.e + 0.5 * p.v2());
    }

}