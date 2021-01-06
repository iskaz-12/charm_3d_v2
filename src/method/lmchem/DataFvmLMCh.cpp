/*
 *     ________  _____    ____  __  ___    _____ ____
 *    / ____/ / / /   |  / __ \/  |/  /   |__  // __ \  __    __
 *   / /   / /_/ / /| | / /_/ / /|_/ /     /_ </ / / /_/ /___/ /_
 *  / /___/ __  / ___ |/ _, _/ /  / /    ___/ / /_/ /_  __/_  __/
 *  \____/_/ /_/_/  |_/_/ |_/_/  /_/____/____/_____/ /_/   /_/
 *
 */


//
// Created by zhrv on 15.12.2020.
//

#include <Config.h>
#include "DataFvmLMCh.h"
#include "MethodFvmLMChException.h"

namespace charm {

    using Cons = DataFvmLMCh::Cons;




    Cons::Cons(Index compCount) :
            ru(0.), rv(0.), rw(0.), rh(0.), rc(compCount) {}


    Cons::Cons(const Cons &cons) {
        *this = cons;
    }

    Cons &Cons::operator=(const Cons &cons) {
        ru = cons.ru;
        rv = cons.rv;
        rw = cons.rw;
        rh = cons.rh;
        rc.assign(cons.rc.begin(), cons.rc.end());
        return *this;
    }

    Cons &Cons::operator=(const Real &a) {
        ru = a;
        rv = a;
        rw = a;
        rh = a;
        rc.assign(rc.size(), a);
        return *this;
    }

    void Cons::normalize() {
        if (::fabs(ru) < EPS) ru = 0.;
        if (::fabs(rv) < EPS) rv = 0.;
        if (::fabs(rw) < EPS) rw = 0.;
        if (::fabs(rh) < EPS) rh = 0.;
        for (auto &c: rc) {
            if (::fabs(c) < EPS) c = 0.;
        }
    }

    Cons &Cons::operator+=(const Cons &cons) {
        ru += cons.ru;
        rv += cons.rv;
        rw += cons.rw;
        rh += cons.rh;
        for (Index i = 0; i < rc.size(); i++) {
            rc[i] += cons.rc[i];
        }
        return *this;
    }

    Cons &Cons::operator-=(const Cons &cons) {
        ru -= cons.ru;
        rv -= cons.rv;
        rw -= cons.rw;
        rh -= cons.rh;
        for (Index i = 0; i < rc.size(); i++) {
            rc[i] -= cons.rc[i];
        }
        return *this;
    }


    Cons &Cons::operator*=(const Real &a) {
        ru *= a;
        rv *= a;
        rw *= a;
        rh *= a;
        for (double &rci : rc) {
            rci *= a;
        }
        return *this;
    }


    inline Index Cons::size() const {
        return sizeof(Real) * (4 + rc.size());
    }


    inline void Cons::shrink() {
        rc.shrink_to_fit();
    }


    /**
     *
     * @return
     */
    Prim DataFvmLMCh::getPrim() {
        Index cCount = Config::getCompCount();
        Prim prim(cCount);
        prim.matId = matId;
        prim.r = 0.;
        for (Index i = 0; i < cCount; i++) {
            prim.r += c.rc[i];
        }
        prim.v.x = c.ru / prim.r;
        prim.v.y = c.rv / prim.r;
        prim.v.z = c.rw / prim.r;
        prim.h = c.rh / prim.r;
        prim.p0 = p0;
        prim.p  = p;

        //prim.c.resize(cCount);
        for (Index i = 0; i < cCount; i++) {
            prim.c[i] = c.rc[i] / prim.r;
            if (prim.c[i] < 0.) prim.c[i] = 0.; // @todo
            if (prim.c[i] > 1.) prim.c[i] = 1.;
        }
        prim.eos(Material::EOS_LOW_MACH_R_TO_T_E);
        prim.eTot = prim.e + 0.5 * prim.v2();
        prim.ml = 0.;
        prim.kp = 0.;
        Real sm = 0.;
        for (Index i = 0; i < cCount; i++) {
            Component *comp = Config::getComponent(i);
            Real cm = prim.c[i]/comp->m;
            sm += cm;
            prim.ml += cm * comp->calcMl(prim.t);
            prim.kp += prim.c[i] * comp->calcKp(prim.t);
        }
        prim.ml /= sm;
        prim.shrink();
        return prim;
    }


    /**
     *
     * @param p
     */
    void DataFvmLMCh::getPrim(Prim &p) {
        p = getPrim();
    }


