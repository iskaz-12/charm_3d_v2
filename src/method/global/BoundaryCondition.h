/*
 *     ________  _____    ____  __  ___    _____ ____
 *    / ____/ / / /   |  / __ \/  |/  /   |__  // __ \  __    __
 *   / /   / /_/ / /| | / /_/ / /|_/ /     /_ </ / / /_/ /___/ /_
 *  / /___/ __  / ___ |/ _, _/ /  / /    ___/ / /_/ /_  __/_  __/
 *  \____/_/ /_/_/  |_/_/ |_/_/  /_/____/____/_____/ /_/   /_/
 *
 */


#ifndef CHARM_3D_V2_BOUNDARYCONDITION_H
#define CHARM_3D_V2_BOUNDARYCONDITION_H

#include <map>
#include <memory>
#include "global.h"
#include "Point.h"
#include "Prim.h"

namespace charm {

    class BoundaryCondition {
    public:
        typedef enum {
            INLET,
            OUTLET,
            WALL_SLIP,
            WALL_NO_SLIP,
            MASS_FLOW,
            SYMMETRY,
            FREE_STREAM,
            PRESSURE,
            UNKNOWN
        } Type;

        static const std::map<String, Type> TypeNames;

        String  name;
        Type    type;

        BoundaryCondition(String name, Type type);

        static BoundaryCondition::Type getTypeByName(const String &name);

        virtual void calc(Prim &parIn, Prim &parOut, Vector &n) = 0;
    };

    typedef BoundaryCondition BC;


    class BoundaryConditionInlet : public BoundaryCondition {
    public:
        Index       matId;
        Vector      v;
        Real        t;
        Real        p;
        ArrayReal   c;

        BoundaryConditionInlet(String name, Vector v, Real t, Real p, ArrayReal c, Index matId);
        void calc(Prim &parIn, Prim &parOut, Vector &n) override;
    };


    class BoundaryConditionOutlet : public BoundaryCondition {
    public:
        explicit BoundaryConditionOutlet(String name);
        void calc(Prim &parIn, Prim &parOut, Vector &n) override;
    };


    class BoundaryConditionWallSlip : public BoundaryCondition {
    public:
        explicit BoundaryConditionWallSlip(String name);
        void calc(Prim &parIn, Prim &parOut, Vector &n) override;
    };


    class BoundaryConditionWallNoSlip : public BoundaryCondition {
    public:
        Real t;
        BoundaryConditionWallNoSlip(String name, Real t);
        void calc(Prim &parIn, Prim &parOut, Vector &n) override;
    };


    class BoundaryConditionMassFlow : public BoundaryCondition {
    public:
        void calc(Prim &parIn, Prim &parOut, Vector &n) override;
    };


    class BoundaryConditionSymmetry : public BoundaryCondition {
    public:
        explicit BoundaryConditionSymmetry(String name);
        void calc(Prim &parIn, Prim &parOut, Vector &n) override;
    };


    class BoundaryConditionFreeStream : public BoundaryCondition {
    public:
        void calc(Prim &parIn, Prim &parOut, Vector &n) override;
    };


    class BoundaryConditionPressure : public BoundaryCondition {
    public:
        void calc(Prim &parIn, Prim &parOut, Vector &n) override;
    };

}
#endif //CHARM_3D_V2_BOUNDARYCONDITION_H
