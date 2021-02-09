/**
 * Created by zhrv on 18.11.2020.
 * @author R.V.Zhalnin, zhalnin@gmail.com
 */

#ifndef CHARM_3D_V2_METHOD_H
#define CHARM_3D_V2_METHOD_H

#include "Config.h"

namespace charm {

    class VtkWriter;

    class Method {
    public:
        VtkWriter* vtkWriter;

    public:
        explicit Method(Config* conf) {}
        virtual void init() = 0;
        virtual void run() = 0;
        virtual void done() = 0;
        virtual Prim getPrim(Index) = 0;

        virtual Index getScalarFieldsCount() = 0;
        virtual String getScalarFieldName(Index) = 0;
        virtual Real getScalarFieldValue(Index, Index) = 0;
        virtual Index getVectorFieldsCount() = 0;
        virtual String getVectorFieldName(Index) = 0;
        virtual Vector getVectorFieldValue(Index, Index) = 0;
    };


}

#endif //CHARM_3D_V2_METHOD_H
