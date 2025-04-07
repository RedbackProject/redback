// Simple trap charging mesh
x = 1;  // number of mesh cells per quadrant
nb_cells_x_left = x;
nb_cells_x_right = x;
nb_cells_y_bottom = x;
nb_cells_y_top = x;
nb_cells_z = 2*x;

Point(1) = {0, 0, 0, 0.1};
Point(2) = {0.5, 0, 0, 0.1};
Point(3) = {1, 0, 0, 0.1};
Point(4) = {0, 0.5, 0, 0.1};
Point(5) = {0.5, 0.5, 0, 0.1};
Point(6) = {1, 0.5, 0, 0.1};
Point(7) = {0, 1, 0, 0.1};
Point(8) = {0.5, 1, 0, 0.1};
Point(9) = {1, 1, 0, 0.1};
Line(1) = {1, 2};
Line(2) = {2, 3};
Line(3) = {4, 5};
Line(4) = {5, 6};
Line(5) = {7, 8};
Line(6) = {8, 9};
Line(7) = {1, 4};
Line(8) = {4, 7};
Line(9) = {2, 5};
Line(10) = {5, 8};
Line(11) = {3, 6};
Line(12) = {6, 9};

Transfinite Line{1,3,5} = nb_cells_x_left + 1;
Transfinite Line{2,4,6} = nb_cells_x_right + 1;
Transfinite Line{7,9,11} = nb_cells_y_bottom + 1;
Transfinite Line{8,10,12} = nb_cells_y_top + 1;

Curve Loop(1) = {1, 9, -3, -7};
Ruled Surface(1) = {1};
Transfinite Surface{1} = {1,2,5,4}; // points indices, ordered
Recombine Surface {1};

Curve Loop(2) = {2,11,-4,-9};
Ruled Surface(2) = {2};
Transfinite Surface{2} = {2,3,6,5}; // points indices, ordered
Recombine Surface {2};

Curve Loop(3) = {3,10,-5,-8};
Ruled Surface(3) = {3};
Transfinite Surface{3} = {4,5,8,7}; // points indices, ordered
Recombine Surface {3};

Curve Loop(4) = {4,12,-6,-10};
Ruled Surface(4) = {4};
Transfinite Surface{4} = {5,6,9,8}; // points indices, ordered
Recombine Surface {4};

out[] = Extrude {0,0,1} {
  Surface{1,2,3,4};
  Layers{nb_cells_z};
  Recombine;
};

Physical Volume("matrix") = {1,2,3,4};

Physical Surface("left") = {33,77};
Physical Surface("right") = {47,91};
Physical Surface("bottom") = {1,2,3,4};
Physical Surface("top") = {34,56,78,100};
Physical Surface("front") = {21,43};
Physical Surface("back") = {73,95};

Physical Surface("fault1") = {29};
Physical Surface("fault2") = {51};
Physical Surface("fault3") = {25,69};

