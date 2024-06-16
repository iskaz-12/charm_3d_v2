
N = 10;
CellSize = 0;

Point(1)={ 0,  0, 0, CellSize};
Point(2)={ 1,  0, 0, CellSize};
Point(3)={ 1,  1, 0, CellSize};
Point(4)={ 0,  1, 0, CellSize};

Line(1)  = {1,2};
Line(2)  = {2,3};
Line(3)  = {3,4};
Line(9)  = {1,4};

Line Loop(101) = {1,2,3,-9};
Plane Surface(1) = {101};

Transfinite Line {  1,2,3,-9 }  = N+1   Using Progression 1;

Transfinite Surface (1);

Recombine Surface (1);

Extrude {0, 0, 1} {
Surface{1}; Layers{N}; Recombine;
}

//+
Physical Volume("CUBE") = {1};

//+
Physical Surface("FRONT") = {123};
//+
Physical Surface("BACK") = {1};

//+
Physical Surface("LEFT") = {122};
//+
Physical Surface("RIGHT") = {114};

//+
Physical Surface("TOP") = {118};
//+
Physical Surface("BOTTOM") = {110};

Mesh.MshFileVersion = 2.2;
