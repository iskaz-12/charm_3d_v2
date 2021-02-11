/**
 * Created by zhrv on 18.11.2020.
 * @author R.V.Zhalnin, zhalnin@gmail.com
 */

#ifndef CHARM_3D_V2_CONFIGFVM_H
#define CHARM_3D_V2_CONFIGFVM_H

#include "Config.h"

namespace charm {

    class ConfigFvm : public Config {
    public:
        ConfigFvm(const String &fileName) : Config(fileName) {}

        void read() override;

        Method* createMethod() override;

        bool useReconstruct;
    };

}
#endif //CHARM_3D_V2_CONFIGFVM_H