    /**
     *
     * @return
     */
    Cons &DataFvmLMCh::getCons() {
        return c;
    }

    /**
     *
     * @param prim
     */
    void DataFvmLMCh::setCons(const Prim &prim) {
        Index count = prim.c.size();
        size_t i;
        matId = prim.matId;
        p0 = prim.p0;
        p = prim.p;
        c.ru = prim.r * prim.v.x;
        c.rv = prim.r * prim.v.y;
        c.rw = prim.r * prim.v.z;
        c.rh = prim.r * prim.h;
        c.rc.resize(count);
        for (Index i = 0; i < count; i++) {
            c.rc[i] = prim.r * prim.c[i];
        }
        c.shrink();
    }


    inline Index DataFvmLMCh::size() const {
        Index res = 0;
        res += c.size();
        res += 2*sizeof(Real);
        res += sizeof(Index);

        return res;
    }

    void DataFvmLMCh::getBuffer(Byte *buf) {
        Index shift = 0;
        *(Real *) (&buf[shift]) = c.ru; shift += sizeof(Real);
        *(Real *) (&buf[shift]) = c.rv; shift += sizeof(Real);
        *(Real *) (&buf[shift]) = c.rw; shift += sizeof(Real);
        *(Real *) (&buf[shift]) = c.rh; shift += sizeof(Real);
        for (Real rc : c.rc) {
            *(Real *) (&buf[shift]) = rc; shift += sizeof(Real);
        }

        *(Real *) (&buf[shift]) = p0; shift += sizeof(Real);
        *(Real *) (&buf[shift]) = p; shift += sizeof(Real);

        *(Index *) (&buf[shift]) = matId; shift += sizeof(Index);

        if (shift != size()) {
            throw DataException("Size of copied to buffer data is diferent from data size.");
        }
    }

    void DataFvmLMCh::setBuffer(Byte *buf) {
        Index shift = 0;
        c.ru = *(Real *) (&buf[shift]); shift += sizeof(Real);
        c.rv = *(Real *) (&buf[shift]); shift += sizeof(Real);
        c.rw = *(Real *) (&buf[shift]); shift += sizeof(Real);
        c.rh = *(Real *) (&buf[shift]); shift += sizeof(Real);
        for (double &rc : c.rc) {
            rc = *(Real *) (&buf[shift]); shift += sizeof(Real);
        }

        p0 = *(Real *) (&buf[shift]);  shift += sizeof(Real);
        p = *(Real *) (&buf[shift]); shift += sizeof(Real);

        matId = *(Index *) (&buf[shift]); shift += sizeof(Index);
        if (shift != size()) {
            throw DataException("Size of copied from buffer data is diferent from data size.");
        }
    }

    Index DataFvmLMCh::getScalarFieldsCount() {
        return 5 + Config::getCompCount();
    }

    String DataFvmLMCh::getScalarFieldName(Index iFld) {
        if (iFld == 0) {
            return "Density";
        } else if (iFld == 1) {
            return "Pressure";
        } else if (iFld == 2) {
            return "Energy";
        } else if (iFld == 3) {
            return "Tot_Energy";
        } else if (iFld == 4) {
            return "Temperature";
        } else if (iFld == 5) {
            return "Mach_num";
        }
        for (Index i = 0; i < Config::getCompCount(); i++) {
            if (iFld == 5 + i) {
                return "C_" + Config::getComponent(i)->name;
            }
        }

        throw Exception("Wrong field number");
    }

    Real DataFvmLMCh::getScalarFieldValue(Index iFld) {
        Prim prim = getPrim();
        if (iFld == 0) {
            return prim.r;
        } else if (iFld == 1) {
            return prim.p + prim.p0;
        } else if (iFld == 2) {
            return prim.e;
        } else if (iFld == 3) {
            return prim.eTot;
        } else if (iFld == 4) {
            return prim.t;
        } else if (iFld == 5) {
            return prim.v.length() / prim.cz;
        }
        for (Index i = 0; i < Config::getCompCount(); i++) {
            if (iFld == 5 + i) {
                return prim.c[i];
            }
        }

        throw Exception("Wrong field number");
    }

    Index DataFvmLMCh::getVectorFieldsCount() {
        return 1;
    }

    String DataFvmLMCh::getVectorFieldName(Index) {
        return "Velosity";
    }

    Vector DataFvmLMCh::getVectorFieldValue(Index) {
        Prim p = getPrim();
        return p.v;
    }

}