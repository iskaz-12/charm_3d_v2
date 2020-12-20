/**
 * Created by zhrv on 18.11.2020.
 * @author R.V.Zhalnin, zhalnin@gmail.com
 */

#ifndef CHARM_3D_V2_METHOD_H
#define CHARM_3D_V2_METHOD_H

#include "Config.h"
#include "Data.h"

namespace charm {

    class VtkWriter;

    class Method {
    public:
        Config *conf;
        Mesh *mesh;
        VtkWriter *vtkWriter;

    public:
        Method(Config *conf);
        Mesh* getMesh() const;
        Config* getConfig() const;
        virtual void init() = 0;
        virtual void run() = 0;
        virtual void done() = 0;
        virtual Data *getData(Index iCell) = 0;
        void exchange();
    };

}

#endif //CHARM_3D_V2_METHOD_H
