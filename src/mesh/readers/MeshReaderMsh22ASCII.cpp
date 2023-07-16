//
// Created by zhrv on 13.11.2020.
//

#include "MeshReaderMsh22ASCII.h"
#include <sstream>
#include <map>
#include <set>

namespace charm {



    Mesh* MeshReaderMsh22ASCII::read() {
        Mesh* mesh = new Mesh();
        std::map<std::set<Index>, Face> faceMap;
        Index fid = 0;
        String line;
        std::fstream fin(fileName);
        std::stringstream ss;
        Int numPatches, numVert, numCells;

        std::map<String, BoundaryCondition*> bndPatchMap;
        for (auto b: config->boundaries) {
            bndPatchMap[b->name] = b;
        }

        while (1) {
            line = getlineUpper(fin);

            if (line == "") {
                break;
            }
            if (line[0] == '$') {
                if (line.find("$PHYSICALNAMES") != String::npos) {
                    ss << getlineUpper(fin);
                    ss >> numPatches; //sscanf(line, "%d", &numPatches);
                    mesh->patches.resize(numPatches);
                    for (int i = 0; i < numPatches; i++) {
                        ss << getlineUpper(fin);
                        Patch patch;
                        ss >> patch.dim >> patch.id >> patch.name;
                        delQuotes(patch.name);
                        patch.id--;
                        mesh->patches[patch.id] = patch;
                    }
                } else if (line.find("$NODES") != String::npos) {
                    ss << getlineUpper(fin);
                    ss >> numVert;
                    mesh->nodes.resize(numVert);
                    for (int i = 0; i < numVert; i++) {
                        ss << getlineUpper(fin);
                        Real x, y, z;
                        Int node;
                        Point p;
                        ss >> node >> p.x >> p.y >> p.z;
                        mesh->nodes[node-1] = p;
                    }
                } else if (line.find("$ELEMENTS") != String::npos) {
                    Index cid = 0;
                    ss << getlineUpper(fin);
                    ss >> numCells;
                    mesh->cells.clear();
                    for (int i = 0; i < numCells; i++) {
                        line = getlineUpper(fin);
                        Int node, type;
                        ss << line;
                        ss >> node >> type;

                        //  UPDATE ON 18.06.2023 - обработка кубических ячеек
                        if (type == 5) {
                            Cell cell;
                            Index tag1, tag2;
                            cell.type = type;
                            ss >> tag1 >> cell.tag >> tag2;
                            cell.tag--;
                            cell.nodesInd.resize(8);
                            ss >> cell.nodesInd[0];
                            ss >> cell.nodesInd[1];
                            ss >> cell.nodesInd[3];
                            ss >> cell.nodesInd[2];
                            ss >> cell.nodesInd[4];
                            ss >> cell.nodesInd[5];
                            ss >> cell.nodesInd[7];
                            ss >> cell.nodesInd[6];
                            for (int j = 0; j < 8; j++) {
                                --cell.nodesInd[j];
                            }
                            cell.id = cid++;
                            mesh->cells.push_back(cell);
                        }

                        //  UPDATE ON 18.06.2023 - обработка граней - четырёхугольников
                        else if (type == 3) {
                            Face face;
                            Index tag1, tag2;
                            face.type = type;
                            ss >> tag1 >> face.tag >> tag2;
                            face.tag--;
                            face.nodesInd.resize(4);
                            ss >> face.nodesInd[0];
                            ss >> face.nodesInd[1];
                            ss >> face.nodesInd[3];
                            ss >> face.nodesInd[2];
                            std::set<Index> fNodes;
                            for (int i = 0; i < 4; i++) {
                                fNodes.insert(--face.nodesInd[i]);
                            }
                            face.id = fid++;
                            face.bnd = bndPatchMap[mesh->patches[face.tag].name];
                            faceMap[fNodes] = face;
                        }
                    }
                }
            }
        }

        mesh->patchesCount = fid;

        for (int ic = 0; ic < mesh->cells.size(); ic++) {
            Cell &cell = mesh->cells[ic];

            //  UPDATE ON 18.06.2023 - заполнение доп.информации о кубических ячейках и их гранях
            //  (номера узлов, номера граней и т.д.)
            if (cell.type == 5) {
                for (int i = 0; i < 6; i++) {
                    std::set<Index> fNodes;

                    for (int j = 0; j < 4; j++) {
                        fNodes.insert(cell.nodesInd[Mesh::ftv[i][j]]);
                    }
                    auto it = faceMap.find(fNodes);
                    Face &f = faceMap[fNodes];
                    if (it == faceMap.end()) {
                        f.id = fid++;
                    }
                    f.nodesInd.resize(4);
                    for (int j = 0; j < 4; j++) {
                        f.nodesInd[j] = cell.nodesInd[Mesh::ftv[i][j]];
                    }
                    f.addCell(ic);
                    //mesh->faces.push_back(f);
                    cell.facesInd.push_back(f.id);
                    //faceMap[fNodes] = f;
                }
            }
        }
        mesh->faces.resize(faceMap.size());
        int i = 0;
        for (auto &item : faceMap) {
            Face &f = item.second;
            mesh->faces[f.id] = f;
            i++;
        }

        mesh->calcGeom();

        return mesh;
    }

    MeshReaderMsh22ASCII::MeshReaderMsh22ASCII(Config *config) {

        this->config      = config;
        //this->mesh        = mesh;
        this->fileName    = config->msh.fileName;
    }


    String MeshReaderMsh22ASCII::getlineUpper(std::fstream &stream) {
        char *line = new char[1024]/*CHARM_ALLOC (char, 1024)*/, *linep = line;
        Index lenmax = 1024, len = lenmax;
        int c;

        for (;;) {
            c = stream.get(); //c = fgetc (stream);
            c = toupper(c); //  UPDATE ON 18.06.2023 -  перевод символа в верхний регистр
            if (c == EOF && linep == line) {
                delete[] linep;
                return "";
            }

            if (--len == 0) {
                char *linen;

                len = lenmax;
                lenmax *= 2;

                //linen = CHARM_REALLOC (linep, char, lenmax);
                linen = (char *) realloc(linep, lenmax);
                if (linen == nullptr) {
                    delete[] linep;
                    return "";
                }

                line = linen + (line - linep);
                linep = linen;
            }
            if ((*line++ = (char) c) == '\n')
                break;
        }
        *line = '\0';

        return String(linep);
    }

}