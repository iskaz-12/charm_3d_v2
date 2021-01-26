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
            EOS_R_E_TO_P_CZ_T,  // (r,e)    =>  (p, cz, T)

            EOS_LOW_MACH_R_TO_T_E
        } EosFlag;

        Int     id;
        String  name;
        virtual void eos(Prim &p, EosFlag flag) = 0;
        static void eosSwitch(Prim &p, EosFlag flag);
        static void calcT(Prim &p);
    };

    typedef std::shared_ptr<Material> MaterialPtr;

    class MaterialIdeal : public Material {
        void eos(Prim &p, Material::EosFlag flag) override;

    };


    class MaterialMix : public Material {
        void eos(Prim &p, Material::EosFlag flag) override;

    };

}


#endif //CHARM_3D_V2_MATERIAL_H
