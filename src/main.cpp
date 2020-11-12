#include <iostream>
#include "Point.h"


int main() {
    Point p;
    p.x = 11.3;
    p.y = 101.3;
    p.z = 34.456;

    for (auto i: {0,1,2}) {
        std::cout << p[i] << std::endl;
    }

    return 0;
}
