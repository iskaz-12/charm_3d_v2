//
// Created by zhrv on 15.12.2020.
//

#include "Prim.h"
#include "Config.h"

namespace charm {

    void Prim::eos(Material::EosFlag flag) {
        Config::getMaterial(matId)->eos(*this, flag);
    }


}