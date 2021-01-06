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
#include "MethodFvmLMChException.h"

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

        gradP.resize(cN, {0, 0, 0});
        gradT.resize(cN, {0, 0, 0});
        gradU.resize(cN, {0, 0, 0});
        gradV.resize(cN, {0, 0, 0});
        gradW.resize(cN, {0, 0, 0});
        gradC.resize(cN, ArrayVector(compCount, {0, 0, 0}));
        gradH.resize(cN, ArrayVector(compCount, {0, 0, 0}));

        p.resize(cN);
        oldP.resize(cN);
        rhsP.resize(cN);

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

    void MethodFvmLMCh::exchange(Array<ArrayVector> &field) {
        if (field.size() != mesh->cCountGhost) {
            throw MethodException("Wrong fields count for exchange.");
        }
        Index compCount = Config::getCompCount();
        for (int p = 0; p < Parallel::procCount; p++) {
            if (p < Parallel::procId) {
                if (mesh->recvCount[p] > 0) {
                    ArrayVector buf(mesh->recvCount[p] * compCount);
                    Parallel::recv(p, 0, mesh->recvCount[p], buf.data());
                    for (Index i = 0; i < mesh->recvCount[p]; i++) {
                        for (Index j = 0; j < compCount; j++) {
                            field[mesh->recvShift[p] + i][j] = buf[i*compCount+j];
                        }
                    }
                }
                int n = mesh->sendInd[p].size();
                if (n > 0) {
                    ArrayVector buf(n*compCount);
                    for (int i = 0; i < n; i++) {
                        for (Index j = 0; j < compCount; j++) {
                            buf[i*compCount+j] = field[mesh->sendInd[p][i]][j];
                        }
                    }
                    Parallel::send(p, 1, n, buf.data());
                }
            }
            else if (p > Parallel::procId) {
                int n = mesh->sendInd[p].size();
                if (n > 0) {
                    ArrayVector buf(n*compCount);
                    for (int i = 0; i < n; i++) {
                        for (Index j = 0; j < compCount; j++) {
                            buf[i*compCount+j] = field[mesh->sendInd[p][i]][j];
                        }
                    }
                    Parallel::send(p, 0, n, buf.data());
                }
                if (mesh->recvCount[p] > 0) {
                    ArrayVector buf(mesh->recvCount[p]);
                    Parallel::recv(p, 1, mesh->recvCount[p], buf.data());
                    for (Index i = 0; i < mesh->recvCount[p]; i++) {
                        for (Index j = 0; j < compCount; j++) {
                            field[mesh->recvShift[p] + i][j] = buf[i*compCount+j];
                        }
                    }
                }
            }
        }
        Parallel::barrier();

    }


}
