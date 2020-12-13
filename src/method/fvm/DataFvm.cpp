//
// Created by zhrv on 19.11.2020.
//

#include "DataFvm.h"
#include "Config.h"
#include "MethodException.h"

namespace charm {
    Prim& Prim::operator = (const Prim &prim) {
        r           = prim.r;
        v           = prim.v;
        e           = prim.e;
        eTot        = prim.eTot;
        this->p     = prim.p;
        t           = prim.t;
        cz          = prim.cz;
        gam         = prim.gam;
        cp          = prim.cp;
        cv          = prim.cv;
        m           = prim.m;
        matId       = prim.matId;
        c.assign(prim.c.begin(), prim.c.end());

        return *this;
    }

    void Prim::eos(Material::EosFlag flag) {
        Config::getMaterial(matId)->eos(*this, flag);
    }


    Cons& Cons::operator = (const Cons& cons) {
        ru      = cons.ru;
        rv      = cons.rv;
        rw      = cons.rw;
        re      = cons.re;
        matId   = cons.matId;
        rc.assign(cons.rc.begin(), cons.rc.end());
        return *this;
    }

    Cons& Cons::operator = (const Real& a) {
        ru      = a;
        rv      = a;
        rw      = a;
        re      = a;
        matId   = a;
        rc.assign(rc.size(), a);
        return *this;
    }

    void Cons::normalize() {
        if (::fabs(ru) < EPS) ru = 0.;
        if (::fabs(rv) < EPS) rv = 0.;
        if (::fabs(rw) < EPS) rw = 0.;
        if (::fabs(re) < EPS) re = 0.;
        for (auto &c: rc) {
            if (::fabs(c) < EPS) c = 0.;
        }
    }

    Cons &Cons::operator+=(const Cons &cons) {
        ru += cons.ru;
        rv += cons.rv;
        rw += cons.rw;
        re += cons.re;
        for (Index i = 0; i < rc.size(); i++) {
            rc[i] += cons.rc[i];
        }
        return *this;
    }

    Cons &Cons::operator-=(const Cons &cons) {
        ru -= cons.ru;
        rv -= cons.rv;
        rw -= cons.rw;
        re -= cons.re;
        for (Index i = 0; i < rc.size(); i++) {
            rc[i] -= cons.rc[i];
        }
        return *this;
    }

    Cons &Cons::operator*=(const Real &a) {
        ru *= a;
        rv *= a;
        rw *= a;
        re *= a;
        for (Index i = 0; i < rc.size(); i++) {
            rc[i] *= a;
        }
        return *this;
    }



    /**
     *
     * @return
     */
    Prim DataFvm::getPrim() {
        Index cCount = Config::getCompCount();
        Prim p(cCount);
        p.matId = c.matId;
        p.r      = 0.;
        for (Index i = 0; i < cCount; i++) {
            p.r += c.rc[i];
        }
        p.v.x      = c.ru/p.r;
        p.v.y      = c.rv/p.r;
        p.v.z      = c.rw/p.r;
        p.eTot     = c.re/p.r;
        p.e        = p.eTot-0.5*p.v2();
        if (p.e < EPS) {
            throw MethodException("p.e < EPS");
        }

        p.c.resize(cCount);
        for (Index i = 0; i < cCount; i++) {
            p.c[i] = c.rc[i] / p.r;
            if (p.c[i] < 0.) p.c[i] = 0.; // @todo
            if (p.c[i] > 1.) p.c[i] = 1.;
        }
        p.eos(Material::EOS_R_E_TO_P_CZ_T);
        p.shrink();
        return p;
    }


    /**
     *
     * @param p
     */
    void DataFvm::getPrim(Prim &p) {
        p = getPrim();
    }


    /**
     *
     * @return
     */
    Cons& DataFvm::getCons() {
        return c;
    }

    /**
     *
     * @param p
     */
    void DataFvm::setCons(const Prim &p) {
        Index count = p.c.size();
        size_t i;
        c.matId = p.matId;
        c.ru = p.r * p.v.x;
        c.rv = p.r * p.v.y;
        c.rw = p.r * p.v.z;
        c.re = p.r * (p.e + 0.5 * p.v2());
        c.rc.resize(count);
        for (Index i = 0; i < count; i++) {
            c.rc[i] = p.r * p.c[i];
        }
        c.shrink();
    }

    void DataFvm::getBuffer(Byte *buf) {
        Index shift = 0;
        *(Real*)(&buf[shift]) = c.ru; shift += sizeof(Real);
        *(Real*)(&buf[shift]) = c.rv; shift += sizeof(Real);
        *(Real*)(&buf[shift]) = c.rw; shift += sizeof(Real);
        *(Real*)(&buf[shift]) = c.re; shift += sizeof(Real);
        for (int i = 0; i < c.rc.size(); i++) {
            *(Real*)(&buf[shift]) = c.rc[i]; shift += sizeof(Real);
        }
        *(Index*)(&buf[shift]) = c.matId; shift += sizeof(Index);
    }

    void DataFvm::setBuffer(Byte *buf) {
        Index shift = 0;
        c.ru = *(Real*)(&buf[shift]); shift += sizeof(Real);
        c.rv = *(Real*)(&buf[shift]); shift += sizeof(Real);
        c.rw = *(Real*)(&buf[shift]); shift += sizeof(Real);
        c.re = *(Real*)(&buf[shift]); shift += sizeof(Real);
        for (int i = 0; i < c.rc.size(); i++) {
            c.rc[i] = *(Real*)(&buf[shift]); shift += sizeof(Real);
        }
        c.matId = *(Index*)(&buf[shift]); shift += sizeof(Index);
    }

    Index DataFvm::getScalarFieldsCount() {
        return 5 + Config::getCompCount();
    }

    String DataFvm::getScalarFieldName(Index iFld) {
        if (iFld == 0) {
            return "Density";
        }
        else if (iFld == 1) {
            return "Pressure";
        }
        else if (iFld == 2) {
            return "Energy";
        }
        else if (iFld == 3) {
            return "Tot_Energy";
        }
        else if (iFld == 4) {
            return "Temperature";
        }
        else if (iFld == 5) {
            return "Mach_num";
        }
        for (Index i = 0; i < Config::getCompCount(); i++) {
            if (iFld == 5+i) {
                return "C_"+Config::getComponent(i)->name;
            }
        }

        throw Exception("Wrong field number");
    }

    Real DataFvm::getScalarFieldValue(Index iFld) {
        Prim p = getPrim();
        if (iFld == 0) {
            return p.r;
        }
        else if (iFld == 1) {
            return p.p;
        }
        else if (iFld == 2) {
            return p.e;
        }
        else if (iFld == 3) {
            return p.eTot;
        }
        else if (iFld == 4) {
            return p.t;
        }
        else if (iFld == 5) {
            return p.v.length()/p.cz;
        }
        for (Index i = 0; i < Config::getCompCount(); i++) {
            if (iFld == 5+i) {
                return p.c[i];
            }
        }

        throw Exception("Wrong field number");
    }

    Index DataFvm::getVectorFieldsCount() {
        return 1;
    }

    String DataFvm::getVectorFieldName(Index) {
        return "Velosity";
    }

    Vector DataFvm::getVectorFieldValue(Index) {
        Prim p = getPrim();
        return p.v;
    }

}
