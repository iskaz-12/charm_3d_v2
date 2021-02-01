/**
 * Created by zhrv on 18.11.2020.
 * @author R.V.Zhalnin, zhalnin@gmail.com
 */

#include <FluxFvmHllc.h>
#include <iomanip>
#include <fstream>
#include <iostream>
#include <FluxFvmLf.h>
#include "MethodFvm.h"
#include "VtkWriter.h"


namespace charm {

    using Cons = DataFvm::Cons;


    void MethodFvm::init() {
        Index cN = mesh->getCellsCountWithGhost();
        Index compCount = Config::getCompCount();
        data.resize(cN, DataFvm(compCount));
        data.shrink_to_fit();
        for (Index ic = 0; ic < cN; ic++) {
            Cell &cell = mesh->getCell(ic);
            auto reg = Config::getRegion(cell.tag);
            Prim p(compCount);
            p.matId = reg->matId;
            p.v = reg->v;
            p.t = reg->t;
            p.p = reg->p;
            p.c.assign(reg->c.begin(), reg->c.end());
            p.eos(Material::EOS_T_P_TO_R_CZ_E);
            p.eTot = p.e + 0.5 * p.v.sqr();
            data[ic].setCons(p);
        }

        integrals.resize(cN, Cons(compCount));
        integrals.shrink_to_fit();

        flux = new FluxFvmHllc(); ///< @todo @todo

        exchange();

        vtkWriter = new VtkWriter(this);

        save();
    }


    void MethodFvm::run() {
        while (true) {
            Index compCount = Config::getCompCount();


            seroIntegrals();
            for (Index iFace = 0; iFace < mesh->getFacesCount(); iFace++) {
                Face &face = mesh->getFace(iFace);
                bool isBnd = face.cells.size() == 1;
                Index c1 = face.cells[0];
                Index c2;
                Prim p1 = data[c1].getPrim();
                Prim p2(compCount);

                if (isBnd) {
                    face.bnd->calc(p1, p2, face.n);
                } else {
                    c2 = face.cells[1];
                    p2 = data[c2].getPrim();
                }

                Cons flx(compCount);

                flux->calc(p1, p2, flx, face.n);
                flx *= face.area;
                integrals[c1] += flx;
                if (!isBnd) {
                    integrals[c2] -= flx;
                }
            }

            Real dt = calcDt();
            for (Index ic = 0; ic < mesh->getCellsCount(); ic++) {
                integrals[ic].normalize();
                Real cfl = dt / mesh->getCell(ic).volume;
                integrals[ic] *= cfl;
                data[ic].c -= integrals[ic];
            }

            exchange();

            conf->t += conf->dt;
            conf->timestep++;

            save();


            if (conf->timestep % conf->logPeriod == 0) {
                std::cout << "STEP: " << conf->timestep << std::endl;
            }

            if (conf->t >= conf->time) {
                break;
            }
        }
    }


    void MethodFvm::done() {

    }

    void MethodFvm::seroIntegrals() {
        for (auto &integral : integrals) {
            integral = 0.;
        }
    }

    Real MethodFvm::calcDt() {
        Config* conf = Config::get();
        Real dt = conf->dt;
        if (conf->cfl > 0) {
            for (Index iCell = 0; iCell < mesh->getCellsCount(); iCell++) {
                Prim p = data[iCell].getPrim();
                dt = std::min(dt, conf->cfl * mesh->getCell(iCell).volume / (p.v.length() + p.cz));
            }
        } else {
            dt = conf->dt;
        }
        return dt;
    }


    void MethodFvm::save() {
        if (conf->timestep % conf->writePeriod == 0) {
            vtkWriter->write(conf->timestep);
        }
    }


    Data* MethodFvm::getData(Index iCell) {
        return &(data[iCell]);
    }

}