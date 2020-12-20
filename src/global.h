/*
 *     ________  _____    ____  __  ___    _____ ____
 *    / ____/ / / /   |  / __ \/  |/  /   |__  // __ \  __    __
 *   / /   / /_/ / /| | / /_/ / /|_/ /     /_ </ / / /_/ /___/ /_
 *  / /___/ __  / ___ |/ _, _/ /  / /    ___/ / /_/ /_  __/_  __/
 *  \____/_/ /_/_/  |_/_/ |_/_/  /_/____/____/_____/ /_/   /_/
 *
 */


/**
 * @author R.V.Zhalnin zhalnin@gmail.com
 */


#ifndef CHARM_3D_V2_GLOBAL_H
#define CHARM_3D_V2_GLOBAL_H

#include <cstdlib>
#include <vector>
#include <string>
#include "mpi.h"

namespace charm {

    typedef double Real;
    typedef int Int;
    typedef unsigned int Uint;
    typedef std::size_t Index;
    typedef std::int8_t Byte;
    typedef std::string String;

    template<typename T>
    using Array = std::vector<T>;

    typedef Array<Byte> ArrayByte;
    typedef Array<Int> ArrayInt;
    typedef Array<Real> ArrayReal;
    typedef Array<Index> ArrayIndex;

    const Real EPS = 1.e-12;
    const Int DIM = 3;


    Real matr3Det(Real a[3][3]);

    void matr3Inv(Real a[3][3], Real a_inv[3][3]);

    void delQuotes(String &s);


    struct Tensor {
        Real xx;
        Real yy;
        Real zz;
        Real xy;
        Real xz;
        Real yz;
    };


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

        static void bcast(Int root, Int n, Real *data);

        static void bcast(Int root, Int n, Int *data);
        //static void bcast(Int tag, Int n, VECTOR* data);

        static void bcast(Int root, Int n, Index *x);

        static void min(Real *x);

        static void max(Real *x);

        static Int procCount;
        static Int procId;
        static Real *buf;

    };

    class Log {
    public:
        static void print(const String &str) { if (Parallel::isRoot()) std::cout << str; }
    };

    class Exception : std::exception {
    public:
        explicit Exception(String message) : msg(message) {}

        String getMessage() { return msg; }

    protected:
        String msg;
    };


    template<typename Base, typename T>
    inline bool instanceof(const T *ptr) {
        return dynamic_cast<const Base *>(ptr) != nullptr;
    }




}
#endif //CHARM_3D_V2_GLOBAL_H
