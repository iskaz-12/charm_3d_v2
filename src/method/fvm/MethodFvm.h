/*
 *     ________  _____    ____  __  ___    _____ ____
 *    / ____/ / / /   |  / __ \/  |/  /   |__  // __ \  __    __
 *   / /   / /_/ / /| | / /_/ / /|_/ /     /_ </ / / /_/ /___/ /_
 *  / /___/ __  / ___ |/ _, _/ /  / /    ___/ / /_/ /_  __/_  __/
 *  \____/_/ /_/_/  |_/_/ |_/_/  /_/____/____/_____/ /_/   /_/
 *
 */


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

        ArrayReal           ru;
        ArrayReal           rv;
        ArrayReal           rw;
        ArrayReal           re;
        Array<ArrayReal>    rc;
        ArrayReal           matId;

        ArrayReal           ruInt;
        ArrayReal           rvInt;
        ArrayReal           rwInt;
        ArrayReal           reInt;
        Array<ArrayReal>    rcInt;

        Array<ArrayVector>  gradRC;
        ArrayVector         gradP;
        ArrayVector         gradU;
        ArrayVector         gradV;
        ArrayVector         gradW;

        FluxFvm *flux;


        struct {
            Real currentDt;
            Real elapsedTime;
        } info;

        void zeroIntegrals();

        Prim getPrim(Index) override;
        void setCons(Index iCell, const Prim &prim) override;

        Prim reconstruct(Index iCell, Point pt);
        void calcGrad();

        Index getScalarFieldsCount() override;
        String getScalarFieldName(Index) override;
        Real getScalarFieldValue(Index, Index) override;
        Index getVectorFieldsCount() override;
        String getVectorFieldName(Index) override;
        Vector getVectorFieldValue(Index, Index) override;
        void exchangeFields();
        void exchangeGrads();

        void printInfo();

    };

}

#endif //CHARM_3D_V2_METHODFVM_H
