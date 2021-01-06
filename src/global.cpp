/*
 *     ________  _____    ____  __  ___    _____ ____
 *    / ____/ / / /   |  / __ \/  |/  /   |__  // __ \  __    __
 *   / /   / /_/ / /| | / /_/ / /|_/ /     /_ </ / / /_/ /___/ /_
 *  / /___/ __  / ___ |/ _, _/ /  / /    ___/ / /_/ /_  __/_  __/
 *  \____/_/ /_/_/  |_/_/ |_/_/  /_/____/____/_____/ /_/   /_/
 *
 */


/**
 * Created by zhrv on 17.11.2020.
 * @author R.V.Zhalnin, zhalnin@gmail.com
 */

#include "global.h"

namespace charm {

    void delQuotes(String &s) {
        Index pos = 0;
        while ((pos = s.find("\"", pos)) != String::npos) {
            s.erase(pos, 1);
        }
    }


    Real matr3Det(Real a[3][3])
    {
        Real det_a = 0.;

        det_a += a[0][0] * a[1][1] * a[2][2];
        det_a += a[0][2] * a[1][0] * a[2][1];
        det_a += a[2][0] * a[0][1] * a[1][2];
        det_a -= a[0][2] * a[1][1] * a[2][0];
        det_a -= a[0][0] * a[1][2] * a[2][1];
        det_a -= a[0][1] * a[1][0] * a[2][2];

        return det_a;
    }

    void matr3Inv(Real a[3][3], Real a_inv[3][3])
    {
        Real a_[3][3];
        int i, j;
        Real det_a = matr3Det(a);

        //CHARM_ASSERT(det_a != 0.);

        a_[0][0] = a[1][1] * a[2][2] - a[1][2] * a[2][1];
        a_[0][1] = -a[0][1] * a[2][2] + a[0][2] * a[2][1];
        a_[0][2] = a[0][1] * a[1][2] - a[0][2] * a[1][1];

        a_[1][0] = -a[1][0] * a[2][2] + a[1][2] * a[2][0];
        a_[1][1] = a[0][0] * a[2][2] - a[0][2] * a[2][0];
        a_[1][2] = -a[0][0] * a[1][2] + a[0][2] * a[1][0];

        a_[2][0] = a[1][0] * a[2][1] - a[1][1] * a[2][0];
        a_[2][1] = -a[0][0] * a[2][1] + a[0][1] * a[2][0];
        a_[2][2] = a[0][0] * a[1][1] - a[0][1] * a[1][0];

        for (i = 0; i < 3; i++) {
            for (j = 0; j < 3; j++) {
                a_inv[i][j] = a_[i][j] / det_a;
            }
        }
    }


    MPI_Status mpiSt;
    Int Parallel::procCount = 0;
    Int Parallel::procId = 0;
    Real * Parallel::buf;

    void Parallel::init(Int* argc, char*** argv)
    {
        MPI_Init(argc, argv);
        MPI_Comm_size(MPI_COMM_WORLD, &procCount);
        MPI_Comm_rank(MPI_COMM_WORLD, &procId);
    }

    void Parallel::done()
    {
        MPI_Finalize();
    }

    void Parallel::send(Int pid, Int tag, Int n, Real* x)
    {
        MPI_Send(x, n, MPI_DOUBLE, pid, tag, MPI_COMM_WORLD);
    }

    void Parallel::send(Int pid, Int tag, Int n, Int* x)
    {
        MPI_Send(x, n, MPI_INT, pid, tag, MPI_COMM_WORLD);
    }


    void Parallel::recv(Int pid, Int tag, Int n, Real* x)
    {
        MPI_Recv(x, n, MPI_DOUBLE, pid, tag, MPI_COMM_WORLD, &mpiSt);
    }

    void Parallel::recv(Int pid, Int tag, Int n, Int* x)
    {
        MPI_Recv(x, n, MPI_INT, pid, tag, MPI_COMM_WORLD, &mpiSt);
    }

    void Parallel::bcast(Int root, Int n, Int* x) {
        MPI_Bcast(x, n, MPI_INT, root, MPI_COMM_WORLD);
    }

    void Parallel::bcast(Int root, Int n, Index* x) {
        MPI_Bcast(x, n, MPI_LONG_INT, root, MPI_COMM_WORLD);
    }

    void Parallel::bcast(Int root, Int n, Real* x)
    {
        MPI_Bcast(x, n, MPI_DOUBLE, root, MPI_COMM_WORLD);
    }

    void Parallel::send(Int pid, Int tag, Int n, Byte *data) {
        MPI_Send(data, n, MPI_CHAR, pid, tag, MPI_COMM_WORLD);
    }

    void Parallel::recv(Int pid, Int tag, Int n, Byte *data) {
        MPI_Recv(data, n, MPI_CHAR, pid, tag, MPI_COMM_WORLD, &mpiSt);
    }

    void Parallel::send(Int pid, Int tag, Int n, Vector *data) {
        ArrayReal x(n*3);
        for (Index i = 0; i < n; i++) {
            x[i*3+0] = data[i][0];
            x[i*3+1] = data[i][1];
            x[i*3+2] = data[i][2];
        }
        MPI_Send(x.data(), n*3, MPI_DOUBLE, pid, tag, MPI_COMM_WORLD);
    }

