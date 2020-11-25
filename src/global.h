/**
 * @author R.V.Zhalnin zhalnin@gmail.com
 */


#ifndef CHARM_3D_V2_GLOBAL_H
#define CHARM_3D_V2_GLOBAL_H

#include <cstdlib>
#include <vector>
#include <string>


namespace charm {

    typedef double              Real;
    typedef int                 Int;
    typedef unsigned int        Uint;
    typedef std::size_t         Index;
    typedef std::int8_t         Byte;
    typedef std::string         String;

    template<typename T>
    using Array = std::vector<T>;

    typedef Array<Int>          ArrayInt;
    typedef Array<Real>         ArrayReal;
    typedef Array<Index>        ArrayIndex;

    const Real      EPS         = 1.e-12;
    const Int       DIM         = 3;



    Real matr3Det(Real a[3][3]);
    void matr3Inv(Real a[3][3], Real a_inv[3][3]);

    void delQuotes(String &s);


    struct Tensor
    {
        Real xx;
        Real yy;
        Real zz;
        Real xy;
        Real xz;
        Real yz;
    };




    class Exception : std::exception {
    public:
        explicit Exception(String message): msg(message) {}

        String getMessage() { return msg; }

    protected:
        String msg;
    };
}
#endif //CHARM_3D_V2_GLOBAL_H
