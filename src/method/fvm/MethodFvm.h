/**
 * Created by zhrv on 18.11.2020.
 * @author R.V.Zhalnin, zhalnin@gmail.com
 */

#ifndef CHARM_3D_V2_METHODFVM_H
#define CHARM_3D_V2_METHODFVM_H

#include <FluxFvm.h>
#include "Method.h"

namespace charm {

    class MethodFvm : public Method {
    public:
        explicit MethodFvm(Config* _conf): Method(_conf) {}

        void init() override;
        void run() override;
        void done() override;

        Real calcDt();

        void save();

        ArrayReal           ro;
        ArrayReal           ru;
        ArrayReal           rv;
        ArrayReal           rw;
        ArrayReal           re;
        ArrayReal           matId;

        ArrayReal           roInt;
        ArrayReal           ruInt;
        ArrayReal           rvInt;
        ArrayReal           rwInt;
        ArrayReal           reInt;

        FluxFvm *flux;

        void seroIntegrals();

        Prim getPrim(Index) override;
        void setCons(Index i, Prim &p);

        Index getScalarFieldsCount() override;
        String getScalarFieldName(Index) override;
        Real getScalarFieldValue(Index) override;
        Index getVectorFieldsCount() override;
        String getVectorFieldName(Index) override;
        Vector getVectorFieldValue(Index) override;
    };

}

#endif //CHARM_3D_V2_METHODFVM_H
