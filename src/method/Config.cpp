/**
 * Created by zhrv on 18.11.2020.
 * @author R.V.Zhalnin, zhalnin@gmail.com
 */

#include <iostream>
#include <MeshReader.h>
#include <ConfigFvmLMCh.h>
#include <lmchem/ConfigFvmLMCh.h>
#include "Config.h"
#include "ConfigFvm.h"
#include "Material.h"
#include "BoundaryCondition.h"
#include "ConfigException.h"


namespace charm {

    Ptr<Config> Config::config = Ptr<Config>(nullptr);


    /**
     *
     * @param fileName
     */
    Config::Config(const String &fileName) {
        confYaml = YAML::LoadFile(fileName);
    }


    /**
     *
     */
    void Config::read() {
        methodName = confYaml["method"].as<String>();
        YAML::Node control = confYaml["control"];
        maxErr                = control["MAX_ERROR"].as<Real>();
        refinePeriod          = control["REFINE_PERIOD"].as<Int>();
        repartitionPeriod     = control["REPARTITION_PERIOD"].as<Int>();
        minLevel              = control["MIN_LEVEL"].as<Int>();
        maxLevel              = control["MAX_LEVEL"].as<Int>();
        writePeriod           = control["FILE_OUTPUT_STEP"].as<Int>();
        restartPeriod         = control["RESTART_OUTPUT_STEP"].as<Int>();
        logPeriod             = control["LOG_OUTPUT_STEP"].as<Int>();
        dt                    = control["TAU"].as<Real>();
        cfl                   = control["CFL"].as<Real>();
        time                  = control["TMAX"].as<Real>();
        timestep              = control["STEP_START"].as<Int>();
        t                     = control["TIME_START"].as<Real>();

        readComponents(confYaml["components"]);
        readBoundaries(confYaml["boundaries"]);
        readMaterials(confYaml["materials"]);
        readRegions(confYaml["regions"]);
        readMeshInfo(confYaml["mesh"]);
    }


    /**
     *
     * @param fileName
     * @return
     */
    Ptr<Config> Config::create(const String &fileName = "task.xml") {
        YAML::Node node = YAML::LoadFile("task.yaml");
        String str = node["method"].as<std::string>();

        if (str != "CHARM_3D") {
            throw ConfigException("Wrong method name. Must be 'CHARM_3D'");
        }
        String model = node["control"]["MODEL"]["name"].as<String>();
        if (model == "EULER_FVM") {
            config = newPtr<ConfigFvm>(fileName);
        }
        if (model == "LOW_MACH_CHEM_FVM") {
            config = newPtr<ConfigFvmLMCh>(fileName);
        }
        else {
            throw ConfigException("Wrong model name.");
        }
        config->read();
        return config;
    }


    /**
     *
     * @param node
     */
    void Config::readBoundaries(const YAML::Node &node) {
        boundaries.clear();
        for (auto it : node) {
            auto b = fetchBoundary(it);
            boundaries.push_back(b);
        }
    }


    /**
     *
     * @param node
     */
    void Config::readMaterials(const YAML::Node &node) {
        materials.clear();
        for (auto it : node) {
            auto m = fetchMaterial(it);
            materials.push_back(m);
        }
    }


    /**
     *
     * @param node
     */
    void Config::readComponents(const YAML::Node &node) {
        components.clear();
        for (auto it : node) {
            auto c = fetchComponent(it);
            components.push_back(c);
        }
    }


    /**
     *
     * @param node
     */
    void Config::readRegions(const YAML::Node &node) {
        regions.clear();
        for (auto it : node) {
            auto reg = fetchRegion(it);
            regions.push_back(reg);
        }
    }


    /**
     *
     * @param node
     */
    void Config::readReactions(const YAML::Node &node) {
        reactions.clear();
        for (auto it : node) {
            auto r = fetchReaction(it);
            reactions.push_back(r);
        }
    }


    /**
     *
     * @param node
     */
    void Config::readMeshInfo(const YAML::Node &node) {
        msh.fileType = Mesh::getfileTypeByStr(node["files_type"].as<std::string>());
        msh.fileName = node["name"].as<std::string>();
    }



