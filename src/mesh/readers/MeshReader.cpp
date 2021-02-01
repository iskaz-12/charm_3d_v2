//
// Created by zhrv on 13.11.2020.
//
#include <iostream>
#include "global.h"
#include "MeshReader.h"
#include "MeshReaderMsh22ASCII.h"
#include "metis.h"
#include "MeshDecompException.h"

namespace charm {



    const char FLAG_IN = 1;
    const char FLAG_EX = 2;




    MeshReader* MeshReader::create(Config* config) {
        // @todo
        return new MeshReaderMsh22ASCII(config);
    }

    void MeshReader::decomp(Mesh* mesh) {
        mesh->cCount = mesh->cCountGhost = mesh->cells.size();
        mesh->fCount = mesh->fCountGhost = mesh->faces.size();
        mesh->nCount = mesh->nCountGhost = mesh->nodes.size();

        if (Parallel::procCount < 2) {
            return;
        }

        idx_t ne, nn, ncommon, nparts, objval;
        Array<idx_t> eptr, eind, epart, npart;
        Int procCount   = Parallel::procCount;
        Int procId      = Parallel::procId;

        ne = mesh->cells.size();
        nn = mesh->nodes.size();
        nparts = procCount;
        ncommon = 3;

        eptr.clear();
        eind.clear();
        epart.resize(ne);
        npart.resize(nn);

        eptr.push_back(0);
        for (Int i = 0; i < ne; i++) {
            auto &c = mesh->cells[i];
            for (auto n : c.nodesInd) {
                eind.push_back(n); 
            }
            eptr.push_back(eind.size());
        }

        Int metisResult = METIS_PartMeshDual(&ne, &nn, eptr.data(), eind.data(), nullptr, nullptr,
                                             &ncommon, &nparts, nullptr, nullptr, &objval,
                                             epart.data(), npart.data());
        if (metisResult != METIS_OK) {
            throw Exception("Metis ERROR!!!");
        }

        std::vector<int> nProc(procCount, 0);
        for (int i = 0; i < ne; i++)
        {
            nProc[epart[i]]++;
        }

        ProcMesh pm;
        buildProcMesh(mesh, epart, pm);

        for (int p = 0; p < Parallel::procCount; p++) {
            if (p == Parallel::procId) {
                printf("PROC %d: cCount: %4lu; cCountGhost: %4lu\n", p, pm.cCount, pm.cCountEx);fflush(stdout);
                printf("-------------------------------\n");fflush(stdout);
                for (int i = 0; i < Parallel::procCount; i++) {
                    printf("\t%d>> SEND: %3zu    RECV: %3lu\n", i, pm.sendInd[i].size(), pm.recvCount[i]);fflush(stdout);
                    printf("\t\tCELLS TO SEND:");fflush(stdout);
                    for (auto j: pm.sendInd[i]) {
                        printf(" %3lu", j);fflush(stdout);
                    }
                    printf("\n");fflush(stdout);
                }

                printf("===============================\n\n");fflush(stdout);
            }
            Parallel::barrier();
        }




        Mesh locMesh;
        locMesh.nCount    = pm.nCount;
        locMesh.nCountGhost  = pm.nCountEx;
        locMesh.nodes.resize(pm.nCountEx);

        locMesh.fCount    = pm.fCount;
        locMesh.fCountGhost  = pm.fCountEx;
        locMesh.faces.resize(pm.fCountEx);

        locMesh.cCount    = pm.cCount;
        locMesh.cCountGhost  = pm.cCountEx;
        locMesh.cells.resize(pm.cCountEx);

//        locMesh.patchesCount = mesh->patchesCount;
//        locMesh.patches.assign(mesh->patches.begin(), mesh->patches.end());

        for (Index i = 0; i < pm.nCountEx; i++) {
            locMesh.nodes[i] = mesh->nodes[pm.gNodes[i]];
        }
        for (Index i = 0; i < pm.fCountEx; i++) {
            Face &gf = mesh->faces[pm.gFaces[i]];
            Face &lf = locMesh.faces[i];
            lf = gf;
            lf.nodesInd.clear();
            for (auto j : gf.nodesInd) {
                lf.nodesInd.push_back(pm.lNodes[j]);
            }
            lf.cells.clear();
            for (auto j : gf.cells) {
                lf.cells.push_back(pm.lCells[j]);
            }
        }

        for (Index i = 0; i < pm.cCountEx; i++) {
            Cell &gc = mesh->cells[pm.gCells[i]];
            Cell &lc = locMesh.cells[i];
            lc = gc;
            lc.nodesInd.clear();
            for (auto j : gc.nodesInd) {
                lc.nodesInd.push_back(pm.lNodes[j]);
            }
            lc.facesInd.clear();
            for (auto j : gc.facesInd) {
                lc.facesInd.push_back(pm.lFaces[j]);
            }
        }

        locMesh.recvCount.assign(pm.recvCount.begin(), pm.recvCount.end());
        locMesh.recvShift.resize(pm.recvCount.size());
        locMesh.sendInd.resize(pm.sendInd.size());

        for (Index i = 0; i < locMesh.sendInd.size(); i++) {
            if (i == 0) {
                locMesh.recvShift[i] = locMesh.cCount;
            }
            else {
                locMesh.recvShift[i] = locMesh.recvShift[i-1]+locMesh.recvCount[i-1];
            }
            locMesh.sendInd[i].assign(pm.sendInd[i].begin(), pm.sendInd[i].end());
        }
        mesh->assign(locMesh);

    }



