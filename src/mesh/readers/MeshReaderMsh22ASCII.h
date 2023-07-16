//
// Created by zhrv on 13.11.2020.
//

#ifndef CHARM_3D_V2_MESHREADERMSH22ASCII_H
#define CHARM_3D_V2_MESHREADERMSH22ASCII_H
#include <fstream>
#include "MeshReader.h"
namespace charm {
    class MeshReaderMsh22ASCII : public MeshReader {
    public:
        MeshReaderMsh22ASCII(Config *config);   //  UPDATE ON 18.06.2023 - конструктор сетки

        virtual Mesh* read();   //  UPDATE ON 18.06.2023 - генерация сетки по содержимому файла

    protected:

    private:

        /*
         * Read a line from a file. Obtained from:
         * http://stackoverflow.com/questions/314401/
         * how-to-read-a-line-from-the-console-in-c/314422#314422
         *
         * Using this avoids a dependence on IEEE Std 1003.1-2008 (``POSIX.1'') for the
         * getline function.
         */
        static String getlineUpper(std::fstream &stream);   //  UPDATE ON 18.06.2023 - считывание строки и её перевод в верхний регистр

    };

}
#endif //CHARM_3D_V2_MESHREADERMSH22ASCII_H
