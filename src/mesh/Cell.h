/*
 *     ________  _____    ____  __  ___    _____ ____
 *    / ____/ / / /   |  / __ \/  |/  /   |__  // __ \  __    __
 *   / /   / /_/ / /| | / /_/ / /|_/ /     /_ </ / / /_/ /___/ /_
 *  / /___/ __  / ___ |/ _, _/ /  / /    ___/ / /_/ /_  __/_  __/
 *  \____/_/ /_/_/  |_/_/ |_/_/  /_/____/____/_____/ /_/   /_/
 *
 */



#ifndef CHARM_3D_V2_CELL_H
#define CHARM_3D_V2_CELL_H

#include "global.h"
#include "Point.h"


namespace charm {

    class Mesh;

    class Cell {
    public:
        Cell() = default;

        Cell(const Cell &c) {
            *this = c;
        }

        ~Cell() = default;

        Cell& operator = (const Cell &c) {
            id              = c.id;
            nodesInd.assign(c.nodesInd.begin(), c.nodesInd.end());
            facesInd.assign(c.facesInd.begin(), c.facesInd.end());
            type            = c.type;
            tag             = c.tag;
            volume          = c.volume;
            center          = c.center;
//            gp.assign(c.gp.begin(), c.gp.end());
//            gw.assign(c.gw.begin(), c.gw.end());
//            dh              = c.dh;
            flag            = c.flag;
            return *this;
        }

        ArrayIndex getNeigh();

        Index               id;
        ArrayIndex          nodesInd;
        ArrayIndex          facesInd;
        Int                 type;
        Int                 tag;
        Real                volume;
        Point               center;
//        Points              gp;
//        ArrayReal           gw;
//        Point               dh;
        Uint                flag;

        friend class Mesh;

    public:
        static const Uint CELL_FLAG_GOOD        = 0x000000;
        static const Uint CELL_FLAG_BAD         = 0x000001;
        static const Uint CELL_FLAG_LIM         = 0x000002;

        /**
         *   Tetrahedron:
         *
         *                      v
         *                    .
         *                  ,/
         *                 /
         *              2
         *            ,/|`\
         *          ,/  |  `\
         *        ,/    '.   `\
         *      ,/       |     `\
         *    ,/         |       `\
         *   0-----------'.--------1 --> u
         *    `\.         |      ,/
         *       `\.      |    ,/
         *          `\.   '. ,/
         *             `\. |/
         *                `3
         *                   `\.
         *                      ` w
         */
        static const Uint CELL_TYPE_TETRAHEDRON = 4;

        /**
         *
         *   Hexahedron (MSH-file):    Hexahedron (CHARM):
         *
         *          v
         *   3----------2              2----------3
         *   |\     ^   |\             |\         |\
         *   | \    |   | \            | \        | \
         *   |  \   |   |  \           |  \       |  \
         *   |   7------+---6          |   6------+---7
         *   |   |  +-- |-- | -> u     |   |      |   |
         *   0---+---\--1   |          0---+------1   |
         *    \  |    \  \  |           \  |       \  |
         *     \ |     \  \ |            \ |        \ |
         *      \|      w  \|             \|         \|
         *       4----------5              4----------5
         *
         *
         *
         */
        static const Uint CELL_TYPE_HEXAHEDRON  = 5;

        /**
         *
         *   Prism:
         *
         *              w
         *              ^
         *              |
         *              3
         *            ,/|`\
         *          ,/  |  `\
         *        ,/    |    `\
         *       4------+------5
         *       |      |      |
         *       |    ,/|`\    |
         *       |  ,/  |  `\  |
         *       |,/    |    `\|
         *      ,|      |      |\
         *    ,/ |      0      | `\
         *   u   |    ,/ `\    |    v
         *       |  ,/     `\  |
         *       |,/         `\|
         *       1-------------2
         *
         *
         */
        static const Uint CELL_TYPE_PRISM       = 6;

        /**
         *
         *   Pyramid:
         *
         *                  4
         *                ,/|\
         *              ,/ .'|\
         *            ,/   | | \
         *          ,/    .' | `.
         *        ,/      |  '.  \
         *      ,/       .' w |   \
         *    ,/         |  ^ |    \
         *   0----------.'--|-3    `.
         *    `\        |   |  `\    \
         *      `\     .'   +----`\ - \ -> v
         *        `\   |    `\     `\  \
         *          `\.'      `\     `\`
         *             1----------------2
         *                       `\
         *                          u
         *
         *
         *
         */
        static const Uint CELL_TYPE_PYRAMID     = 7;

    };
}

#endif //CHARM_3D_V2_CELL_H