    void MeshReader::buildProcMesh(Mesh* mesh, const Array<idx_t> &epart, ProcMesh &pm) {
        ArrayInt procCellEx;
        idx_t ne = epart.size();

        if (ne != mesh->cells.size()) {
            throw MeshDecompException("Wrong number of cells.");
        }

        for (int i = 0; i < ne; i++)
        {
            if (epart[i] == Parallel::procId)
            {
                pm.gCells.push_back(i);
            }
        }
        for (Index gCell : pm.gCells)
        {
            Cell& c = mesh->cells[gCell];
//            ArrayIndex neigh = mesh->cellGetNeighIndexes(gCell);

            for (auto j : mesh->cellGetNeighIndexes(gCell))
            {
                if ((epart[j] != Parallel::procId)) procCellEx.push_back(j);
            }
        }


        Int exCnt = procCellEx.size();
        // сортировка фиктивных ячеек в порядке убывания процессора
        for (int ii = 0; ii < exCnt; ii++) {
            for (int j = 0; j < exCnt - 1 - ii; j++) {
                if (epart[procCellEx[j]] > epart[procCellEx[j + 1]]) {
                    int tmp = procCellEx[j];
                    procCellEx[j] = procCellEx[j + 1];
                    procCellEx[j + 1] = tmp;
                }
            }
        }

        pm.cCount = pm.gCells.size();
        pm.cCountEx = pm.cCount + exCnt;
        for (size_t i = 0; i < exCnt; i++)
        {
            pm.gCells.push_back(procCellEx[i]);
        }


        pm.recvCount.resize(Parallel::procCount);
        pm.sendInd.resize(Parallel::procCount);
        for (int i = 0; i < Parallel::procCount; i++) {
            pm.recvCount[i] = 0;
            pm.sendInd[i].clear();
        }
        for (int i = 0; i < pm.cCountEx; i++) {
            pm.lCells[pm.gCells[i]] = i;
        }
        for (int i = pm.cCount; i < pm.cCountEx; i++) {
            pm.recvCount[epart[pm.gCells[i]]]++;
        }

//        ArrayInt procEx;
//        for (int i = pm.cCount; i < pm.cCountGhost; i++) {
//            procEx.push_back(pm.gCells[i]);
//        }

        for (Int p = 0; p < Parallel::procCount; p++) {

            if (p == Parallel::procId) {
                Parallel::bcast(p, 1, &exCnt);
                Parallel::bcast(p, exCnt, procCellEx.data());
            }
            else {
                Int nEx;
                Parallel::bcast(p, 1, &nEx);
                ArrayInt procExP;
                procExP.resize(nEx);
                Parallel::bcast(p, nEx, procExP.data());
                for (auto giCell: procExP) {
                    if (epart[giCell] == Parallel::procId) {
                        pm.sendInd[p].push_back(pm.lCells[giCell]);
                    }
                }
            }
            Parallel::barrier();
        }

        procCellEx.clear();


        char * faceFlg = new char[mesh->faces.size()];
        memset(faceFlg, 0, mesh->faces.size()*sizeof(char));
        for (int i = 0; i < pm.cCount; i++)
        {
            for (unsigned long j : mesh->cells[pm.gCells[i]].facesInd)
            {
                faceFlg[j] |= FLAG_IN;
            }
        }
        for (int i = pm.cCount; i < pm.cCountEx; i++)
        {
            for (unsigned long j : mesh->cells[pm.gCells[i]].facesInd)
            {
                faceFlg[j] |= FLAG_EX;
            }
        }

        for (int i = 0; i < mesh->faces.size(); i++)
        {
            if (faceFlg[i] & FLAG_IN) pm.gFaces.push_back(i);
        }
        pm.fCount = pm.gFaces.size();
        for (int i = 0; i < mesh->faces.size(); i++)
        {
            if (((faceFlg[i] & FLAG_EX) == FLAG_EX) && ((faceFlg[i] & FLAG_IN) == 0)) pm.gFaces.push_back(i);
        }
        pm.fCountEx = pm.gFaces.size();
        delete[] faceFlg;
        for (int i = 0; i < pm.fCountEx; i++) {
            pm.lFaces[pm.gFaces[i]] = i;
        }

        char * nodeFlg = new char[mesh->nodes.size()];
        memset(nodeFlg, 0, mesh->nodes.size()*sizeof(char));
        for (int i = 0; i < pm.cCount; i++)
        {
            for (unsigned long j : mesh->cells[pm.gCells[i]].nodesInd)
            {
                nodeFlg[j] |= FLAG_IN;
            }
        }
        for (int i = pm.cCount; i < pm.cCountEx; i++)
        {
            for (unsigned long j : mesh->cells[pm.gCells[i]].nodesInd)
            {
                nodeFlg[j] |= FLAG_EX;
            }
        }

        for (int i = 0; i < mesh->nodes.size(); i++) {
            if (nodeFlg[i] & FLAG_IN) pm.gNodes.push_back(i);
        }
        pm.nCount = pm.gNodes.size();
        for (int i = 0; i < mesh->nodes.size(); i++)
        {
            if (((nodeFlg[i] & FLAG_EX) == FLAG_EX) && ((nodeFlg[i] & FLAG_IN) == 0)) pm.gNodes.push_back(i);
        }
        pm.nCountEx = pm.gNodes.size();
        delete[] nodeFlg;
        for (int i = 0; i < pm.nCountEx; i++) {
            pm.lNodes[pm.gNodes[i]] = i;
        }

    }
}