    /**
     *
     * @param node
     * @return
     */
    Ptr<Region> Config::fetchRegion(const YAML::Node &node)
    {
        YAML::Node n1;
        Index id;
        Index idx;
        Real c;

        String str;
        auto reg = newPtr<Region>();

        reg->id     = node["id"].as<Int>();
        reg->name   = node["name"].as<std::string>();
        reg->matId = node["material_id"].as<Index>();

        n1 = node["parameters"];
        reg->v[0]   = n1["V"]["x"].as<Real>();
        reg->v[1]   = n1["V"]["y"].as<Real>();
        reg->v[2]   = n1["V"]["z"].as<Real>();
        reg->t      = n1["T"].as<Real>();
        reg->p      = n1["P"].as<Real>();

        reg->grav[0]   = n1["G"]["x"].as<Real>();
        reg->grav[1]   = n1["G"]["y"].as<Real>();
        reg->grav[2]   = n1["G"]["z"].as<Real>();

        reg->c.resize(components.size());
        n1 = node["components"];
        for (auto it : n1) {
            id = it["id"].as<int>();
            c  = it["concentration"].as<Real>();
            bool found = false;

            for (Index idx = 0; idx < components.size(); idx++) {
                if (components[idx]->id == id) {
                    reg->c[idx] = c;
                    found = true;
                }
            }
            if (!found) {
                throw ConfigException("Unknown component id "+ std::to_string(id) +" for region '" + reg->name + "' in file 'task.yaml'");
            }
        }

        c = 0;
        for (Index i = 0; i < components.size(); i++) {
            c += reg->c[i];
        }
        if (fabs(c)-1. > EPS) {
            throw ConfigException("Sum of concentrations for region '" + reg->name + "' is not equal to 1.");
        }
        return reg;
    }

