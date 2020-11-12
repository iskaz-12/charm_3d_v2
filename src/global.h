//
// Created by zhrv on 12.11.2020.
//

#ifndef CHARM_3D_V2_GLOBAL_H
#define CHARM_3D_V2_GLOBAL_H

#include <cstdlib>
#include <vector>

typedef double      charm_real_t;
typedef int         charm_int_t;
typedef size_t      charm_size_t;
typedef int8_t      charm_byte_t;

const charm_real_t          CHARM_EPS               = 1.e-12;
const charm_int_t           CHARM_DIM               = 3;

const std::vector<charm_int_t> CHARM_DIM_RANGE = {0, 1, 2};


#endif //CHARM_3D_V2_GLOBAL_H
