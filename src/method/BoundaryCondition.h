/**
 * Created by zhrv on 18.11.2020.
 * @author R.V.Zhalnin, zhalnin@gmail.com
 */

#ifndef CHARM_3D_V2_BOUNDARYCONDITION_H
#define CHARM_3D_V2_BOUNDARYCONDITION_H

#include <map>
#include "global.h"
#include "Point.h"
#include "Data.h"

//  UPDATE ON 13.07.2023 - include DataDgHeat.h
#include "DataDgHeat.h"

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

        //  UPDATE ON 24.06.2023 - создание словаря вида: название граничного условия в yaml-файле - название переменной
        static const std::map<String, Type> TypeNames;

        String  name;
        Type    type;

        BoundaryCondition(String name, Type type);

        //  UPDATE ON 01.07.2023 - получение типа граничного условия по имени
        static BoundaryCondition::Type getTypeByName(const String &name);

        virtual void calc(Prim &parIn, Prim &parOut, Vector &n) = 0;

        //  UPDATE ON 13.07.2023 - добавляю дополнительную функцию calcHeat для обработки границ в случае задач теплопроводности
        virtual void calcHeat(PrimHeat &parIn, PrimHeat &parOut, Vector &n) = 0;
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
        virtual void calc(Prim &parIn, Prim &parOut, Vector &n);

        //  UPDATE ON 13.07.2023 - добавляю дополнительную функцию calcHeat для обработки границ в случае задач теплопроводности
        virtual void calcHeat(PrimHeat &parIn, PrimHeat &parOut, Vector &n);
    };


    class BoundaryConditionOutlet : public BoundaryCondition {
    public:
        explicit BoundaryConditionOutlet(String name);
        virtual void calc(Prim &parIn, Prim &parOut, Vector &n);

        //  UPDATE ON 13.07.2023 - добавляю дополнительную функцию calcHeat для обработки границ в случае задач теплопроводности
        virtual void calcHeat(PrimHeat &parIn, PrimHeat &parOut, Vector &n);
    };


    class BoundaryConditionWallSlip : public BoundaryCondition {
    public:
        explicit BoundaryConditionWallSlip(String name);
        virtual void calc(Prim &parIn, Prim &parOut, Vector &n);
        
        //  UPDATE ON 13.07.2023 - добавляю дополнительную функцию calcHeat для обработки границ в случае задач теплопроводности
        virtual void calcHeat(PrimHeat &parIn, PrimHeat &parOut, Vector &n);
    };


    class BoundaryConditionWallNoSlip : public BoundaryCondition {
    public:
        Real t;
        BoundaryConditionWallNoSlip(String name, Real t);
        virtual void calc(Prim &parIn, Prim &parOut, Vector &n);

        //  UPDATE ON 13.07.2023 - добавляю дополнительную функцию calcHeat для обработки границ в случае задач теплопроводности
        virtual void calcHeat(PrimHeat &parIn, PrimHeat &parOut, Vector &n);
    };


    class BoundaryConditionMassFlow : public BoundaryCondition {
    public:
        virtual void calc(Prim &parIn, Prim &parOut, Vector &n);

        //  UPDATE ON 13.07.2023 - добавляю дополнительную функцию calcHeat для обработки границ в случае задач теплопроводности
        virtual void calcHeat(PrimHeat &parIn, PrimHeat &parOut, Vector &n);
    };


    class BoundaryConditionSymmetry : public BoundaryCondition {
    public:
        explicit BoundaryConditionSymmetry(String name);
        virtual void calc(Prim &parIn, Prim &parOut, Vector &n);

        //  UPDATE ON 13.07.2023 - добавляю дополнительную функцию calcHeat для обработки границ в случае задач теплопроводности
        virtual void calcHeat(PrimHeat &parIn, PrimHeat &parOut, Vector &n);
    };


    class BoundaryConditionFreeStream : public BoundaryCondition {
    public:
        virtual void calc(Prim &parIn, Prim &parOut, Vector &n);

        //  UPDATE ON 13.07.2023 - добавляю дополнительную функцию calcHeat для обработки границ в случае задач теплопроводности
        virtual void calcHeat(PrimHeat &parIn, PrimHeat &parOut, Vector &n);
    };


    class BoundaryConditionPressure : public BoundaryCondition {
    public:
        virtual void calc(Prim &parIn, Prim &parOut, Vector &n);

        //  UPDATE ON 13.07.2023 - добавляю дополнительную функцию calcHeat для обработки границ в случае задач теплопроводности
        virtual void calcHeat(PrimHeat &parIn, PrimHeat &parOut, Vector &n);
    };

}
#endif //CHARM_3D_V2_BOUNDARYCONDITION_H
