/**
 * Created by zhrv on 18.11.2020.
 * @author R.V.Zhalnin, zhalnin@gmail.com
 */

#ifndef CHARM_3D_V2_METHOD_H
#define CHARM_3D_V2_METHOD_H

#include "Config.h"

namespace charm{

    class Method {
    public:
        Method(Config *conf, Mesh *mesh);
        Config *conf;
        Mesh   *mesh;

        virtual void init() = 0;
        virtual void run() = 0;
        virtual void done() = 0;

        static Method* create(Config *conf, Mesh *mesh);
    };

}

#endif //CHARM_3D_V2_METHOD_H
