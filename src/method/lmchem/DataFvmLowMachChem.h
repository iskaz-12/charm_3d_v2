//
// Created by zhrv on 15.12.2020.
//

#ifndef CHARM_3D_V2_DATAFVMLOWMACHCHEM_H
#define CHARM_3D_V2_DATAFVMLOWMACHCHEM_H

#include <method/Data.h>

namespace charm {

    class DataFvmLowMachChem : public Data{
    public:
        Cons    c;

        explicit DataFvm(Index compCount): c(compCount) {}

        void getPrim(Prim &p);
        Prim getPrim();
        Cons& getCons();
        void setCons(const Prim &p);
        void getBuffer(Byte *) override;
        void setBuffer(Byte *) override;
        Index size() override { return c.size(); }

        Index getScalarFieldsCount() override;
        String getScalarFieldName(Index) override;
        Real getScalarFieldValue(Index) override;
        Index getVectorFieldsCount() override;
        String getVectorFieldName(Index) override;
        Vector getVectorFieldValue(Index) override;
    };

}

#endif //CHARM_3D_V2_DATAFVMLOWMACHCHEM_H
