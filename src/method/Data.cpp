//
// Created by zhrv on 19.11.2020.
//

#include "Data.h"
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

    void Cons::normalize() {
        if (::fabs(ru) < EPS) ru = 0.;
        if (::fabs(rv) < EPS) rv = 0.;
        if (::fabs(rw) < EPS) rw = 0.;
        if (::fabs(re) < EPS) re = 0.;
        for (auto &c: rc) {
            if (::fabs(c) < EPS) c = 0.;
        }
    }



    /**
     *
     * @return
     */
    Prim Data::getPrim() {
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
    void Data::getPrim(Prim &p) {
        p = getPrim();
    }


    /**
     *
     * @return
     */
    Cons& Data::getCons() {
        return c;
    }

    /**
     *
     * @param p
     */
    void Data::setCons(const Prim &p) {
        Index count = p.c.size();
        size_t i;   //  UPDATE ON 24.06.2023 - size_t - беззнаковый целочисленный тип данных (может хранить теоретически максимально возможное значение любого типа данных)
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
}
