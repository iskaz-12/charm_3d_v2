/**
 * Created by zhrv on 18.11.2020.
 * @author R.V.Zhalnin, zhalnin@gmail.com
 */

#include <FluxHllc.h>
#include <iomanip>
#include <fstream>
#include <iostream>
#include <FluxLf.h>
#include "MethodFvm.h"

namespace charm {

    void MethodFvm::init() {
        Index cN = mesh->cells.size();
        Index cCount = Config::getCompCount();
        data.resize(cN, Data(cCount));
        data.shrink_to_fit();
        for (Index ic = 0; ic < cN; ic++) {
            Cell &cell = mesh->cells[ic];
            Region *reg = Config::getRegion(cell.tag);
            Prim p(cCount);
            p.matId = reg->matId;
            p.v     = reg->v;
            p.t     = reg->t;
            p.p     = reg->p;
            p.c.assign(reg->c.begin(), reg->c.end());
            data[ic].grav = reg->grav;
            p.eos(Material::EOS_T_P_TO_R_CZ_E);
            p.eTot = p.e + 0.5*p.v.sqr();
            data[ic].setCons(p);
        }

        integrals.resize(cN, Cons(cCount));
        integrals.shrink_to_fit();

        flux = new FluxHllc(); ///< @todo @todo
        save();
    }


    void MethodFvm::run() {
        while (true) {
            Index cCount = Config::getCompCount();


            seroIntegrals();
            for (auto & face : mesh->faces) {
                bool isBnd = face.cells.size() == 1;
                Index c1 = face.cells[0];
                Index c2;
                Prim p1 = data[c1].getPrim();
                Prim p2(cCount);

                if (isBnd) {
                    face.bnd->calc(p1, p2, face.n);
                }
                else{
                    c2 = face.cells[1];
                    p2 = data[c2].getPrim();
                }

                Cons flx(cCount);

                flux->calc(p1, p2, flx, face.n);

                integrals[c1].ru += flx.ru * face.area;
                integrals[c1].rv += flx.rv * face.area;
                integrals[c1].rw += flx.rw * face.area;
                integrals[c1].re += flx.re * face.area;
                for (Index i = 0; i < cCount; i++) {
                    integrals[c1].rc[i] += flx.rc[i] * face.area;
                }
                if (!isBnd) {
                    integrals[c2].ru -= flx.ru * face.area;
                    integrals[c2].rv -= flx.rv * face.area;
                    integrals[c2].rw -= flx.rw * face.area;
                    integrals[c2].re -= flx.re * face.area;

                    for (Index i = 0; i < cCount; i++) {
                        integrals[c2].rc[i] -= flx.rc[i] * face.area;
                    }
                }
            }

            Real dt = calcDt();
            for (Index ic = 0; ic < data.size(); ic++) {
                integrals[ic].normalize();
                Real cfl = dt / mesh->cells[ic].volume;
                data[ic].c.ru -= cfl * integrals[ic].ru;
                data[ic].c.rv -= cfl * integrals[ic].rv;
                data[ic].c.rw -= cfl * integrals[ic].rw;
                data[ic].c.re -= cfl * integrals[ic].re;
                for (Index i = 0; i < cCount; i++) {
                    data[ic].c.rc[i] -= cfl * integrals[ic].rc[i];
                }

                Prim p = data[ic].getPrim();
            }
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
            integral.ru = 0.;
            integral.rv = 0.;
            integral.rw = 0.;
            integral.re = 0.;
            integral.rc.assign(Config::getCompCount(), 0.);
        }
    }

    Real MethodFvm::calcDt() {
        Config *conf = Config::get();
        Real dt = conf->dt;
        if (conf->cfl > 0) {
            for (Index iCell = 0; iCell < mesh->cells.size(); iCell++)
            {
                Prim p = data[iCell].getPrim();
                dt = std::min(dt, conf->cfl * mesh->cells[iCell].volume / (p.v.length() + p.cz));
            }
        }
        else {
            dt = conf->dt;
        }
        return dt;
    }


    void MethodFvm::save() {
        if (conf->timestep % conf->writePeriod == 0) {
            saveVtk();
        }
    }


