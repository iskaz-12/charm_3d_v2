/**
 * Created by zhrv on 10.12.2020.
 * @author R.V.Zhalnin, zhalnin@gmail.com
 */

#ifndef CHARM_3D_V2_VTKWRITER_H
#define CHARM_3D_V2_VTKWRITER_H

#include "global.h"
#include "Method.h"

namespace charm {

    class VtkWriter {
    public:
        VtkWriter(Method *method);

        void write(Index step);

    protected:
        Method *method;
    };

}


#endif //CHARM_3D_V2_VTKWRITER_H
