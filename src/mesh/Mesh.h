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

        explicit Mesh();

        void calcGeom();

        Points cellGetVertices(Cell &c);
        Points cellGetVertices(Index ci);
        Points cellGetFaceVertices(Cell &c, Index fi);
        Points cellGetFaceVertices(Index ci, Index fi);
        ArrayIndex cellGetNeighIndexes(Index ci);
        void assign(const Mesh &msh);

        static void cellHexCalcGpAtPoint(Points vertices, Point ref_p, Point &gp, Real &gj);
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
            nCount = size;
            nCountGhost = size;
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
            cCount = size;
            cCountGhost = size;
            cells.resize(size);
        }

        inline void cellsClear() {
            cells.clear();
        }

        inline void cellPush(Cell &c) {
            cells.push_back(c);
            cCount++;
            cCountGhost++;

        }

        inline Index getFacesCount() const {
            return fCount;
        }

        inline Index getFacesCountWithGhost() const {
            return fCountGhost;
        }


        Points faceGetVertices(Face &f);

        inline void facesResize(Index size) {
            fCount = size;
            fCountGhost = size;
            faces.resize(size);
        }


        static const Index FTV_QUAD[6][4];
        static const Index FTV_TET[4][3];
        static const Index FTV_PRISM[5][4];
        static const Index FTV_PYR[5][4];

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
        friend class Parallel;
    };


}

#endif //CHARM_3D_V2_MESH_H
