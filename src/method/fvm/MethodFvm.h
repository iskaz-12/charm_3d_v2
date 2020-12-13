/**
 * Created by zhrv on 18.11.2020.
 * @author R.V.Zhalnin, zhalnin@gmail.com
 */

#ifndef CHARM_3D_V2_METHODFVM_H
#define CHARM_3D_V2_METHODFVM_H

#include <Flux.h>
#include <method/Data.h>
#include "Method.h"

namespace charm {

    class MethodFvm : public Method {
    public:
        explicit MethodFvm(Config *conf, Mesh *mesh): Method(conf, mesh) {}

        void init() override;
        void run() override;
        void done() override;
        Data* getData(Index iCell) override;

        Real calcDt();

        void save();
        void saveVtk();

        Array<DataFvm>  data;
        Array<Cons>     integrals;

        Flux *flux;

        void seroIntegrals();
    };

}

#endif //CHARM_3D_V2_METHODFVM_H