    /**
     *
     * @param node
     * @return
     */
    Ptr<BoundaryCondition> Config::fetchBoundary(const YAML::Node &node) {
        Ptr<BoundaryCondition> bnd;
        YAML::Node n2, n3;
        BoundaryCondition::Type bndType = BoundaryCondition::getTypeByName(node["type"].as<std::string>().c_str());
        String bndName = node["name"].as<String>();

        if (bndType == BoundaryCondition::INLET) {
            n2 = node["parameters"];
            Vector v = {
                    n2["V"]["x"].as<Real>(),
                    n2["V"]["y"].as<Real>(),
                    n2["V"]["z"].as<Real>()
            };
            Real t = n2["T"].as<Real>();
            Real p = n2["P"].as<Real>();
            auto matId = node["material_id"].as<Index>();

            ArrayReal c;
            c.resize(components.size());
            n3 = node["components"];
            for (auto it : n3) {
                auto id = it["id"].as<Index>();
                Real _c = it["concentration"].as<Real>();
                bool found = false;

                for (Index idx = 0; idx < components.size(); idx++) {
                    if (components[idx]->id == id) {
                        c[idx] = _c;
                        found = true;
                    }
                }
                if (!found) {
                    throw ConfigException("Unknown component id " + std::to_string(id) + " for boundary condition '" + bndName);
                }
            }

            Real s = 0;
            for (Index i = 0; i < components.size(); i++) {
                s += c[i];
            }
            if (fabs(s - 1.) > EPS) {
                throw ConfigException("Sum of concentrations for boundary '" + bndName + "' is not equal to 1");
            }
            bnd = newPtr<BoundaryConditionInlet>(bndName, v, t, p, c, matId);
        }
        else if (bndType == BoundaryCondition::OUTLET) {
            bnd = newPtr<BoundaryConditionOutlet>(bndName);
        }
        else if (bndType == BoundaryCondition::SYMMETRY) {
            bnd = newPtr<BoundaryConditionSymmetry>(bndName);
        }
        else if (bndType == BoundaryCondition::WALL_SLIP) {
            bnd = newPtr<BoundaryConditionWallSlip>(bndName);
        }
        else if (bndType == BoundaryCondition::WALL_NO_SLIP) {
            n2 = node["parameters"];
            Real t = n2["T"].as<Real>();
            bnd = newPtr<BoundaryConditionWallNoSlip>(bndName, t);
        }
//        else if (bndType == BoundaryCondition::MASS_FLOW) {
//            bnd->bnd_fn = charm_bnd_cond_fn_mass_flow;
//            n2 = node["parameters"];
//            bnd->params = CHARM_ALLOC(charm_real_t, 7 + c_count);
//            bnd->params[0] = n2["M"].as<charm_real_t>();
//            bnd->params[1] = n2["P"].as<charm_real_t>();
//            bnd->params[2] = n2["T"].as<charm_real_t>();
//            bnd->params[3] = n2["CosX"].as<charm_real_t>();
//            bnd->params[4] = n2["CosY"].as<charm_real_t>();
//            bnd->params[5] = n2["CosZ"].as<charm_real_t>();
//            bnd->params[6] = n2["P"].as<charm_real_t>();
//            n3 = n2["components"];
//            i = 7;
//            memset(&(bnd->params[i]), 0, sizeof(charm_real_t) * c_count);
//            for (auto it : n3) {
//                id = it["id"].as<int>();
//                c = it["concentration"].as<charm_real_t>();
//                if (charm_comp_index_find_by_id(ctx, id, &idx)) {
//                    bnd->params[i + idx] = c;
//                } else {
//                    CHARM_LERRORF("Unknown component id %d for boundary '%s' in file 'task.yaml'\n", id, bnd->name);
//                    charm_abort(nullptr, 1);
//                }
//            }
//
//            c = 0;
//            for (idx = 0; idx < c_count; idx++) {
//                c += bnd->params[i + idx];
//            }
//            if (fabs(c) - 1. > CHARM_EPS) {
//                CHARM_LERRORF("Sum of concentrations for boundary '%s' is not equal to 1 in file 'task.yaml'\n",
//                              bnd->name);
//                charm_abort(nullptr, 1);
//            }
//        }
//        else if (bndType == BoundaryCondition::FREE_STREAM) {
//            bnd->bnd_fn = charm_bnd_cond_fn_free_stream;
//            n2 = node["parameters"];
//            bnd->params = CHARM_ALLOC(charm_real_t, 6 + c_count);
//            bnd->params[0] = n2["M"].as<charm_real_t>();
//            bnd->params[1] = n2["P"].as<charm_real_t>();
//            bnd->params[2] = n2["T"].as<charm_real_t>();
//            bnd->params[3] = n2["CosX"].as<charm_real_t>();
//            bnd->params[4] = n2["CosY"].as<charm_real_t>();
//            bnd->params[5] = n2["CosZ"].as<charm_real_t>();
//            n3 = n2["components"];
//            i = 6;
//            memset(&(bnd->params[i]), 0, sizeof(charm_real_t) * c_count);
//            for (auto it : n3) {
//                id = it["id"].as<int>();
//                c = it["concentration"].as<charm_real_t>();
//                if (charm_comp_index_find_by_id(ctx, id, &idx)) {
//                    bnd->params[i + idx] = c;
//                } else {
//                    CHARM_LERRORF("Unknown component id %d for boundary '%s' in file 'task.yaml'\n", id, bnd->name);
//                    charm_abort(nullptr, 1);
//                }
//            }
//
//            c = 0;
//            for (idx = 0; idx < c_count; idx++) {
//                c += bnd->params[i + idx];
//            }
//            if (fabs(c) - 1. > CHARM_EPS) {
//                CHARM_LERRORF("Sum of concentrations for boundary '%s' is not equal to 1 in file 'task.yaml'\n",
//                              bnd->name);
//                charm_abort(nullptr, 1);
//            }
//
//            for (auto it : n3) {
//                if (i > 7+c_count) { //@todo
//                    CHARM_LERRORF("BOUND_FREE_STREAM: Too many components specified. Must be %d\n", c_count);
//                }
//                bnd->params[i++] = it.as<charm_real_t>();
//            }
//            if (i < 7+c_count) {
//                CHARM_LERRORF("BOUND_FREE_STREAM: Too few components specified. Must be %d\n", c_count);
//            }
//
//        }
//        else if (bndType == BoundaryCondition::PRESSURE) {
//            bnd->bnd_fn = charm_bnd_cond_fn_pressure;
//            n2 = node["parameters"];
//            bnd->params = CHARM_ALLOC(charm_real_t, 2 + c_count);
//            bnd->params[0] = n2["P"].as<charm_real_t>();
//            bnd->params[1] = n2["T"].as<charm_real_t>();
//            n3 = n2["components"];
//            i = 2;
//            memset(&(bnd->params[i]), 0, sizeof(charm_real_t) * c_count);
//            for (auto it : n3) {
//                id = it["id"].as<int>();
//                c = it["concentration"].as<charm_real_t>();
//                if (charm_comp_index_find_by_id(ctx, id, &idx)) {
//                    bnd->params[i + idx] = c;
//                } else {
//                    CHARM_LERRORF("Unknown component id %d for boundary '%s' in file 'task.yaml'\n", id, bnd->name);
//                    charm_abort(nullptr, 1);
//                }
//            }
//
//            c = 0;
//            for (idx = 0; idx < c_count; idx++) {
//                c += bnd->params[i + idx];
//            }
//            if (fabs(c) - 1. > CHARM_EPS) {
//                CHARM_LERRORF("Sum of concentrations for boundary '%s' is not equal to 1 in file 'task.yaml'\n",
//                              bnd->name);
//                charm_abort(nullptr, 1);
//            }

//            for (auto it : n3) {
//                if (i > 2+c_count) { //@todo
//                    CHARM_LERRORF("BOUND_MASS_FLOW: Too many components specified. Must be %d\n", c_count);
//                }
//                bnd->params[i++] = it.as<charm_real_t>();
//            }
//            if (i < 2+c_count) {
//                CHARM_LERRORF("BOUND_MASS_FLOW: Too few components specified. Must be %d\n", c_count);
//            }
//
//        }
        else {
            throw ConfigException("Unknown boundary type %s\n" + bnd->name);
        }
        return bnd;
    }

