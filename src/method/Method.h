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
        Ptr<Config>    conf;
        Ptr<Mesh>      mesh;
        Ptr<VtkWriter> vtkWriter;

    public:
        explicit Method(Ptr<Config> conf);
        Ptr<Mesh> getMesh() const;
        Ptr<Config> getConfig() const;
        virtual void init() = 0;
        virtual void run() = 0;
        virtual void done() = 0;
        virtual Ptr<Data> getData(Index iCell) = 0;
        void exchange();
        void exchange(ArrayReal &field);
        void exchange(ArrayVector &field);
        void exchange(Array<ArrayVector> &field);
    };


}

#endif //CHARM_3D_V2_METHOD_H
