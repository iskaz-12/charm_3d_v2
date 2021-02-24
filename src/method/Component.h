/**
 * Created by zhrv on 18.11.2020.
 * @author R.V.Zhalnin, zhalnin@gmail.com
 */

#ifndef CHARM_3D_V2_COMPONENT_H
#define CHARM_3D_V2_COMPONENT_H

#include "global.h"

namespace charm {

    class Component {
    public:
        typedef enum {
            CP_CONST,
            CP_POLYNOM
        } CpType;

        typedef enum {
            KP_CONST,
            KP_SATHERLAND,
            KP_NV
        } KpType;

        typedef enum {
            ML_CONST,
            ML_SATHERLAND,
            ML_NV
        } MlType;

        Real calcCp(Real t);
        Real calcH(Real t);
        Real calcCpDt(Real t);

        Real calcKp(Real t) const;
        Real calcMl(Real t) const;

        String      name;
        Index       id;
        Real        m;
        Real        ml0; //<! динамическая вязкость вещества при температуре T0
        Real        kp0; //<! теплопроводность вещества при температуре T0
        Real        t0;  //<! константа для формулы Сазерленда
        Real        ts;  //<! константа для формулы Сазерленда
        Real        sig; //<! параметры Леннарда-Джонса
        Real        ek;  //<! параметры Леннарда-Джонса
        Real        h0;  //<! энтальпия образования вещества
        CpType      cpType;
        KpType      kpType;
        MlType      mlType;
        ArrayReal   cp;

    };


}



#endif //CHARM_3D_V2_COMPONENT_H
