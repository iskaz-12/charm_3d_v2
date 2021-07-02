/*
 *     ________  _____    ____  __  ___    _____ ____
 *    / ____/ / / /   |  / __ \/  |/  /   |__  // __ \  __    __
 *   / /   / /_/ / /| | / /_/ / /|_/ /     /_ </ / / /_/ /___/ /_
 *  / /___/ __  / ___ |/ _, _/ /  / /    ___/ / /_/ /_  __/_  __/
 *  \____/_/ /_/_/  |_/_/ |_/_/  /_/____/____/_____/ /_/   /_/
 *
 */

#ifndef CHARM_3D_V2_PARALLEL_H
#define CHARM_3D_V2_PARALLEL_H

#include "global.h"

namespace charm {

    struct Parallel {
        static void init(Int *argc, char ***argv);

        static void done();

        static bool isRoot() { return (procId == 0); }

        static void barrier() { MPI_Barrier(MPI_COMM_WORLD); }

        static void send(Int pid, Int tag, Int n, Real *data);

        static void send(Int pid, Int tag, Int n, Int *data);

        static void send(Int pid, Int tag, Int n, Byte *data);

        static void recv(Int pid, Int tag, Int n, Real *data);

        static void recv(Int pid, Int tag, Int n, Int *data);

        static void recv(Int pid, Int tag, Int n, Byte *data);

        static void send(Int pid, Int tag, Int n, Vector *data);

        static void recv(Int pid, Int tag, Int n, Vector *data);

        static void bcast(Int root, Int n, Vector *data);

        static void bcast(Int root, Int n, Real *data);

        static void bcast(Int root, Int n, Int *data);
        //static void bcast(Int tag, Int n, VECTOR* data);

        static void bcast(Int root, Int n, Index *x);

        static void sum(Real *x);

        static void min(Real *x);

        static void max(Real *x);

        static void exchange(ArrayReal &field);

        static void exchange(ArrayVector &field);

        static void exchange(Array<ArrayReal> &field);

        static void exchange(Array <ArrayVector> &field);

        static Int procCount;
        static Int procId;

    };

}
#endif //CHARM_3D_V2_PARALLEL_H
