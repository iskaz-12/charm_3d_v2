//
// Created by zhrv on 13.11.2020.
//

#include "MeshReaderMsh22ASCII.h"
#include "MeshReaderException.h"
#include <sstream>
#include <map>
#include <set>
#include <iostream>
#include <iomanip>

namespace charm {



    Mesh* MeshReaderMsh22ASCII::read() {
        auto mesh = new Mesh();
        std::map<std::set<Index>, Face> faceMap;
        Index fid = 0;
        String line;
        std::fstream fin(fileName);
        if (!fin) {
            throw MeshReaderException("Unable to open mesh file '" + fileName + "'.");
        }

        std::stringstream ss;
        Int numPatches, numVert, numCells;

        std::map<String, BoundaryCondition*> bndPatchMap;
        for (Index i = 0; i < Config::getBndCount(); i++) {
            auto b = Config::getBnd(i);
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
                    patches.resize(numPatches);
                    for (int i = 0; i < numPatches; i++) {
                        ss << getlineUpper(fin);
                        Patch patch;
                        ss >> patch.dim >> patch.id >> patch.name;
                        delQuotes(patch.name);
                        patch.id--;
                        patches[patch.id] = patch;
                    }
                } else if (line.find("$NODES") != String::npos) {
                    ss << getlineUpper(fin);
                    ss >> numVert;
                    mesh->nodesResize(numVert);
                    for (int i = 0; i < numVert; i++) {
                        ss << getlineUpper(fin);
                        Real x, y, z;
                        Int node;
                        Point p;
                        ss >> node >> p.x >> p.y >> p.z;
                        mesh->getNode(node-1) = p;
                    }
                } else if (line.find("$ELEMENTS") != String::npos) {
                    Index cid = 0;
                    ss << getlineUpper(fin);
                    ss >> numCells;
                    mesh->cellsClear();
                    for (int i = 0; i < numCells; i++) {
                        line = getlineUpper(fin);
                        Int node, type;
                        ss << line;
                        ss >> node >> type;
                        if (type == Cell::CELL_TYPE_HEXAHEDRON) {
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
                            mesh->cellPush(cell);
                        }
                        else if (type == Cell::CELL_TYPE_TETRAHEDRON) {
                            Cell cell;
                            Index tag1, tag2;
                            cell.type = type;
                            ss >> tag1 >> cell.tag >> tag2;
                            cell.tag--;
                            cell.nodesInd.resize(4);
                            ss >> cell.nodesInd[0];
                            ss >> cell.nodesInd[1];
                            ss >> cell.nodesInd[2];
                            ss >> cell.nodesInd[3];
                            for (int j = 0; j < 4; j++) {
                                --cell.nodesInd[j];
                            }
                            cell.id = cid++;
                            mesh->cellPush(cell);
                        }
                        else if (type == Cell::CELL_TYPE_PRISM) {
                            // TODO
                        }
                        else if (type == Cell::CELL_TYPE_PYRAMID) {
                            // TODO
                        }
                        else if (type == Face::FACE_TYPE_TRIANGLE) {
                            Face face;
                            Index tag1, tag2;
                            face.type = type;
                            ss >> tag1 >> face.tag >> tag2;
                            face.tag--;
                            face.nodesInd.resize(3);
                            ss >> face.nodesInd[0];
                            ss >> face.nodesInd[1];
                            ss >> face.nodesInd[2];
                            std::set<Index> fNodes;
                            for (int ind = 0; ind < 3; ind++) {
                                fNodes.insert(--face.nodesInd[ind]);
                            }
                            face.id = fid++;
                            face.bnd = bndPatchMap[patches[face.tag].name];
                            faceMap[fNodes] = face;
                        }
                        else if (type == Face::FACE_TYPE_QUADRANGLE) {
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
                            face.bnd = bndPatchMap[patches[face.tag].name];
                            faceMap[fNodes] = face;
                        }
                    }
                }
            }
        }

        patchesCount = fid;

        for (Index ic = 0; ic < mesh->getCellsSize(); ic++) {
            Cell &cell = mesh->getCell(ic);
            if (cell.type == Cell::CELL_TYPE_HEXAHEDRON) {
                for (Int i = 0; i < 6; i++) {
                    std::set<Index> fNodes;

                    for (Int j = 0; j < 4; j++) {
                        fNodes.insert(cell.nodesInd[Mesh::FTV_QUAD[i][j]]);
                    }
                    auto it = faceMap.find(fNodes);
                    Face &f = faceMap[fNodes];
                    if (it == faceMap.end()) {
                        f.id = fid++;
                        f.type = Face::FACE_TYPE_QUADRANGLE;
                    }
                    f.nodesInd.resize(4);
                    for (Int j = 0; j < 4; j++) {
                        f.nodesInd[j] = cell.nodesInd[Mesh::FTV_QUAD[i][j]];
                    }
                    f.addCell(ic);
                    cell.facesInd.push_back(f.id);
                }
            }
            else if (cell.type == Cell::CELL_TYPE_TETRAHEDRON) {
                for (Int i = 0; i < 4; i++) {
                    std::set<Index> fNodes;

                    for (Int j = 0; j < 3; j++) {
                        fNodes.insert(cell.nodesInd[Mesh::FTV_TET[i][j]]);
                    }
                    auto it = faceMap.find(fNodes);
                    Face &f = faceMap[fNodes];
                    if (it == faceMap.end()) {
                        f.id = fid++;
                        f.type = Face::FACE_TYPE_TRIANGLE;
                    }
                    f.nodesInd.resize(3);
                    for (Int j = 0; j < 3; j++) {
                        f.nodesInd[j] = cell.nodesInd[Mesh::FTV_TET[i][j]];
                    }
                    f.addCell(ic);
                    cell.facesInd.push_back(f.id);
                }
            }
        }
        mesh->facesResize(faceMap.size());
        int i = 0;
        for (auto &item : faceMap) {
            Face &f = item.second;
            mesh->getFace(f.id) = f;
            i++;
        }



        decomp(mesh);
        mesh->calcGeom();


        return mesh;
    }

    MeshReaderMsh22ASCII::MeshReaderMsh22ASCII(Config* config) {

        this->config      = config;
        this->fileName    = config->msh.fileName;
    }


    String MeshReaderMsh22ASCII::getlineUpper(std::fstream &stream) {
        char *line = new char[1024], *linep = line;
        Index lenmax = 1024, len = lenmax;
        int c;

        for (;;) {
            c = stream.get(); //c = fgetc (stream);
            c = toupper(c);
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