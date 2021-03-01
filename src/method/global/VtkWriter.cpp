/*
 *     ________  _____    ____  __  ___    _____ ____
 *    / ____/ / / /   |  / __ \/  |/  /   |__  // __ \  __    __
 *   / /   / /_/ / /| | / /_/ / /|_/ /     /_ </ / / /_/ /___/ /_
 *  / /___/ __  / ___ |/ _, _/ /  / /    ___/ / /_/ /_  __/_  __/
 *  \____/_/ /_/_/  |_/_/ |_/_/  /_/____/____/_____/ /_/   /_/
 *
 */


#include <iomanip>
#include "Mesh.h"
#include "Parallel.h"
#include "VtkWriter.h"
#include "Config.h"
#include "MethodException.h"

namespace charm {

    VtkWriter::VtkWriter(Method* m) : method(m) {}

    void VtkWriter::write(Index step) {
        Mesh& mesh = Config::getMesh();
        Index cScalarFields = method->getScalarFieldsCount();
        Index cVectorFields = method->getVectorFieldsCount();

        Array<String> fNames;


        for (Index p = 0; p < Parallel::procCount; p++) {
            std::stringstream ss;
            ss << Config::get().methodName << "_";
            ss << std::setw(10) << std::setfill('0') << step << ".";
            ss << std::setw(4) << std::setfill('0') << p;
            ss << ".vtu";
            fNames.push_back(ss.str());
        }

        Array<String> scalarFieldNames;
        String scalarFieldNamesStr;
        for (int iFld = 0; iFld < cScalarFields; iFld++) {
            String name = method->getScalarFieldName(iFld);
            scalarFieldNames.push_back(name);
            if (iFld != 0) {
                scalarFieldNamesStr += ", ";
            }
            scalarFieldNamesStr += name;
        }

        Array<String> vectorFieldNames;
        String vectorFieldNamesStr;
        for (int iFld = 0; iFld < cVectorFields; iFld++) {
            String name = method->getVectorFieldName(iFld);
            vectorFieldNames.push_back(name);
            if (iFld != 0) {
                vectorFieldNamesStr += ", ";
            }
            vectorFieldNamesStr += name;
        }

        FILE *fp;
        if (Parallel::isRoot()) {
            std::stringstream ss;
            ss << Config::get().methodName << ".";
            ss << std::setw(10) << std::setfill('0') << step;
            ss << ".pvtu";
            String fileName = ss.str();

            fp = fopen(fileName.c_str(), "w");
            if (!fp) {
                throw MethodException("Could not open VTK file.");
            }
            fprintf(fp, "<?xml version=\"1.0\"?>\n");
            fprintf(fp, "<VTKFile type=\"PUnstructuredGrid\" version=\"0.1\" byte_order=\"LittleEndian\">\n");
            fprintf(fp, "  <PUnstructuredGrid GhostLevel=\"0\">\n");
            fprintf(fp, "    <PPoints><PDataArray type=\"Float32\" NumberOfComponents = \"3\" /></PPoints>\n");
            fprintf(fp, "    <PCellData Scalars=\"%s, ProcId\" Vectors=\"%s, ProcId\">\n", scalarFieldNamesStr.c_str(), vectorFieldNamesStr.c_str());
            for (auto name: scalarFieldNames) {
                fprintf(fp, "      <PDataArray type=\"Float32\" Name=\"%s\" format=\"ascii\" />\n", name.c_str());
            }
            for (auto name: vectorFieldNames) {
                fprintf(fp, "      <PDataArray type=\"Float32\" Name=\"%s\" format=\"ascii\" NumberOfComponents=\"3\" />\n", name.c_str());
            }
            fprintf(fp, "      <PDataArray type=\"Int32\" Name=\"ProcId\" format=\"ascii\" />\n");
            fprintf(fp, "    </PCellData>\n");

            for (int pid = 0; pid < Parallel::procCount; pid++) {
                fprintf(fp, "    <Piece Source=\"%s\"/>\n", fNames[pid].c_str());
            }
            fprintf(fp, "  </PUnstructuredGrid>\n");

            fprintf(fp, "</VTKFile>\n");
            fclose(fp);
        }


        fp = fopen(fNames[Parallel::procId].c_str(), "w");
        fprintf(fp, "<?xml version=\"1.0\"?>\n");
        fprintf(fp, "<VTKFile type=\"UnstructuredGrid\" version=\"0.1\" byte_order=\"LittleEndian\">\n");
        fprintf(fp, "  <UnstructuredGrid GhostLevel=\"0\">\n");
        fprintf(fp, "    <Piece NumberOfPoints=\"%lu\" NumberOfCells=\"%lu\">\n", mesh.getNodesCount(), mesh.getCellsCount());
        fprintf(fp, "      <Points>\n");
        fprintf(fp, "        <DataArray type=\"Float32\" NumberOfComponents=\"3\" format=\"ascii\">\n");
        fprintf(fp, "          ");
        for (int iNode = 0; iNode < mesh.getNodesCount(); iNode++) {
            fprintf(fp, "%25.15f %25.15f %25.15f ", mesh.getNode(iNode).x, mesh.getNode(iNode).y, mesh.getNode(iNode).z);
        }
        fprintf(fp, "\n");
        fprintf(fp, "        </DataArray>\n");
        fprintf(fp, "      </Points>\n");
        fprintf(fp, "      <Cells>\n");
        fprintf(fp, "        <DataArray type=\"Int32\" Name=\"offsets\" format=\"ascii\">\n");
        fprintf(fp, "          ");

        Int offset = 0;
        for (Int i = 0; i < mesh.getCellsCount(); i++) {
            Cell &c = mesh.getCell(i);
            if (c.type == Cell::CELL_TYPE_HEXAHEDRON) {
                offset += 8;
                fprintf(fp, "%d ", offset);
            }
            else if (c.type == Cell::CELL_TYPE_TETRAHEDRON) {
                offset += 4;
                fprintf(fp, "%d ", offset);
            }
            else {
                throw Exception("VTK writer: not realized cell type.");
            }
        }
        fprintf(fp, "\n");
        fprintf(fp, "        </DataArray>\n");
        fprintf(fp, "        <DataArray type=\"Int32\" Name=\"connectivity\" format=\"ascii\">\n");
        fprintf(fp, "          ");
        for (int i = 0; i < mesh.getCellsCount(); i++) {
            Cell &c = mesh.getCell(i);
            if (c.type == Cell::CELL_TYPE_HEXAHEDRON) {
                fprintf(fp, "%lu %lu %lu %lu %lu %lu %lu %lu ",
                        c.nodesInd[0], c.nodesInd[1], c.nodesInd[2], c.nodesInd[3],
                        c.nodesInd[4], c.nodesInd[5], c.nodesInd[6], c.nodesInd[7]);
            }
            else if (c.type == Cell::CELL_TYPE_TETRAHEDRON) {
                fprintf(fp, "%lu %lu %lu %lu ", c.nodesInd[0], c.nodesInd[1], c.nodesInd[2], c.nodesInd[3]);
            }
            else {
                throw Exception("VTK writer: not realized cell type.");
            }
        }
        fprintf(fp, "\n");
        fprintf(fp, "        </DataArray>\n");
        fprintf(fp, "        <DataArray type=\"UInt8\" Name=\"types\" format=\"ascii\">\n");
        fprintf(fp, "          ");
        for (int i = 0; i < mesh.getCellsCount(); i++) {
            Cell &c = mesh.getCell(i);
            if (c.type == Cell::CELL_TYPE_HEXAHEDRON) {
                fprintf(fp, "%d ", 11);
            }
            else if (c.type == Cell::CELL_TYPE_TETRAHEDRON) {
                fprintf(fp, "%d ", 10);
            }
            else {
                throw Exception("VTK writer: not realized cell type.");
            }

        }
        fprintf(fp, "\n");
        fprintf(fp, "        </DataArray>\n");
        fprintf(fp, "      </Cells>\n");


        fprintf(fp, "      <CellData Scalars=\"%s, ProcId\">\n", scalarFieldNamesStr.c_str());


        for (Index iFld = 0; iFld < cScalarFields; iFld++) {
            // плотность
            fprintf(fp, "        <DataArray type=\"Float32\" Name=\"%s\" format=\"ascii\">\n",
                    scalarFieldNames[iFld].c_str());
            fprintf(fp, "          ");
            for (int i = 0; i < mesh.getCellsCount(); i++) {
                fprintf(fp, "%f ", method->getScalarFieldValue(i, iFld));
                if (i + 1 % 8 == 0 || i + 1 == mesh.getCellsCount()) fprintf(fp, "\n");
            }
            fprintf(fp, "        </DataArray>\n");
        }
        for (Index iFld = 0; iFld < cVectorFields; iFld++) {
            fprintf(fp, "        <DataArray type=\"Float32\" Name=\"%s\" format=\"ascii\" NumberOfComponents=\"3\">\n",
                    vectorFieldNames[iFld].c_str());
            fprintf(fp, "          ");
            for (int i = 0; i < mesh.getCellsCount(); i++) {
                Vector v = method->getVectorFieldValue(i, iFld);
                fprintf(fp, "%f %f %f ", v.x, v.y, v.z);
                if (i + 1 % 8 == 0 || i + 1 == mesh.getCellsCount()) fprintf(fp, "\n");
            }
            fprintf(fp, "        </DataArray>\n");
        }
        fprintf(fp, "        <DataArray type=\"Int32\" Name=\"ProcId\" format=\"ascii\">\n");
        fprintf(fp, "          ");
        for (int i = 0; i < mesh.getCellsCount(); i++) {
            fprintf(fp, "%d ", Parallel::procId);
            if (i + 1 % 8 == 0 || i + 1 == mesh.getCellsCount()) fprintf(fp, "\n");
        }
        fprintf(fp, "        </DataArray>\n");

        fprintf(fp, "      </CellData>\n");


        fprintf(fp, "    </Piece>\n");


        fprintf(fp, "  </UnstructuredGrid>\n");

        fprintf(fp, "</VTKFile>\n");
        fclose(fp);
    }
}