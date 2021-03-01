/*
 *     ________  _____    ____  __  ___    _____ ____
 *    / ____/ / / /   |  / __ \/  |/  /   |__  // __ \  __    __
 *   / /   / /_/ / /| | / /_/ / /|_/ /     /_ </ / / /_/ /___/ /_
 *  / /___/ __  / ___ |/ _, _/ /  / /    ___/ / /_/ /_  __/_  __/
 *  \____/_/ /_/_/  |_/_/ |_/_/  /_/____/____/_____/ /_/   /_/
 *
 */


#include "Component.h"
#include <cmath>

namespace charm {
    Real Component::calcCp(Real t) {
        Real res = 0.;
        Real tt  = 1.;
        if (cpType == CP_CONST) {
            return cp[0];
        }
        else if (cpType == CP_POLYNOM) {
            for (auto cpi: cp){
                res += tt*(cpi);
                tt *= t;
            }
            return res;
        }
        else {
            return cp[0];
        }

    }

    Real Component::calcCpDt(Real t) {
        if (cpType != CP_POLYNOM) {
            throw Exception("Wrong call of function 'CALC_CP_DT'");
        }

        Real res = 0.;
        Real tt  = 1.;
        for (Index i= 1; i < cp.size(); i++) {
            res += tt*(cp[i])*i;
            tt *= t;
        }
        return res;
    }

    Real Component::calcH(Real t) {
        double tRef = 298.15; // @todo
//        double t1,t2;
        Real h = h0;
        for (int i = 0; i < cp.size(); i++){
//            t2 = exp((i+1)*std::log(t))/(i+1);
//            t1 = exp((i+1)*std::log(tRef))/(i+1);
            h += cp[i]*(std::pow(t, i+1.)-std::pow(tRef, i+1.))/(i+1.);
        }
        return h;
    }

    Real Component::calcKp(Real t) const {
        if (kpType == KP_CONST) {
            return kp0;
        }
        else if (kpType == KP_SATHERLAND) {
            return kp0 * sqrt( pow( t / t0, 3 ) ) * ( t0 + ts ) / ( t + ts );
        }
        else {
            return kp0;
        }
    }

    Real Component::calcMl(Real t) const {
        if (mlType == ML_CONST) {
            return ml0;
        }
        else if (mlType == ML_SATHERLAND) {
            return ml0 * sqrt( pow( t / t0, 3 ) ) * ( t0 + ts ) / ( t + ts );
        }
        else {
            return ml0;
        }
    }


}