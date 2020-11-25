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


    void Material::calcT(Prim &p)
    {
        Config *conf = Config::get();
        Real tt = 1.0;		// начальное приближение для температуры
        Real e  = p.e;		// энергия
        //int c_count = charm_get_comp_count(p4est);
        Real rm, cp, cp_dt, ft, ft_dt, tt1;

        Real m_ = 0.0;		//  формулы   M_ = 1 / M   где   M = 1 / SUM( Ci / Mi )


        for (Index i = 0; i < conf->components.size(); i++)	{
            m_ += p.c[i] / conf->components[i]->m;
        }
        rm = gR * m_;


        for (Index j=0; j<100; j++ )
        {
            cp = 0.0;
            cp_dt = 0.0;
            for (Index i = 0; i < conf->components.size(); i++)	{
                Component *comp = conf->components[i];
                cp  += p.c[i] * comp->calcCp(tt);
                cp_dt += p.c[i] * comp->calcCpDt(tt);
            }


            ft    = ( cp - rm ) * tt - e;
            ft_dt = ( cp - rm ) + cp_dt * tt;

            tt1 = tt - ft / ft_dt;
            if ( pow( tt1 - tt, 2 ) < EPS ) {
                tt = tt1;
                break;
            }
            tt = tt1;
        }
        p.t = tt;
    } // Newton_Method


    void MaterialIdeal::eos(Prim &p, Material::EosFlag flag) {
        Config  *conf = Config::get();
        Component *comp = Config::getComponent(0);
//        Real t;
//
//        if (comp->cpType == Component::CP_POLYNOM && (flag == EOS_R_P_TO_E_T || flag == EOS_R_E_TO_P_CZ_T)) {
//            calcT(p);
//            t = p.t;
//        }
//        else {
//            t = p.t;
//        }

        p.cp  = comp->cp[0];//comp->calcCp(t);
        p.m   = comp->m;
        p.cv  = p.cp-gR/p.m;
        p.gam = p.cp/p.cv;
        eosSwitch(p, flag);
    }

    void MaterialMix::eos(Prim &p, Material::EosFlag flag) {
        //@todo
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

            default:
                throw MethodException("WRONG EOS FLAG!");
        }


    }
}