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

    class Config {
    public:

        Config(const String &fileName);
        virtual void read();

        static Config* get();

        /**
         *
         * @param fileName
         * @return
         */
        static Config* create(const String &fileName);

        /**
         *
         * @return
         */
        virtual Method* createMethod();

        /**
         *
         * @param i
         * @return
         */
        inline static Component* getComponent(Index i) {
            assert(i < getCompCount() && "Wrong component index...");
            return config->components[i];
        }


        inline static Material*  getMaterial(Index i)  {
            assert(i < getMatCount() && "Wrong material index...");
            return config->materials[i];
        }


        inline static Region*    getRegion(Index i)    {
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

        inline static BoundaryCondition* getBnd(Index i) {
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

        BoundaryCondition*  fetchBoundary(const YAML::Node &node);
        Region*             fetchRegion(const YAML::Node &node);
        Material*           fetchMaterial(const YAML::Node &node);
        Component*          fetchComponent(const YAML::Node &node);
        Reaction*           fetchReaction(const YAML::Node &node);
    public:

        static Config* config;
        YAML::Node  confYaml;

        Mesh* mesh;

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
        Array<BoundaryCondition*>   boundaries;
        Array<Material*>            materials;       ///< materials */
        Array<Region*>              regions;       ///< regions */
        Array<Component*>           components;      ///< components */
        Array<Reaction*>            reactions; ///< reactions */
    };


}

#endif //CHARM_3D_V2_CONFIG_H
