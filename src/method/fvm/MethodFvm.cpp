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
#include "ConfigFvm.h"

namespace charm {

    void MethodFvm::init() {
        Mesh& mesh = Config::getMesh();
        Index cN = Config::getMesh().getCellsCountWithGhost();
        Index compCount = Config::getCompCount();
        matId.resize(cN);
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

        gradR.resize(cN);
        gradP.resize(cN);
        gradU.resize(cN);
        gradV.resize(cN);
        gradW.resize(cN);

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
            calcGrad();
            zeroIntegrals();
            for (Index iFace = 0; iFace < mesh.getFacesCount(); iFace++) {
                Face &face = mesh.getFace(iFace);
                Point fc = face.center;
                bool isBnd = face.cells.size() == 1;
                Index c1 = face.cells[0];
                Index c2;
                Prim p1 = reconstruct(c1, fc);
                Prim p2(1);

                if (isBnd) {
                    face.bnd->calc(p1, p2, face.n);
                } else {
                    c2 = face.cells[1];
                    p2 = reconstruct(c2, fc);
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
                    roInt[c2] -= flxR;
                    ruInt[c2] -= flxU;
                    rvInt[c2] -= flxV;
                    rwInt[c2] -= flxW;
                    reInt[c2] -= flxE;
                }
            }

            Real dt = calcDt();
            for (Index ic = 0; ic < mesh.getCellsCount(); ic++) {
                Real cfl = dt / mesh.getCell(ic).volume;

                ro[ic] -= NORM_REAL(cfl*roInt[ic]);
                ru[ic] -= NORM_REAL(cfl*ruInt[ic]);
                rv[ic] -= NORM_REAL(cfl*rvInt[ic]);
                rw[ic] -= NORM_REAL(cfl*rwInt[ic]);
                re[ic] -= NORM_REAL(cfl*reInt[ic]);
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

    void MethodFvm::zeroIntegrals() {
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
        return 5;
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

        throw Exception("Wrong field number");
    }

    Real MethodFvm::getScalarFieldValue(Index i, Index iFld) {
        Prim p = getPrim(i);
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


    void MethodFvm::setCons(Index i, const Prim &p) {
        Index count = p.c.size();
        matId[i] = p.matId;
        ro[i] = p.r;
        ru[i] = p.r * p.v.x;
        rv[i] = p.r * p.v.y;
        rw[i] = p.r * p.v.z;
        re[i] = p.r * (p.e + 0.5 * p.v2());
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
            gradR[i] = 0.;
        }

        for (Index iFace = 0; iFace < fN; iFace++) {
            Face &face = mesh.getFace(iFace);
            Vector n = face.n;
            bool isBnd = face.cells.size() == 1;
            Index c1 = face.cells[0];
            Index c2;
            Prim p1 = getPrim(c1);
            Prim p2(compCount);

            Real vol1 = mesh.getCell(c1).volume;
            Real vol2;

            if (isBnd) {
                face.bnd->calc(p1, p2, face.n);
                vol2 = vol1;
            } else {
                c2 = face.cells[1];
                p2 = getPrim(c2);
                vol2 = mesh.getCell(c2).volume;
            }

            Real s = face.area / (vol1 + vol2);
            vol1 *= s;
            vol2 *= s;

            Vector vR(n), vP(n), vU(n), vV(n), vW(n);
            Array<Vector> vC(compCount, n);
            Array<Vector> vH(compCount, n);


            vR *= vol1 * p1.r   + vol2 * p2.r;
            vP *= vol1 * p1.p   + vol2 * p2.p;
            vU *= vol1 * p1.v.x + vol2 * p2.v.x;
            vV *= vol1 * p1.v.y + vol2 * p2.v.y;
            vW *= vol1 * p1.v.z + vol2 * p2.v.z;


            gradR[c1] += vR;
            gradP[c1] += vP;
            gradU[c1] += vU;
            gradV[c1] += vV;
            gradW[c1] += vW;
            if (!isBnd) {
                gradR[c2] -= vR;
                gradP[c2] -= vP;
                gradU[c2] -= vU;
                gradV[c2] -= vV;
                gradW[c2] -= vW;
            }
        }

        for (Index iCell = 0; iCell < lN; iCell++) {
            Real vol = mesh.getCell(iCell).volume;
            gradR[iCell] /= vol;
            gradP[iCell] /= vol;
            gradU[iCell] /= vol;
            gradV[iCell] /= vol;
            gradW[iCell] /= vol;
        }

        Parallel::exchange(gradR);
        Parallel::exchange(gradP);
        Parallel::exchange(gradU);
        Parallel::exchange(gradV);
        Parallel::exchange(gradW);
    }

    Prim MethodFvm::reconstruct(Index iCell, Point pt) {
        Prim prim = getPrim(iCell);
        if (dynamic_cast<ConfigFvm&>(Config::get()).useReconstruct) {
            Point c = pt;
            c -= Config::getMesh().getCell(iCell).center;

            prim.r += scalarProd(gradR[iCell], c);
            prim.p += scalarProd(gradP[iCell], c);
            prim.v.x += scalarProd(gradU[iCell], c);
            prim.v.y += scalarProd(gradV[iCell], c);
            prim.v.z += scalarProd(gradW[iCell], c);
            prim.eos(Material::EOS_R_P_TO_E_T);
            prim.eTot = prim.e + 0.5 * prim.v2();
        }
        return prim;
    }

}