/**
 * Created by zhrv on 18.11.2020.
 * @author R.V.Zhalnin, zhalnin@gmail.com
 */

#ifndef CHARM_3D_V2_CONFIG_H
#define CHARM_3D_V2_CONFIG_H

#include <cassert>
#include "global.h"
#include "yaml-cpp/yaml.h"
#include "Mesh.h"
#include "Material.h"
#include "Component.h"
#include "Region.h"
#include "Reaction.h"
#include "BoundaryCondition.h"

namespace charm {
    class Method;
    class Config;
    typedef std::shared_ptr<Config> ConfigPtr;
    typedef std::shared_ptr<Method> MethodPtr;

    class Config {
    public:

        Config(const String &fileName);
        virtual void read();

        static ConfigPtr get();

        /**
         *
         * @param fileName
         * @return
         */
        static ConfigPtr create(const String &fileName);

        /**
         *
         * @return
         */
        virtual MethodPtr createMethod();

        /**
         *
         * @param i
         * @return
         */
        inline static ComponentPtr getComponent(Index i) {
            assert(i < getCompCount() && "Wrong component index...");
            return config->components[i];
        }


        inline static MaterialPtr  getMaterial(Index i)  {
            assert(i < getMatCount() && "Wrong material index...");
            return config->materials[i];
        }


        inline static RegionPtr    getRegion(Index i)    {
            assert(i < getRegCount() && "Wrong region index...");
            return config->regions[i];
        }


        inline static Index getCompCount() {
            return config->components.size();
        }


        inline static Index getMatCount()  {
            return config->materials.size();
        }


        inline static Index getRegCount()  {
            return config->regions.size();
        }

        inline static Index getBndCount() {
            return config->boundaries.size();
        }

        inline static BoundaryConditionPtr getBnd(Index i) {
            assert(i < getBndCount() && "Wrong boundary condition index...");
            return config->boundaries[i];
        }

    protected:
        void readBoundaries(const YAML::Node &node);
        void readMaterials(const YAML::Node &node);
        void readComponents(const YAML::Node &node);
        void readRegions(const YAML::Node &node);
        void readMeshInfo(const YAML::Node &node);
        void readReactions(const YAML::Node &node); //@todo

        BoundaryConditionPtr    fetchBoundary(const YAML::Node &node);
        RegionPtr               fetchRegion(const YAML::Node &node);
        MaterialPtr             fetchMaterial(const YAML::Node &node);
        ComponentPtr            fetchComponent(const YAML::Node &node);
        ReactionPtr             fetchReaction(const YAML::Node &node);
    public:

        static ConfigPtr config;
        YAML::Node  confYaml;

        Mesh       *mesh;

        String      methodName;
        Real        maxErr;            ///< maximum allowed global interpolation error
        Int         refinePeriod;      ///< the number of time steps between mesh refinement
        Int         repartitionPeriod; ///< the number of time steps between repartitioning
        Int         writePeriod;       ///< the number of time steps between writing vtk files
        Int         restartPeriod;     ///< the number of time steps between writing restart files
        Int         logPeriod;         ///< the number of time steps between writing log
        Int         minLevel;          ///< the minimal level
        Int         maxLevel;          ///< the allowed level
        Real        cfl;               ///< the CFL
        Real        dt;                ///< time step delta
        Real        t;                 ///< the current time
        Real        time;              ///< the max time
        Int         timestep;

        struct {
            Mesh::FileType   fileType;
            String           fileName;
        } msh;

    protected:
        Array<BoundaryConditionPtr>   boundaries;
        Array<MaterialPtr>            materials;       ///< materials */
        Array<RegionPtr>              regions;       ///< regions */
        Array<ComponentPtr>           components;      ///< components */
        Array<ReactionPtr>            reactions; ///< reactions */
    };

    typedef std::shared_ptr<Config> ConfigPtr;

}

#endif //CHARM_3D_V2_CONFIG_H
