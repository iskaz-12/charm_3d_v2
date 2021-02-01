/**
 * Created by zhrv on 18.11.2020.
 * @author R.V.Zhalnin, zhalnin@gmail.com
 */

#include "Method.h"
#include "MethodException.h"

namespace charm {
    Method::Method(Config* _conf): conf(_conf) {
        mesh = conf->mesh;
    }


    Mesh* Method::getMesh() const {
        return mesh;
    }


    Config* Method::getConfig() const {
        return conf;
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
    }


    void Method::exchange(ArrayVector &field) {
        if (field.size() != mesh->cCountGhost) {
            throw MethodException("Wrong fields count for exchange.");
        }
        for (int p = 0; p < Parallel::procCount; p++) {
            if (p < Parallel::procId) {
                if (mesh->recvCount[p] > 0) {
                    ArrayReal buf(mesh->recvCount[p]*3);
                    Parallel::recv(p, 0, mesh->recvCount[p]*3, buf.data());
                    for (Index i = 0; i < mesh->recvCount[p]; i++) {
                        field[mesh->recvShift[p]+i][0] = buf[i*3+0];
                        field[mesh->recvShift[p]+i][1] = buf[i*3+1];
                        field[mesh->recvShift[p]+i][2] = buf[i*3+2];
                    }
                }
                int n = mesh->sendInd[p].size();
                if (n > 0) {
                    ArrayReal buf(n*3);
                    for (int i = 0; i < n; i++) {
                        buf[i*3+0] = field[mesh->sendInd[p][i]][0];
                        buf[i*3+1] = field[mesh->sendInd[p][i]][1];
                        buf[i*3+2] = field[mesh->sendInd[p][i]][2];
                    }
                    Parallel::send(p, 1, n, buf.data());
                }
            }
            else if (p > Parallel::procId) {
                int n = mesh->sendInd[p].size();
                if (n > 0) {
                    ArrayReal buf(n*3);
                    for (int i = 0; i < n; i++) {
                        buf[i*3+0] = field[mesh->sendInd[p][i]][0];
                        buf[i*3+1] = field[mesh->sendInd[p][i]][1];
                        buf[i*3+2] = field[mesh->sendInd[p][i]][2];
                    }
                    Parallel::send(p, 0, n, buf.data());
                }
                if (mesh->recvCount[p] > 0) {
                    ArrayReal buf(mesh->recvCount[p]*3);
                    Parallel::recv(p, 1, mesh->recvCount[p], buf.data());
                    for (Index i = 0; i < mesh->recvCount[p]; i++) {
                        field[mesh->recvShift[p]+i][0] = buf[i*3+0];
                        field[mesh->recvShift[p]+i][1] = buf[i*3+1];
                        field[mesh->recvShift[p]+i][2] = buf[i*3+2];
                    }
                }
            }
        }
        Parallel::barrier();

    }


    void Method::exchange(ArrayReal &field) {
        if (field.size() != mesh->cCountGhost) {
            throw MethodException("Wrong fields count for exchange.");
        }
        for (int p = 0; p < Parallel::procCount; p++) {
            if (p < Parallel::procId) {
                if (mesh->recvCount[p] > 0) {
                    ArrayReal buf(mesh->recvCount[p]);
                    Parallel::recv(p, 0, mesh->recvCount[p], buf.data());
                    for (Index i = 0; i < mesh->recvCount[p]; i++) {
                        field[mesh->recvShift[p]+i] = buf[i];
                    }
                }
                int n = mesh->sendInd[p].size();
                if (n > 0) {
                    ArrayReal buf(n);
                    for (int i = 0; i < n; i++) {
                        buf[i] = field[mesh->sendInd[p][i]];
                    }
                    Parallel::send(p, 1, n, buf.data());
                }
            }
            else if (p > Parallel::procId) {
                int n = mesh->sendInd[p].size();
                if (n > 0) {
                    ArrayReal buf(n);
                    for (int i = 0; i < n; i++) {
                        buf[i] = field[mesh->sendInd[p][i]];
                    }
                    Parallel::send(p, 0, n, buf.data());
                }
                if (mesh->recvCount[p] > 0) {
                    ArrayReal buf(mesh->recvCount[p]);
                    Parallel::recv(p, 1, mesh->recvCount[p], buf.data());
                    for (Index i = 0; i < mesh->recvCount[p]; i++) {
                        field[mesh->recvShift[p]+i] = buf[i];
                    }
                }
            }
        }
        Parallel::barrier();

    }


    void Method::exchange(Array<ArrayVector> &field) {
        if (field.size() != mesh->getCellsCountWithGhost()) {
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
