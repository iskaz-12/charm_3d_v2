/**
 * Created by zhrv on 18.11.2020.
 * @author R.V.Zhalnin, zhalnin@gmail.com
 */

#ifndef CHARM_3D_V2_METHOD_H
#define CHARM_3D_V2_METHOD_H

#include "Config.h"
#include "Data.h"
#include "VtkWriter.h"
//#include "VtkWriter.h"

namespace charm {

    class VtkWriter;

    class Method {
    public:
        Method(Config *conf, Mesh *mesh);

        Config *conf;
        Mesh *mesh;
        VtkWriter *vtkWriter;

        Mesh* getMesh() { return mesh; }
        Config* getConfig() { return conf; }

        virtual void init() = 0;

        virtual void run() = 0;

        virtual void done() = 0;

        virtual Data *getData(Index iCell) = 0;

        void exchange();

        static Method *create(Config *conf, Mesh *mesh);
    };

}

#endif //CHARM_3D_V2_METHOD_H
