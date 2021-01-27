/**
 * Created by zhrv on 18.11.2020.
 * @author R.V.Zhalnin, zhalnin@gmail.com
 */

#ifndef CHARM_3D_V2_METHODFVM_H
#define CHARM_3D_V2_METHODFVM_H

#include <FluxFvm.h>
#include <method/Data.h>
#include "Method.h"

namespace charm {

    class MethodFvm : public Method {
    public:
        explicit MethodFvm(Ptr<Config> conf): Method(conf) {}

        void init() override;
        void run() override;
        void done() override;
        Ptr<Data> getData(Index iCell) override;

        Real calcDt();

        void save();

        Array<DataFvm>  data;
        Array<DataFvm::Cons>     integrals;

        FluxFvm *flux;

        void seroIntegrals();
    };

}

#endif //CHARM_3D_V2_METHODFVM_H
