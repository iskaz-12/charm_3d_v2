//
// Created by zhrv on 20.11.2020.
//

#include "FluxHllc.h"
#include "Config.h"

namespace charm {

    //  UPDATE ON 02.07.2023 - что за условные обозначения???
    //  Пока не разбираю данный класс...
#define F_HLLC_U(UK, FK, SK, SS, PK, RK, VK) (((SS)*((SK)*(UK)-(FK)) + (SK)*( (PK)+(RK)*((SK)-(VK))*((SS)-(VK)) )) / ((SK)-(SS)))
#define F_HLLC_V(UK, FK, SK, SS, PK, RK, VK) (((SS)*((SK)*(UK)-(FK))) / ((SK)-(SS)))
#define F_HLLC_E(UK, FK, SK, SS, PK, RK, VK) (((SS)*((SK)*(UK)-(FK)) + (SK)*( (PK)+(RK)*((SK)-(VK))*((SS)-(VK)) )*(SS)) / ((SK)-(SS)))


    void FluxHllc::x_1(const Prim &prim1, const Prim &prim2, Cons &flux) {
        Index         cCount = Config::getCompCount();
        Real          sl, sr, p_star, s_star, p_pvrs, ql, qr, tmp;

        p_pvrs = 0.5*(prim1.p+prim2.p)-0.5*(prim2.v.x-prim1.v.x)*0.25*(prim1.r+prim2.r)*(prim1.cz+prim2.cz);
        p_star = (p_pvrs > 0.) ? p_pvrs : 0.;

        ql = (p_star <= prim1.p) ? 1 : sqrt(1.+(prim1.gam+1.)*(p_star/prim1.p-1.)/(2.*prim1.gam));
        qr = (p_star <= prim2.p) ? 1 : sqrt(1.+(prim2.gam+1.)*(p_star/prim2.p-1.)/(2.*prim2.gam));

        sl = prim1.v.x-prim1.cz*ql;
        sr = prim2.v.x+prim2.cz*qr;

        if (sl>sr) {
            tmp = sl;
            sl = sr;
            sr = tmp;
        }

        s_star  = prim2.p-prim1.p;
        s_star += prim1.r*prim1.v.x*(sl-prim1.v.x);
        s_star -= prim2.r*prim2.v.x*(sr-prim2.v.x);
        s_star /= (prim1.r*(sl-prim1.v.x)-prim2.r*(sr-prim2.v.x));

        if (s_star < sl) s_star = sl;
        if (s_star > sr) s_star = sr;


        if (!((sl <= s_star) && (s_star <= sr))) {
            throw Exception("HLLC: inequality SL <= S* <= SR is FALSE.");
        }

        if (sl >= 0.) {
            flux.ru = prim1.r*prim1.v.x*prim1.v.x+prim1.p;
            flux.rv = prim1.r*prim1.v.y*prim1.v.x;
            flux.rw = prim1.r*prim1.v.z*prim1.v.x;
            flux.re = (prim1.r*prim1.eTot+prim1.p)*prim1.v.x;
            for (Index i = 0; i < cCount; i++) {
                flux.rc[i] = prim1.r*prim1.v.x*prim1.c[i];
            }
        }
        else if (sr <= 0.) {
            flux.ru = prim2.r*prim2.v.x*prim2.v.x+prim2.p;
            flux.rv = prim2.r*prim2.v.y*prim2.v.x;
            flux.rw = prim2.r*prim2.v.z*prim2.v.x;
            flux.re = (prim2.r*prim2.eTot+prim2.p)*prim2.v.x;
            for (Index i = 0; i < cCount; i++) {
                flux.rc[i] = prim2.r*prim2.v.x*prim2.c[i];
            }
        }
        else {
            if (s_star >= 0) {
                flux.ru = F_HLLC_U( /*  UK, FK, SK, SS, PK, RK, VK */
                        prim1.r * prim1.v.x,
                        prim1.r * prim1.v.x * prim1.v.x + prim1.p,
                        sl, s_star, prim1.p, prim1.r, prim1.v.x
                );
                flux.rv = F_HLLC_V( /*  UK, FK, SK, SS, PK, RK, VK */
                        prim1.r * prim1.v.y,
                        prim1.r * prim1.v.x * prim1.v.y,
                        sl, s_star, prim1.p, prim1.r, prim1.v.x
                );
                flux.rw = F_HLLC_V( /*  UK, FK, SK, SS, PK, RK, VK */
                        prim1.r * prim1.v.z,
                        prim1.r * prim1.v.x * prim1.v.z,
                        sl, s_star, prim1.p, prim1.r, prim1.v.x
                );
                flux.re = F_HLLC_E( /*  UK, FK, SK, SS, PK, RK, VK */
                        prim1.r * prim1.eTot,
                        (prim1.r * prim1.eTot + prim1.p)*prim1.v.x,
                        sl, s_star, prim1.p, prim1.r, prim1.v.x
                );
                for (Index i = 0; i < cCount; i++) {
                    flux.rc[i] = F_HLLC_V( /*  UK, FK, SK, SS, PK, RK, VK */
                            prim1.r * prim1.c[i],
                            prim1.r * prim1.c[i] * prim1.v.x,
                            sl, s_star, prim1.p, prim1.r, prim1.v.x
                    );
                }
            }
            else {
                flux.ru = F_HLLC_U( /*  UK, FK, SK, SS, PK, RK, VK */
                        prim2.r * prim2.v.x,
                        prim2.r * prim2.v.x * prim2.v.x + prim2.p,
                        sr, s_star, prim2.p, prim2.r, prim2.v.x
                );
                flux.rv = F_HLLC_V( /*  UK, FK, SK, SS, PK, RK, VK */
                        prim2.r * prim2.v.y,
                        prim2.r * prim2.v.x * prim2.v.y,
                        sr, s_star, prim2.p, prim2.r, prim2.v.x
                );
                flux.rw = F_HLLC_V( /*  UK, FK, SK, SS, PK, RK, VK */
                        prim2.r * prim2.v.z,
                        prim2.r * prim2.v.x * prim2.v.z,
                        sr, s_star, prim2.p, prim2.r, prim2.v.x
                );
                flux.re = F_HLLC_E( /*  UK, FK, SK, SS, PK, RK, VK */
                        prim2.r * prim2.eTot,
                        (prim2.r * prim2.eTot + prim2.p)*prim2.v.x,
                        sr, s_star, prim2.p, prim2.r, prim2.v.x
                );
                for (Index i = 0; i < cCount; i++) {
                    flux.rc[i] = F_HLLC_V( /*  UK, FK, SK, SS, PK, RK, VK */
                            prim2.r * prim2.c[i],
                            prim2.r * prim2.c[i] * prim2.v.x,
                            sr, s_star, prim2.p, prim2.r, prim2.v.x
                    );
                }
            }
        }
    }

