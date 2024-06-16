/**
 * Created by zhrv on 18.11.2020.
 * @author R.V.Zhalnin, zhalnin@gmail.com
 */

#ifndef CHARM_3D_V2_MATERIAL_H
#define CHARM_3D_V2_MATERIAL_H

#include "global.h"


namespace charm {

    class Prim;

    class Material {
    public:
        typedef enum {
            EOS_R_E_TO_P_CZ,    // (r,e)    =>  (p,cz)
            EOS_R_P_TO_E_T,     // (r,p)    =>  (e,t)
            EOS_T_P_TO_R_CZ,    // (T,p)    =>  (r,cz)
            EOS_T_P_TO_R_CZ_E,  // (T,p)    =>  (r, cz, e)
            EOS_R_E_TO_P_CZ_T   // (r,e)    =>  (p, cz, T)
        } EosFlag;

        Int     id;
        String  name;
        virtual void eos(Prim &p, EosFlag flag) = 0;    //  UPDATE ON 19.06.2023 - определение недостающих параметров с использованием уравнения состояния
        static void eosSwitch(Prim &p, EosFlag flag);   //  UPDATE ON 19.06.2023 - переключение флага, определяющего вид уравнений состояния
        static void calcT(Prim &p); //  UPDATE ON 19.06.2023 - вычисление температуры
    };


    class MaterialIdeal : public Material {
        void eos(Prim &p, Material::EosFlag flag);

    };


    class MaterialMix : public Material {
        void eos(Prim &p, Material::EosFlag flag);

    };

}

/*

 typedef struct charm_mat
{
    int                 id;
    char                name[64];
    charm_eos_fn_t      eos_fn;
} charm_mat_t;






 */

#endif //CHARM_3D_V2_MATERIAL_H
