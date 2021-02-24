//
// Created by zhrv on 20.11.2020.
//

#include "FluxFvmHllc.h"
#include "Config.h"

namespace charm {
#define F_HLLC_U(UK, FK, SK, SS, PK, RK, VK) (((SS)*((SK)*(UK)-(FK)) + (SK)*( (PK)+(RK)*((SK)-(VK))*((SS)-(VK)) )) / ((SK)-(SS)))
#define F_HLLC_V(UK, FK, SK, SS, PK, RK, VK) (((SS)*((SK)*(UK)-(FK))) / ((SK)-(SS)))
#define F_HLLC_E(UK, FK, SK, SS, PK, RK, VK) (((SS)*((SK)*(UK)-(FK)) + (SK)*( (PK)+(RK)*((SK)-(VK))*((SS)-(VK)) )*(SS)) / ((SK)-(SS)))


    void FluxFvmHllc::x_1(const Prim &prim1, const Prim &prim2, Real &ru, Real &rv, Real &rw, Real &re, ArrayReal &rc) {
        Index         compCount = Config::getCompCount();
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
            ru = prim1.r*prim1.v.x*prim1.v.x+prim1.p;
            rv = prim1.r*prim1.v.y*prim1.v.x;
            rw = prim1.r*prim1.v.z*prim1.v.x;
            re = (prim1.r*prim1.eTot+prim1.p)*prim1.v.x;
            for (Index j = 0; j < compCount; j++) {
                rc[j] = prim1.r*prim1.c[j]*prim1.v.x;
            }
        }
        else if (sr <= 0.) {
            ru = prim2.r*prim2.v.x*prim2.v.x+prim2.p;
            rv = prim2.r*prim2.v.y*prim2.v.x;
            rw = prim2.r*prim2.v.z*prim2.v.x;
            re = (prim2.r*prim2.eTot+prim2.p)*prim2.v.x;
            for (Index j = 0; j < compCount; j++) {
                rc[j] = prim2.r*prim2.c[j]*prim2.v.x;
            }
        }
        else {
            if (s_star >= 0) {
                ru = F_HLLC_U( /*  UK, FK, SK, SS, PK, RK, VK */
                        prim1.r * prim1.v.x,
                        prim1.r * prim1.v.x * prim1.v.x + prim1.p,
                        sl, s_star, prim1.p, prim1.r, prim1.v.x
                );
                rv = F_HLLC_V( /*  UK, FK, SK, SS, PK, RK, VK */
                        prim1.r * prim1.v.y,
                        prim1.r * prim1.v.x * prim1.v.y,
                        sl, s_star, prim1.p, prim1.r, prim1.v.x
                );
                rw = F_HLLC_V( /*  UK, FK, SK, SS, PK, RK, VK */
                        prim1.r * prim1.v.z,
                        prim1.r * prim1.v.x * prim1.v.z,
                        sl, s_star, prim1.p, prim1.r, prim1.v.x
                );
                re = F_HLLC_E( /*  UK, FK, SK, SS, PK, RK, VK */
                        prim1.r * prim1.eTot,
                        (prim1.r * prim1.eTot + prim1.p)*prim1.v.x,
                        sl, s_star, prim1.p, prim1.r, prim1.v.x
                );
                for (Index i = 0; i < compCount; i++) {
                    rc[i] = F_HLLC_V( /*  UK, FK, SK, SS, PK, RK, VK */
                            prim1.r * prim1.c[i],
                            prim1.r * prim1.c[i] * prim1.v.x,
                            sl, s_star, prim1.p, prim1.r, prim1.v.x
                    );
                }
            }
            else {
                ru = F_HLLC_U( /*  UK, FK, SK, SS, PK, RK, VK */
                        prim2.r * prim2.v.x,
                        prim2.r * prim2.v.x * prim2.v.x + prim2.p,
                        sr, s_star, prim2.p, prim2.r, prim2.v.x
                );
                rv = F_HLLC_V( /*  UK, FK, SK, SS, PK, RK, VK */
                        prim2.r * prim2.v.y,
                        prim2.r * prim2.v.x * prim2.v.y,
                        sr, s_star, prim2.p, prim2.r, prim2.v.x
                );
                rw = F_HLLC_V( /*  UK, FK, SK, SS, PK, RK, VK */
                        prim2.r * prim2.v.z,
                        prim2.r * prim2.v.x * prim2.v.z,
                        sr, s_star, prim2.p, prim2.r, prim2.v.x
                );
                re = F_HLLC_E( /*  UK, FK, SK, SS, PK, RK, VK */
                        prim2.r * prim2.eTot,
                        (prim2.r * prim2.eTot + prim2.p)*prim2.v.x,
                        sr, s_star, prim2.p, prim2.r, prim2.v.x
                );
                for (Index i = 0; i < compCount; i++) {
                    rc[i] = F_HLLC_V( /*  UK, FK, SK, SS, PK, RK, VK */
                            prim2.r * prim2.c[i],
                            prim2.r * prim2.c[i] * prim2.v.x,
                            sl, s_star, prim2.p, prim2.r, prim2.v.x
                    );
                }
            }
        }
    }

    void FluxFvmHllc::calc(const Prim &prim1, const Prim &prim2, Real &ru, Real &rv, Real &rw, Real &re, ArrayReal &rc, const Vector &n) {
        Index compCount = Config::getCompCount();
        Real ri;
        Vector nt[3], vv[2], vn[2];
        Real u_[2], v_[2], w_[2];

        Prim prim_1(compCount), prim_2(compCount);

        u_[0] = prim1.v.x;
        v_[0] = prim1.v.y;
        w_[0] = prim1.v.z;

        u_[1] = prim2.v.x;
        v_[1] = prim2.v.y;
        w_[1] = prim2.v.z;


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

        ArrayReal rc_flx(compCount);
        Real ru_flx;
        Real rv_flx;
        Real rw_flx;
        Real re_flx;

        x_1( prim_1, prim_2, ru_flx, rv_flx, rw_flx, re_flx, rc_flx );
        ru = ru_flx*nt[0][0] + rv_flx*nt[1][0] + rw_flx*nt[2][0];
        rv = ru_flx*nt[0][1] + rv_flx*nt[1][1] + rw_flx*nt[2][1];
        rw = ru_flx*nt[0][2] + rv_flx*nt[1][2] + rw_flx*nt[2][2];
        re = re_flx;
        for (Index i = 0; i < compCount; i++) {
            rc[i] = rc_flx[i];
        }
    }

}