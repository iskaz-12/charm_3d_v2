/*
 *     ________  _____    ____  __  ___    _____ ____
 *    / ____/ / / /   |  / __ \/  |/  /   |__  // __ \  __    __
 *   / /   / /_/ / /| | / /_/ / /|_/ /     /_ </ / / /_/ /___/ /_
 *  / /___/ __  / ___ |/ _, _/ /  / /    ___/ / /_/ /_  __/_  __/
 *  \____/_/ /_/_/  |_/_/ |_/_/  /_/____/____/_____/ /_/   /_/
 *
 */


#include "BoundaryCondition.h"
#include "Config.h"

#include <utility>

namespace charm {

    const std::map<String, BoundaryCondition::Type> BoundaryCondition::TypeNames ={
            {"BOUND_INLET",         BoundaryCondition::INLET},
            {"BOUND_OUTLET",        BoundaryCondition::OUTLET},
            {"BOUND_WALL_SLIP",     BoundaryCondition::WALL_SLIP},
            {"BOUND_WALL_NO_SLIP",  BoundaryCondition::WALL_NO_SLIP},
            {"BOUND_MASS_FLOW",     BoundaryCondition::MASS_FLOW},
            {"BOUND_SYMMETRY",      BoundaryCondition::SYMMETRY},
            {"BOUND_FREE_STREAM",   BoundaryCondition::FREE_STREAM},
            {"BOUND_PRESSURE",      BoundaryCondition::PRESSURE},
            {"BOUND_PRESSURE_SIN",  BoundaryCondition::PRESSURE_SIN}
    };




    BoundaryCondition::Type BoundaryCondition::getTypeByName(const String &name) {
        auto it = TypeNames.find(name);
        if (it != TypeNames.end()) {
            return it->second;
        }
        return BoundaryCondition::UNKNOWN;
    }

    BoundaryCondition::BoundaryCondition(String _name, BoundaryCondition::Type _type):
        name(std::move(_name)),
        type(_type) {}


     BoundaryConditionSymmetry::BoundaryConditionSymmetry(String _name):
            BoundaryCondition(std::move(_name), BoundaryCondition::SYMMETRY) {}


    void BoundaryConditionSymmetry::calc(Prim &parIn, Prim &parOut, Vector &n) {
        Vector v = parIn.v;

        parOut = parIn;

        Real svn = scalarProd( v, n );
        Vector vv = n;
        vv *= svn;
        v -= vv;
        v -= vv;
        parOut.v = v;
    }


    BoundaryConditionWallSlip::BoundaryConditionWallSlip(String _name):
            BoundaryCondition(std::move(_name), BoundaryCondition::WALL_SLIP) {}


    void BoundaryConditionWallSlip::calc(Prim &parIn, Prim &parOut, Vector &n) {
        Vector v = parIn.v;

        parOut = parIn;

        Real svn = scalarProd( v, n );
        Vector vv = n;
        vv *= svn;
        v -= vv;
        v -= vv;
        parOut.v = v;
    }




    BoundaryConditionOutlet::BoundaryConditionOutlet(String _name):
            BoundaryCondition(std::move(_name), BoundaryCondition::OUTLET) {}


    void BoundaryConditionOutlet::calc(Prim &parIn, Prim &parOut, Vector &n) {
        parOut = parIn;
    }




    BoundaryConditionInlet::BoundaryConditionInlet(String _name, Vector _v, Real _t, Real _p, ArrayReal _c, Index _matId):
        BoundaryCondition(std::move(_name), BoundaryCondition::INLET),
        v(_v),
        t(_t),
        p(_p),
        c(_c),
        matId(_matId) {}


    void BoundaryConditionInlet::calc(Prim &parIn, Prim &parOut, Vector &n) {
        parOut = parIn;
        parOut.v = v;
        parOut.t = t;
        parOut.p = p;
        parOut.matId = matId;
        parOut.c.assign(c.begin(), c.end());
        parOut.eos(Material::EOS_T_P_TO_R_CZ_E);
        parOut.eTot = parOut.e + 0.5*parOut.v.sqr();
    }




    void BoundaryConditionPressure::calc(charm::Prim &parIn, charm::Prim &parOut, charm::Vector &n) {
        throw NotImplementedException();
    }


    void BoundaryConditionPressureSin::calc(charm::Prim &parIn, charm::Prim &parOut, charm::Vector &n) {
        Config& conf = Config::get();
        parOut = parIn;
        parOut.v = v;
        parOut.t = t;
        parOut.p = p+amplitude*sin(frequency*(conf.time+start));
        parOut.matId = matId;
        parOut.c.assign(c.begin(), c.end());
        parOut.eos(Material::EOS_T_P_TO_R_CZ_E);
        parOut.eTot = parOut.e + 0.5*parOut.v.sqr();

    }

    BoundaryConditionPressureSin::BoundaryConditionPressureSin(String _name, Vector _v, Real _t, Real _p, ArrayReal _c, Index _matId, Real _offset,
                                                               Real _frequency, Real _amplitude, Real _start):
        BoundaryCondition(std::move(_name), PRESSURE_SIN),
        v(_v),
        t(_t),
        p(_p),
        c(std::move(_c)),
        matId(_matId),
        offset(_offset),
        frequency(_frequency),
        amplitude(_amplitude),
        start(_start) {}


    void BoundaryConditionFreeStream::calc(Prim &parIn, Prim &parOut, Vector &n) {
        throw NotImplementedException();
    }


    void BoundaryConditionMassFlow::calc(Prim &parIn, Prim &parOut, Vector &n) {
        throw NotImplementedException();
    }




    BoundaryConditionWallNoSlip::BoundaryConditionWallNoSlip(String _name, Real _t):
            BoundaryCondition(_name, BoundaryCondition::WALL_NO_SLIP),
            t(_t) {}


    void BoundaryConditionWallNoSlip::calc(Prim &parIn, Prim &parOut, Vector &n) {
        throw NotImplementedException();
    }


}