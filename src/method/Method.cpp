/**
 * Created by zhrv on 18.11.2020.
 * @author R.V.Zhalnin, zhalnin@gmail.com
 */

#include <MethodFvm.h>
#include <iostream>
#include "Method.h"

namespace charm {
    Method::Method(Config *_conf, Mesh *_mesh): conf(_conf), mesh(_mesh) {}

    Method* Method::create(Config *conf, Mesh *mesh) {
        if (conf->methodName == "CHARM_FVM") {
            return new MethodFvm(conf, mesh);
        }
        else {
            std::cerr << "Wrong method!!!\n";
            exit(1);
        }
    }


    void Method::exchange() {

        Index dataSize = getData(0)->size();
        ArrayByte dbuf(dataSize);
        for (int p = 0; p < Parallel::procCount; p++) {
            if (p < Parallel::procId) {
                if (mesh->recvCount[p] > 0) {
                    ArrayByte buf(mesh->recvCount[p]*dataSize);
                    Parallel::recv(p, 0, mesh->recvCount[p]*dataSize, buf.data());
                    for (Index i = 0; i < mesh->recvCount[p]; i++) {
                        getData(mesh->recvShift[p]+i)->setBuffer(&buf[i*dataSize]);
                    }
                }
                int n = mesh->sendInd[p].size();
                if (n > 0) {
                    ArrayByte buf(n*dataSize);
                    for (int i = 0; i < n; i++) {
                        getData(mesh->sendInd[p][i])->getBuffer(dbuf.data());
                        memcpy(&buf[i*dataSize], dbuf.data(), dataSize);
                    }
                    Parallel::send(p, 1, n*dataSize, buf.data());
                }
            }
            else if (p > Parallel::procId) {
                int n = mesh->sendInd[p].size();
                if (n > 0) {
                    ArrayByte buf(n*dataSize);
                    for (int i = 0; i < n; i++) {
                        getData(mesh->sendInd[p][i])->getBuffer(dbuf.data());
                        memcpy(&buf[i*dataSize], dbuf.data(), dataSize);
                    }
                    Parallel::send(p, 0, n*dataSize, buf.data());
                }
                if (mesh->recvCount[p] > 0) {
                    ArrayByte buf(mesh->recvCount[p]*dataSize);
                    Parallel::recv(p, 1, mesh->recvCount[p]*dataSize, buf.data());
                    for (Index i = 0; i < mesh->recvCount[p]; i++) {
                        getData(mesh->recvShift[p]+i)->setBuffer(&buf[i*dataSize]);
                    }
                }
            }
        }
        Parallel::barrier();
//        Log::print("OK!!!!!!!\n");
//        Parallel::done();
//        exit(0);
    }
}