    void MethodFvm::saveVtk() // @todo:
    {
        Index cCount = Config::getCompCount();
        Prim p(cCount);
        Material *mat;
        String fileName;
        std::ofstream fout;

        Index nodesCount = mesh->nodes.size();
        Index cellsCount = mesh->cells.size();

        std::stringstream ss;
        ss << "charm_" << std::setfill('0') << std::setw(8) << conf->timestep << ".vtk";
        fileName = ss.str();

        std::cout <<" Writing VTK file '" << fileName << "'..." << std::endl;

        fout.open(fileName, std::ios_base::out);
        fout << "# vtk DataFile Version 3.0" << std::endl;

        //  UPDATE ON 02.07.2023 - скорее всего, ошибочное название метода в результативном vtk-файле...
        fout << "3D RKDG method results for task '%s'." << std::endl;
        fout << "ASCII" << std::endl;
        fout << "DATASET UNSTRUCTURED_GRID" << std::endl;



        fout << "POINTS " << nodesCount << " double" << std::endl;

        for (const auto &node: mesh->nodes) {
            fout << node.x << " " << node.y << " " << node.z << std::endl;
        }

        fout << std::endl << "CELLS " << cellsCount << " " << 9*cellsCount << std::endl;

        for (const auto &cell: mesh->cells) {

            fout << cell.nodesInd.size();
            for (auto nIdx: cell.nodesInd) {
                fout << " " << nIdx;
            }
            fout << std::endl;
        }

        fout << std::endl << "CELL_TYPES " << cellsCount << std::endl;

        for (int i = 1; i <= cellsCount; i++)
        {
            fout << "11 ";
            if (i % 20 == 0 ) {
                fout << std::endl;
            }
        }
        fout << std::endl;


        // Плотность
        fout << "CELL_DATA " << cellsCount << std::endl;
        fout << std::endl << "SCALARS Density double 1" << std::endl << "LOOKUP_TABLE default" << std::endl;
        for (int i = 0; i < cellsCount; i++)
        {
            Prim prim = data[i].getPrim();
            fout << prim.r << " ";
            if (i % 8 == 0  ||  i == cellsCount) {
                fout << std::endl;
            }
        }

        // Давление
        fout << std::endl << "SCALARS Pressure double 1" << std::endl << "LOOKUP_TABLE default" << std::endl;
        for (int i = 0; i < cellsCount; i++)
        {
            Prim prim = data[i].getPrim();
            fout << prim.p << " ";
            if (i % 8 == 0  ||  i == cellsCount) {
                fout << std::endl;
            }
        }

        // Температура
        fout << std::endl << "SCALARS Temperature double 1" << std::endl << "LOOKUP_TABLE default" << std::endl;
        for (int i = 0; i < cellsCount; i++)
        {
            Prim prim = data[i].getPrim();
            //  UPDATE ON 02.07.2023 - вместо температуры выводится плотность!!!
            //fout << prim.r << " ";
            fout << prim.t << " ";
            if (i % 8 == 0  ||  i == cellsCount) {
                fout << std::endl;
            }
        }

        // Полная энергия
        fout << std::endl << "SCALARS TotalEnergy double 1" << std::endl << "LOOKUP_TABLE default" << std::endl;
        for (int i = 0; i < cellsCount; i++)
        {
            Prim prim = data[i].getPrim();
            fout << prim.eTot << " ";
            if (i % 8 == 0  ||  i == cellsCount) {
                fout << std::endl;
            }
        }

        // Число Маха
        fout << std::endl << "SCALARS Mach double 1" << std::endl << "LOOKUP_TABLE default" << std::endl;
        for (int i = 0; i < cellsCount; i++)
        {
            Prim prim = data[i].getPrim();
            fout << ::sqrt(prim.v2())/prim.cz << " ";
            if (i % 8 == 0  ||  i == cellsCount) {
                fout << std::endl;
            }
        }


        // Вектор скорости
        fout << std::endl << "VECTORS Velosity double" << std::endl;
        for (int i = 0; i < cellsCount; i++)
        {
            Prim prim = data[i].getPrim();
            fout << prim.v.x << " " << prim.v.y << " " << prim.v.z << " ";
            if (i % 8 == 0  ||  i == cellsCount) {
                fout << std::endl;
            }
        }


        // Полное давление
        fout << std::endl << "SCALARS TotalPressure double 1" << std::endl << "LOOKUP_TABLE default" << std::endl;
        for (int i = 0; i < cellsCount; i++)
        {
            Prim prim = data[i].getPrim();
            fout << prim.p * ::pow( 1.0 + 0.5 * ( prim.gam - 1.0 ) * prim.v2() / ( prim.cz*prim.cz ) , prim.gam / ( prim.gam - 1.0 ) ) << " ";
            if (i % 8 == 0  ||  i == cellsCount) {
                fout << std::endl;
            }
        }

        // Полная температура
        fout << std::endl << "SCALARS TotalTemperature double 1" << std::endl << "LOOKUP_TABLE default" << std::endl;
        for (int i = 0; i < cellsCount; i++)
        {
            Prim prim = data[i].getPrim();
            fout << prim.t + 0.5 * prim.v2() / prim.cp << " ";
            if (i % 8 == 0  ||  i == cellsCount) {
                fout << std::endl;
            }
        }

        // Полный объём
        fout << std::endl << "SCALARS Volume double 1" << std::endl << "LOOKUP_TABLE default" << std::endl;
        for (int i = 0; i < cellsCount; i++)
        {
            fout << mesh->cells[i].volume << " ";
            if (i % 8 == 0  ||  i == cellsCount) {
                fout << std::endl;
            }
        }

        // Идентификаторы ячеек
        fout << std::endl << "SCALARS CellId double 1" << std::endl << "LOOKUP_TABLE default" << std::endl;
        for (int i = 0; i < cellsCount; i++)
        {
            fout << i << " ";
            if (i % 8 == 0  ||  i == cellsCount) {
                fout << std::endl;
            }
        }

        fout.close();
        std::cout << "File '" << fileName << "' is saved." << std::endl;
    }

}