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

#include "MethodFvmLMCh.h"
#include "FluxFvmLMChLF.h"
#include "VtkWriter.h"

namespace charm {

    using Cons = DataFvmLMCh::Cons;


    MethodFvmLMCh::MethodFvmLMCh(Config *conf): Method(conf) {
        flux = nullptr;
    }


    void MethodFvmLMCh::init() {
        Index cN = mesh->cCountGhost;
        Index compCount = Config::getCompCount();
        data.resize(cN, DataFvmLMCh(compCount));
        data.shrink_to_fit();
        for (Index ic = 0; ic < cN; ic++) {
            Cell &cell = mesh->cells[ic];
            Region *reg = Config::getRegion(cell.tag);
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

        flux = new FluxFvmLMChLF(); ///< @todo @todo

        exchange();

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


    void MethodFvmLMCh::done() {
    }


    Data *MethodFvmLMCh::getData(Index iCell) {
        return nullptr;
    }


    void MethodFvmLMCh::save() {
        if (conf->timestep % conf->writePeriod == 0) {
            vtkWriter->write(conf->timestep);
        }
    }


    Real MethodFvmLMCh::calcDt() {
        Config *conf = Config::get();
        Real dt = conf->dt;
        if (conf->cfl > 0) {
            for (Index iCell = 0; iCell < mesh->cCount; iCell++) {
                Prim p = data[iCell].getPrim();
                dt = std::min(dt, conf->cfl * mesh->cells[iCell].volume / (p.v.length()));
            }
            Parallel::min(&dt);
        }
        return dt;
    }


    MethodFvmLMCh::~MethodFvmLMCh() {
        integrals.clear();
        if (flux != nullptr) {
            delete flux;
        }
    }

}