    void Parallel::recv(Int pid, Int tag, Int n, Vector *data) {
        ArrayReal x(n*3);
        MPI_Recv(x.data(), n*3, MPI_DOUBLE, pid, tag, MPI_COMM_WORLD, &mpiSt);
        for (Index i = 0; i < n; i++) {
            data[i][0] = x[i*3+0];
            data[i][1] = x[i*3+1];
            data[i][2] = x[i*3+2];
        }
    }

    void Parallel::bcast(Int root, Int n, Vector* data)
    {
        ArrayReal x(n*3);
        if (procId == root) {
            for (Index i = 0; i < n; i++) {
                x[i * 3 + 0] = data[i][0];
                x[i * 3 + 1] = data[i][1];
                x[i * 3 + 2] = data[i][2];
            }
        }
        MPI_Bcast(x.data(), n, MPI_DOUBLE, root, MPI_COMM_WORLD);
        if (procId != root) {
            for (Index i = 0; i < n; i++) {
                data[i][0] = x[i * 3 + 0];
                data[i][1] = x[i * 3 + 1];
                data[i][2] = x[i * 3 + 2];
            }
        }
    }

    void Parallel::min(Real *x) {
        Real tmp;
        MPI_Allreduce(x, &tmp, 1, MPI_DOUBLE, MPI_MIN, MPI_COMM_WORLD);
        *x = tmp;
    }

    void Parallel::max(Real *x) {
        Real tmp;
        MPI_Allreduce(x, &tmp, 1, MPI_DOUBLE, MPI_MIN, MPI_COMM_WORLD);
        *x = tmp;
    }

    void Parallel::sum(Real *x) {
        Real tmp;
        MPI_Allreduce(x, &tmp, 1, MPI_DOUBLE, MPI_SUM, MPI_COMM_WORLD);
        *x = tmp;
    }


//        void matrInv(charm_matr_t a_src, charm_matr_t am)
//        {
//            int *mask;
//            Real fmaxval;
//            int maxind;
//            int tmpi;
//            Real tmp;
//            Real **a;
//            int N = CHARM_BASE_FN_COUNT;
//            int i, j, ni, nj;
//
//            mask = (int *) malloc(N * sizeof(int));//   new int[N];
//            a = (Real **) malloc(N * sizeof(Real * )); //new Real*[N];
//            for (i = 0; i < N; i++) {
//                a[i] = (Real *) malloc(N * sizeof(Real)); //new Real[N];
//                for (j = 0; j < N; j++) {
//                    a[i][j] = a_src[i][j];
//                }
//            }
//
//            for (i = 0; i < N; i++) {
//                for (j = 0; j < N; j++) {
//                    if (i == j) {
//                        am[i][j] = 1.0;
//                    } else {
//                        am[i][j] = 0.0;
//                    }
//                }
//            }
//            for (i = 0; i < N; i++) {
//                mask[i] = i;
//            }
//            for (i = 0; i < N; i++) {
//                maxind = i;
//                fmaxval = fabs(a[i][i]);
//                for (ni = i + 1; ni < N; ni++) {
//                    if (fabs(fmaxval) <= fabs(a[ni][i])) {
//                        fmaxval = fabs(a[ni][i]);
//                        maxind = ni;
//                    }
//                }
//                fmaxval = a[maxind][i];
//                CHARM_ASSERT(fmaxval != 0);
//                if (i != maxind) {
//                    for (nj = 0; nj < N; nj++) {
//                        tmp = a[i][nj];
//                        a[i][nj] = a[maxind][nj];
//                        a[maxind][nj] = tmp;
//
//                        tmp = am[i][nj];
//                        am[i][nj] = am[maxind][nj];
//                        am[maxind][nj] = tmp;
//                    }
//                    tmpi = mask[i];
//                    mask[i] = mask[maxind];
//                    mask[maxind] = tmpi;
//                }
//                Real aii = a[i][i];
//                for (j = 0; j < N; j++) {
//                    a[i][j] = a[i][j] / aii;
//                    am[i][j] = am[i][j] / aii;
//                }
//                for (ni = 0; ni < N; ni++) {
//                    if (ni != i) {
//                        Real fconst = a[ni][i];
//                        for (nj = 0; nj < N; nj++) {
//                            a[ni][nj] = a[ni][nj] - fconst * a[i][nj];
//                            am[ni][nj] = am[ni][nj] - fconst * am[i][nj];
//                        }
//                    }
//                }
//            }
//            /**/
//            for (i = 0; i < N; i++) {
//                if (mask[i] != i) {
//                    for (j = 0; j < N; j++) {
//                        tmp = a[i][j];
//                        a[i][j] = a[mask[i]][j];
//                        a[mask[i]][j] = tmp;
//                    }
//                }
//            }
//            /**/
//            for (i = 0; i < N; i++) {
//                free(a[i]);
//            }
//            free(a);
//            free(mask);
//
//        }

}