// Generate simple X across 2D box
nb_cells_side = 10;  // must be even

Point(1) = {0, 0, 0, 0.1};
Point(2) = {1, 0, 0, 0.1};
Point(3) = {1, 1, 0, 0.1};
Point(4) = {0, 1, 0, 0.1};

Line(1) = {1, 2};
Line(2) = {2, 3};
Line(3) = {3, 4};
Line(4) = {4, 1};

Transfinite Line{1,3} = nb_cells_side + 1;
Transfinite Line{2,4} = nb_cells_side + 1;
  

Curve Loop(1) = {1, 2, 3, 4};
Ruled Surface(1) = {1};
Transfinite Surface{1} = {1,2,3,4}; // points indices, ordered
Recombine Surface {1};

out[] = Extrude {0,0,0.5} {
  Surface{1};
  Layers{nb_cells_side/2};
  Recombine;
};

out[] = Extrude {0,0,-0.5} {
  Surface{1};
  Layers{nb_cells_side/2};
  Recombine;
};


Physical Volume("matrix") = {1,2};

Physical Surface("left") = {25,47};
Physical Surface("right") = {17,39};
Physical Surface("bottom") = {48};
Physical Surface("top") = {26};
Physical Surface("fault") = {1};
Physical Surface("front") = {13,35};
Physical Surface("back") = {21,43};


