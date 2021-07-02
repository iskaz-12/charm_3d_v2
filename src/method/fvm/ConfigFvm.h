/*
 *     ________  _____    ____  __  ___    _____ ____
 *    / ____/ / / /   |  / __ \/  |/  /   |__  // __ \  __    __
 *   / /   / /_/ / /| | / /_/ / /|_/ /     /_ </ / / /_/ /___/ /_
 *  / /___/ __  / ___ |/ _, _/ /  / /    ___/ / /_/ /_  __/_  __/
 *  \____/_/ /_/_/  |_/_/ |_/_/  /_/____/____/_____/ /_/   /_/
 *
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
