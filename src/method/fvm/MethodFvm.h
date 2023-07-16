/**
 * Created by zhrv on 18.11.2020.
 * @author R.V.Zhalnin, zhalnin@gmail.com
 */

#ifndef CHARM_3D_V2_METHODFVM_H
#define CHARM_3D_V2_METHODFVM_H

#include <Flux.h>
#include "Method.h"

namespace charm {

    class MethodFvm : public Method {
    public:
        explicit MethodFvm(Config *conf, Mesh *mesh): Method(conf, mesh) {}

        //  UPDATE ON 02.07.2023 - инициализация метода конечных объёмов
        void init() override;
        //  UPDATE ON 02.07.2023 - основной шаг работы метода
        void run() override;
        void done() override;

        //  UPDATE ON 02.07.2023 - вычисление шага по времени (берётся либо TAU из task.yaml, либо вычисленное по числу Куранта)
        Real calcDt();

        void save();
        //  UPDATE ON 02.07.2023 - сохранение результатов расчётов в vtk-файл для визуализации
        void saveVtk();

        Array<Data> data;
        Array<Cons> integrals;

        Flux *flux;

        //  UPDATE ON 02.07.2023 - обнуление интегралов
        void seroIntegrals();
    };

}

#endif //CHARM_3D_V2_METHODFVM_H
