/**
 * Created by zhrv on 18.11.2020.
 * @author R.V.Zhalnin, zhalnin@gmail.com
 */

#ifndef CHARM_3D_V2_CONFIG_H
#define CHARM_3D_V2_CONFIG_H

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

        //  UPDATE ON 01.07.2023 - считывание условий задачи из yaml-файла
        virtual void read();

        static Config* get();

        /**
         *
         * @param fileName
         * @return
         */
         // UPDATE ON 01.07.2023 - создание файла конфигурации
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
            if (i >= getCompCount()) {
                throw Exception("Wrong component index...");
            }
            return config->components[i];
        }


        inline static Material*  getMaterial(Index i)  {
            if (i >= getMatCount()) {
                throw Exception("Wrong material index...");
            }
            return config->materials[i];
        }


        inline static Region*    getRegion(Index i)    {
            if (i >= getRegCount()) {
                throw Exception("Wrong region index...");
            }
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

    protected:

        //  UPDATE ON 02.07.2023 - считывание информации о границах, материалах, веществах, регионах, сетке и реакциях
        void readBoundaries(const YAML::Node &node);
        void readMaterials(const YAML::Node &node);
        void readComponents(const YAML::Node &node);
        void readRegions(const YAML::Node &node);
        void readMeshInfo(const YAML::Node &node);
        void readReactions(const YAML::Node &node); //@todo

        //  UPDATE ON 02.07.2023 - получение параметров границ, регионов, материалов, веществ и реакций из task.yaml
        BoundaryCondition*    fetchBoundary(const YAML::Node &node);
        Region*     fetchRegion(const YAML::Node &node);
        Material*   fetchMaterial(const YAML::Node &node);
        Component*  fetchComponent(const YAML::Node &node);
        Reaction*   fetchReaction(const YAML::Node &node);
    public:

        static Config* config;
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

        Array<BoundaryCondition*>   boundaries;
        Array<Material*>            materials;       ///< materials */
        Array<Region*>              regions;       ///< regions */
        Array<Component*>           components;      ///< components */
        Array<Reaction*>            reactions; ///< reactions */

//        union {
//            struct {
//
//            } euler;
//
//            struct {
//                Int                         use_visc;
//                Int                         use_diff;
//                Real                t_ref;
//                struct {
//                    charm_init_fn_t             init_cond_fn;
//                    charm_turb_model_fn_t       model_fn;
//                    charm_turb_models_t         model_type;
//                    union {
//                        struct {
//                            Real a1;
//                            Real sigma_k1;
//                            Real sigma_k2;
//                            Real sigma_w1;
//                            Real sigma_w2;
//                            Real beta_star;
//                            Real beta_1;
//                            Real beta_2;
//                            Real kappa;
//                        } sst;
//
//                        struct {
//                            Real sigma;
//                            Real kappa;
//                            Real cb1;
//                            Real cb2;
//                            Real cw1;
//                            Real cw2;
//                            Real cw3;
//                            Real cv1;
//                            Real ct1;
//                            Real ct2;
//                            Real ct3;
//                            Real ct4;
//                        } sa;
//                    } param;
//                } turb;
//
//            } ns;
//        } model;
////    Real              visc_m;
////    Real              visc_l;


//        charm_mesh_info_t          *msh;
//        charm_timestep_single_fn_t  timestep_single_fn;
//        charm_get_timestep_fn_t     get_dt_fn;
//        charm_flux_fn_t             flux_fn;
//        charm_limiter_fn_t          lim_fn;
//        charm_amr_init_fn_t         amr_init_fn;
//        charm_amr_fn_t              amr_fn;
//        charm_init_fn_t             model_init_cond_fn;

    };

}

#endif //CHARM_3D_V2_CONFIG_H
