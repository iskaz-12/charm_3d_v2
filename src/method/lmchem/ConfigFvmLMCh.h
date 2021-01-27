/*
 *     ________  _____    ____  __  ___    _____ ____
 *    / ____/ / / /   |  / __ \/  |/  /   |__  // __ \  __    __
 *   / /   / /_/ / /| | / /_/ / /|_/ /     /_ </ / / /_/ /___/ /_
 *  / /___/ __  / ___ |/ _, _/ /  / /    ___/ / /_/ /_  __/_  __/
 *  \____/_/ /_/_/  |_/_/ |_/_/  /_/____/____/_____/ /_/   /_/
 *
 */


//
// Created by zhrv on 17.12.2020.
//

#ifndef CHARM_3D_V2_CONFIGFVMLMCH_H
#define CHARM_3D_V2_CONFIGFVMLMCH_H

#include <Config.h>

namespace charm {
    class ConfigFvmLMCh : public Config  {
    public:
        explicit ConfigFvmLMCh(const String &fileName);

        void read() override;

        Ptr<Method> createMethod() override;

        Int     useVisc;
        Int     useDiff;
        Real    tRef;

        Real    pressEps;
        Index   pressMaxIter;
    };
}

#endif //CHARM_3D_V2_CONFIGFVMLMCH_H