    void FluxHllc::calc(const Prim &prim1, const Prim &prim2, Cons &flux, const Vector &n) {
        Index cCount = Config::getCompCount();
        Real ri;
        Vector nt[3], vv[2], vn[2];
        Real r_[2], u_[2], v_[2], w_[2], p_[2];
        Real _qu, _qv, _qw;

        Prim prim_1(cCount), prim_2(cCount);

        r_[0] = prim1.r;
        u_[0] = prim1.v.x;
        v_[0] = prim1.v.y;
        w_[0] = prim1.v.z;
        p_[0] = prim1.p;

        r_[1] = prim2.r;
        u_[1] = prim2.v.x;
        v_[1] = prim2.v.y;
        w_[1] = prim2.v.z;
        p_[1] = prim2.p;


        nt[0][0] = n[0];
        nt[0][1] = n[1];
        nt[0][2] = n[2];


        ri = sqrt(n[0]*n[0]+n[1]*n[1]);
        if (ri > EPS) {
            nt[1][0] = -n[1]/ri;
            nt[1][1] =  n[0]/ri;
            nt[1][2] =  0.;
        }
        else {
            ri = sqrt(n[1]*n[1]+n[2]*n[2]);
            if (ri > EPS) {
                nt[1][0] =  0.;
                nt[1][1] = -n[2]/ri;
                nt[1][2] =  n[1]/ri;
            }
            else {
                ri = sqrt(n[0]*n[0]+n[2]*n[2]);
                if (ri > EPS) {
                    nt[1][0] = -n[2]/ri;
                    nt[1][1] =  0.;
                    nt[1][2] =  n[0]/ri;
                }
                else {
                    if (1./sqrt(n[1]*n[1]+n[2]*n[2]) <= EPS) {
                        throw Exception("Wrong normal!");
                    }
                }
            }
        }
        nt[2] = vectorProd(nt[0], nt[1]);
        for (Index i = 0; i < 2; i++) {
            vv[i][0] = u_[i];
            vv[i][1] = v_[i];
            vv[i][2] = w_[i];
        }
        for (Index j = 0; j < 3; j++) {
            vn[0][j] = scalarProd(vv[0], nt[j]);
            vn[1][j] = scalarProd(vv[1], nt[j]);
        }
        prim_1 = prim1;
        prim_1.v = vn[0];
        prim_2 = prim2;
        prim_2.v = vn[1];

        Cons _flx(Config::getCompCount());

        x_1( prim_1, prim_2, _flx );
        flux = _flx;
        flux.ru = _flx.ru*nt[0][0] + _flx.rv*nt[1][0] + _flx.rw*nt[2][0];
        flux.rv = _flx.ru*nt[0][1] + _flx.rv*nt[1][1] + _flx.rw*nt[2][1];
        flux.rw = _flx.ru*nt[0][2] + _flx.rv*nt[1][2] + _flx.rw*nt[2][2];
    }

}