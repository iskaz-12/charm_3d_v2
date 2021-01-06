/*
 *     ________  _____    ____  __  ___    _____ ____
 *    / ____/ / / /   |  / __ \/  |/  /   |__  // __ \  __    __
 *   / /   / /_/ / /| | / /_/ / /|_/ /     /_ </ / / /_/ /___/ /_
 *  / /___/ __  / ___ |/ _, _/ /  / /    ___/ / /_/ /_  __/_  __/
 *  \____/_/ /_/_/  |_/_/ |_/_/  /_/____/____/_____/ /_/   /_/
 *
 */

#include "MethodFvmLMCh.h"


namespace charm {
    using Cons = DataFvmLMCh::Cons;

    void MethodFvmLMCh::caldDiffCoeff() {
        Index cC = Config::getCompCount();
        ArrayReal x(cC, 0.);
        for (Index iCell = 0; iCell < mesh->cCount; iCell++) {
            Prim prim = data[iCell].getPrim();
            Real pabs = prim.p/101325.0;
            Real s = 0.;
            for (Index i = 0; i < cC; i++) {
                Component *ci = Config::getComponent(i);
                x[i] = prim.c[i] / ci->m;
                s += x[i];
            }
            for (Index i = 0; i < cC; i++) {
                x[i] /= s;
            }
            for (Index i = 0; i < cC; i++) {
                if (fabs(prim.c[i] - 1.) <= EPS) {
                    data[iCell].d[i] = 0.;
                } else {
                    Component *ci = Config::getComponent(i);
                    Real s_xd = 0.;
                    for (Index j = 0; j < cC; j++) {
                        Component *cj = Config::getComponent(j);
                        Real td = prim.t / sqrt(ci->ek * cj->ek);
                        Real wd = 1.06036 / pow(td, 0.1561) + 0.1930 / exp(0.47635 * td) + 1.03587 / exp(1.52996 * td) +
                             1.76474 / exp(3.89411 * td);
                        Real dij = (2.628e-7) * sqrt(0.5 * (1.0 / (1000.0 * ci->m) + 1.0 / (1000.0 * cj->m))) *
                              sqrt(prim.t * prim.t * prim.t) /
                              (pabs * pow((0.5 * (ci->sig + cj->sig)), 2.0) * wd);
                        s_xd += i == j ? 0. : x[j] / dij;
                    }
                    data[iCell].d[i] = (1. - x[i]) / s_xd;
                }
            }
        }
    }

    void MethodFvmLMCh::calcDiff() {

    }

}
