/**
 * @file
 * @brief Class for mesh model
 * @author R.V.Zhalnin
 * @date 2020-11-15
 */

#ifndef CHARM_3D_V2_MESH_H
#define CHARM_3D_V2_MESH_H

#include "global.h"
#include "Point.h"
#include "Cell.h"
#include "Face.h"

namespace charm {

    class MeshReader;

    class Mesh {
    public:
        typedef enum {
            UNKNOWN,
            GMSH_MSH,
            GMSH_INP,
            GMSH_UNV,
            OPENFOAM,
            SALOME_UNV,
            TETGEN
        } FileType;

        void calcGeom();

        Points cellGetVertices(Cell &c);
        Points cellGetVertices(Index ci);
        Points cellGetFaceVertices(Cell &c, Index fi);
        Points cellGetFaceVertices(Index ci, Index fi);
        ArrayIndex cellGetNeighIndexes(Index ci);
        void assign(const Mesh &msh);

        static void cellCalcGpAtPoint(Points vertices, Point ref_p, Point &gp, Real &gj);
        static void cellCalcGp(Cell &cell);
        static Point unitCubeToReal(Array<Point> vertices, Real x, Real y, Real z);

        static FileType getfileTypeByStr(String str);

        Point &getNode(Index iNode);
        Face &getFace(Index iFace);
        Cell &getCell(Index iCell);
        Real getCellVolume(Index iCell);
        Real getFaceArea(Index iFace);

        inline Index getNodesCount() const {
            return nCount;
        }

        inline Index getNodesCountWithGhost() const {
            return nCountGhost;
        }

        inline void nodesResize(Index size) {
            nodes.resize(size);
        }

        inline Index getCellsCount() const {
            return cCount;
        }

        inline Index getCellsCountWithGhost() const {
            return cCountGhost;
        }

        inline Index getCellsSize() const {
            return cells.size();
        }

        inline void cellsResize(Index size) {
            nodes.resize(size);
        }

        inline void cellsClear() {
            nodes.clear();
        }

        inline void cellPush(Cell &c) {
            cells.push_back(c);
        }

        inline Index getFacesCount() const {
            return fCount;
        }

        inline Index getFacesCountWithGhost() const {
            return fCountGhost;
        }

        inline void facesResize(Index size) {
            nodes.resize(size);
        }


        static const Index ftv[6][4];

    protected:
        Index               nCount;
        Index               nCountGhost;
        Points              nodes;     ///< Mesh nodes.

        Index               cCount;
        Index               cCountGhost;
        Array<Cell>         cells;     ///< Mesh cells.

        Index               fCount;
        Index               fCountGhost;
        Array<Face>         faces;     ///< Mesh faces.

        ArrayIndex          recvCount;
        ArrayIndex          recvShift;
        Array<ArrayIndex>   sendInd;


        friend class MeshReader;
        friend class Method;
    };


}

#endif //CHARM_3D_V2_MESH_H
