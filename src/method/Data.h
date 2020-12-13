/**
 * Created by zhrv on 10.12.2020.
 * @author R.V.Zhalnin, zhalnin@gmail.com
 */

#ifndef CHARM_3D_V2_DATA_H
#define CHARM_3D_V2_DATA_H


namespace charm {

    struct Data {
    public:
        virtual void getBuffer(Byte *) = 0;
        virtual void setBuffer(Byte *) = 0;
        virtual Index size() = 0;

        virtual Index getScalarFieldsCount() = 0;
        virtual String getScalarFieldName(Index) = 0;
        virtual Real getScalarFieldValue(Index) = 0;
        virtual Index getVectorFieldsCount() = 0;
        virtual String getVectorFieldName(Index) = 0;
        virtual Vector getVectorFieldValue(Index) = 0;
    };

}

#endif //CHARM_3D_V2_DATA_H