    /**
     *
     * @param node
     * @return
     */
    Ptr<Material> Config::fetchMaterial(const YAML::Node &node) {
        std::string str;
        Ptr<Material> mat;
        str = node["eof_type"].as<std::string>();
        if (str == "IDEAL") {
            mat = newPtr<MaterialIdeal>();
            if (components.size() > 1) {
                Log::print("WARNING! There is more than one componen. First component's parameters is used for EOS.\n");
            }
        }
        else if (str == "MIX") {
            mat = newPtr<MaterialMix>();
        }
        else {
            throw ConfigException("Unknown EOS type '" + str + "'. Use: LF, GODUNOV.");
        }
        mat->id = node["id"].as<int>();
        mat->name = node["name"].as<std::string>();

        return mat;
    }

    /**
     *
     * @param node
     * @return
     */
    Ptr<Component> Config::fetchComponent(const YAML::Node &node) {
        String str;
        auto comp = newPtr<Component>();
        comp->id = node["id"].as<Int>();
        comp->name = node["name"].as<String>();


        str = node["cp_type"].as<String>();
        if (str == "CONST") {
            comp->cpType = Component::CP_CONST;
        }
        else if (str == "POLYNOM") {
            comp->cpType = Component::CP_POLYNOM;
        }
        else {
            std::cerr << ("Unknown Cp type '%s'. Use: CONST, POLYNOM.", str.c_str());
            exit(1);
        }

        str = node["kp_type"].as<String>();
        if (str == "CONST") {
            comp->kpType = Component::KP_CONST;
        }
        else if (str == "SATHERLAND") {
            comp->kpType = Component::KP_SATHERLAND;
        }
        else {
            std::cerr << ("Unknown KP type '%s'. Use: CONST, SATHERLAND.", str.c_str());
            exit(1);
        }
        str = node["ml_type"].as<std::string>();
        if (str == "CONST") {
            comp->mlType = Component::ML_CONST;
        }
        else if (str == "SATHERLAND") {
            comp->mlType = Component::ML_SATHERLAND;
        }
        else {
            throw ConfigException("Unknown ML type '" + str + "'. Use: CONST, SATHERLAND.");
        }

        comp->m     = node["M"].as<Real>();
        comp->ml0   = node["ML0"].as<Real>();
        comp->kp0   = node["KP0"].as<Real>();
        comp->t0    = node["T0"].as<Real>();
        comp->ts    = node["TS"].as<Real>();
        comp->sig   = node["Sig"].as<Real>();
        comp->ek    = node["ek"].as<Real>();
        comp->h0    = node["h0"].as<Real>();

        comp->cp.clear();
        YAML::Node cp = node["Cp"];
        for (auto it : cp) {
            comp->cp.push_back(it.as<Real>());
        }

        return comp;
    }

    /**
     *
     * @param node
     * @return
     */
    Ptr<Reaction> Config::fetchReaction(const YAML::Node &node) {
        return nullptr;
    }

    Ptr<Method> Config::createMethod() {
        mesh = MeshReader::create(config)->read();
        return nullptr;
    }

    Ptr<Config> Config::get() {
        if (config == nullptr) {
            config = Config::create();
        }
        return config;
    }

}