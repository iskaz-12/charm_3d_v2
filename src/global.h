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
#include <cmath>
#include <memory>
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

//    template<typename T>
//    using Ptr = std::shared_ptr<T>;
//
//    template <typename T, typename... Args>
//    inline Ptr<T> newPtr(Args&&... args) { return std::make_shared<T>(std::forward<Args>(args)...); }

    typedef Array<Byte> ArrayByte;
    typedef Array<Int> ArrayInt;
    typedef Array<Real> ArrayReal;
    typedef Array<Index> ArrayIndex;

    const Real EPS = 1.e-12;
    const Int DIM = 3;

    class Vector {
    public:
        union {
            struct {
                Real x, y, z;
            };
            Real coords[DIM];
        };

        Vector() : x(0.), y(0.), z(0.) {}

        Vector(Vector &p) {
            *this = p;
        }

        Vector(const Vector &p) {
            *this = p;
        }

        Vector(Real _x, Real _y, Real _z) : x(_x), y(_y), z(_z) {}

        Vector& operator = (const Vector &p) {
            x = p.x;
            y = p.y;
            z = p.z;
            return *this;
        }

        Vector& operator = (const Real a) {
            x = a;
            y = a;
            z = a;
            return *this;
        }

        inline Real &operator[](Int i) {
            return coords[i];
        }

        inline const Real &operator[](Int i) const {
            return coords[i];
        }

        inline Vector &operator+=(Vector &p) {
            for (auto i: {0,1,2}) { coords[i] += p.coords[i]; }
            return *this;
        }

        inline Vector &operator-=(Vector &p) {
            for (auto i: {0,1,2}) { coords[i] -= p.coords[i]; }
            return *this;
        }

        inline Vector &operator*=(Real a) {
            for (auto i: {0,1,2}) { coords[i] *= a; }
            return *this;
        }

        inline Vector &operator/=(Real a) {
            for (auto i: {0,1,2}) { coords[i] /= a; }
            return *this;
        }

        inline Real sqr() const {
            return x*x+y*y+z*z;
        }

        inline Real length() const {
            return ::sqrt(sqr());
        }

        friend Real scalarProd(const Vector &p1, const Vector &p2) {
            Real res = 0.;
            for (auto i: {0,1,2}) {
                res += p1[i] * p2[i];
            }
            return res;
        }

        friend Vector vectorProd(const Vector &p1, const Vector &p2) {
            Vector res;
            res.x = p1.y * p2.z - p1.z * p2.y;
            res.y = p1.z * p2.x - p1.x * p2.z;
            res.z = p1.x * p2.y - p1.y * p2.x;
            return res;
        }

        friend Real mixProd(const Vector &p1, const Vector &p2, const Vector &p3) {
            return scalarProd(p1, vectorProd(p2, p3));
        }

        static inline Index size() { return sizeof(Real)*3; }


    };

    typedef Array<Vector> ArrayVector;

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





    class Exception : std::exception {
    public:
        explicit Exception(const String &message) : msg(message) {}

        String getMessage() { return msg; }

    protected:
        String msg;
    };


    class NotImplementedException : public Exception {
    public:
        explicit NotImplementedException() : Exception("Calling of not implemented function") {}
    };


    template<typename Base, typename T>
    inline bool instanceof1(const T *ptr) {
        return dynamic_cast<const Base *>(ptr) != nullptr;
    }




}
#endif //CHARM_3D_V2_GLOBAL_H
