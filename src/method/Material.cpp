/**
 * Created by zhrv on 18.11.2020.
 * @author R.V.Zhalnin, zhalnin@gmail.com
 */

#include "Material.h"
#include "MethodException.h"
#include "Config.h"
#include "EosException.h"
#include <cmath>

namespace charm {

    Real gR = 8.31446261815324; // Дж/(моль К)


    void Material::calcT(Prim &p) {
        Config& conf = Config::get();
        Real tt = 1.0;        // начальное приближение для температуры
        Real h = p.h;        // энтальпия
        Real cp, cp_dt, tt1;

        // Newton_Method
        for (Index j = 0; j < 100; j++) {
            cp = 0.0;
            cp_dt = 0.0;
            for (Index i = 0; i < Config::getCompCount(); i++) {
                Component* comp = Config::getComponent(i);
                cp    += p.c[i] * comp->calcH(tt);
                cp_dt += p.c[i] * comp->calcCp(tt);
            }

            tt1 = tt - (cp - h) / cp_dt;
            if (std::pow(tt1 - tt, 2) < EPS) {
                tt = tt1;
                break;
            }
            tt = tt1;
        }
        p.t = tt;
    }


    void MaterialIdeal::eos(Prim &p, Material::EosFlag flag) {
        Component* comp = Config::getComponent(0);
        Real t;

        if (comp->cpType == Component::CP_POLYNOM &&
            (flag == EOS_R_P_TO_E_T ||
             flag == EOS_R_E_TO_P_CZ_T ||
             flag == EOS_LOW_MACH_R_TO_T_E)) {
            calcT(p);
            t = p.t;
        } else {
            t = p.t;
        }

        p.cp = comp->calcCp(t);
        p.m = comp->m;
        p.cv = p.cp - gR / p.m;
        p.gam = p.cp / p.cv;
        eosSwitch(p, flag);
    }


    void MaterialMix::eos(Prim &p, Material::EosFlag flag) {
        Index compCount = Config::getCompCount();
        Component* comp = Config::getComponent(0);
        Real t;

        if (comp->cpType == Component::CP_POLYNOM &&
            (flag == EOS_R_P_TO_E_T ||
             flag == EOS_R_E_TO_P_CZ_T ||
             flag == EOS_LOW_MACH_R_TO_T_E)) {
            calcT(p);
            t = p.t;
        } else {
            t = p.t;
        }

        p.cp  = 0.;
        Real M_  = 0.;
        for (Index i = 0; i < compCount; i++) {
            comp = Config::getComponent(i);
            M_ += p.c[i]/comp->m;
            p.cp += p.c[i]*comp->calcCp(t);
        }
        p.m   = 1./M_;
        p.cv = p.cp - gR / p.m;
        p.gam = p.cp / p.cv;
        eosSwitch(p, flag);
    }

    void Material::eosSwitch(Prim &p, Material::EosFlag flag) {
        switch (flag) {
            case EOS_R_E_TO_P_CZ:        // p=p(r,e)
                if (p.r < EPS) {
                    throw EosException("EOS_R_E_TO_P_CZ: p.r < EPS");
                }
                p.p = p.r * p.e * (p.gam - 1);
                p.cz = ::sqrt(p.gam * p.p / p.r);
                break;

            case EOS_R_P_TO_E_T:        // (e,t)=e(r,p)
                if (p.p < EPS) {
                    throw EosException("EOS_R_P_TO_E_T: p.p < EPS");
                }
                if (p.r < EPS) {
                    throw EosException("EOS_R_P_TO_E_T: p.r < EPS");
                }
                p.e = p.p / (p.r * (p.gam - 1));
                p.t = p.e / p.cv;
                break;

            case EOS_T_P_TO_R_CZ:        // r=r(T,p)
                if (p.p < EPS) {
                    throw EosException("EOS_T_P_TO_R_CZ: p.p < EPS");
                }
                p.r = p.p * p.m / (p.t * gR);
                p.cz = ::sqrt(p.gam * p.p / p.r);
                break;

            case EOS_T_P_TO_R_CZ_E: // (T,p) => (r, cz, e)
                if (p.p < EPS) {
                    throw EosException("EOS_T_P_TO_R_CZ_E: p.p < EPS");
                }
                p.r = p.p * p.m / (p.t * gR);
                p.cz = sqrt(p.gam * p.p / p.r);
                p.e = p.p / (p.r * (p.gam - 1));
                break;

            case EOS_R_E_TO_P_CZ_T: // (r,e) => (p, cz, T)
                if (p.r < EPS) {
                    throw EosException("EOS_R_E_TO_P_CZ_T: p.r < EPS");
                }
                p.p = p.r * p.e * (p.gam - 1);
                p.cz = sqrt(p.gam * p.p / p.r);
                p.t = p.e / p.cv;
                break;

            // Low Mach flow
            case EOS_LOW_MACH_T_P_TO_R_CZ_E:
                if (p.p0 < EPS) {
                    throw EosException("EOS_T_P_TO_R_CZ_E: p.p0 < EPS");
                }
                p.r = p.p0 * p.m / (p.t * gR);
                p.cz = sqrt(p.gam * p.p0 / p.r);
                p.e = p.p0 / (p.r * (p.gam - 1));
                break;

            case EOS_LOW_MACH_R_TO_T_E:
                p.e = p.p0 / (p.r * (p.gam - 1));
                p.t = p.e / p.cv;
                p.cz = ::sqrt(p.gam * p.p0 / p.r);
                break;

            default:
                throw MethodException("WRONG EOS FLAG!");
        }


    }
}