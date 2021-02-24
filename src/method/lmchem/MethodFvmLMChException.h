/*
 *     ________  _____    ____  __  ___    _____ ____
 *    / ____/ / / /   |  / __ \/  |/  /   |__  // __ \  __    __
 *   / /   / /_/ / /| | / /_/ / /|_/ /     /_ </ / / /_/ /___/ /_
 *  / /___/ __  / ___ |/ _, _/ /  / /    ___/ / /_/ /_  __/_  __/
 *  \____/_/ /_/_/  |_/_/ |_/_/  /_/____/____/_____/ /_/   /_/
 *
 */


//
// Created by zhrv on 15.12.2020.
//

#ifndef CHARM_3D_V2_METHODFVMLMCHEXCEPTION_H
#define CHARM_3D_V2_METHODFVMLMCHEXCEPTION_H

#include <MethodException.h>

namespace charm {

    class MethodFvmLMChException : public MethodException {
    public:
        explicit MethodFvmLMChException(String msg) : MethodException(msg) {}
    };

}


#endif //CHARM_3D_V2_METHODFVMLMCHEXCEPTION_H
