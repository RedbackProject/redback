// Simple trap charging mesh
x = 4;  // mesh refinement factor
nb_cells_x_left = 5*x;
nb_cells_x_right = 5*x;
nb_cells_y_bottom = 3*x;
nb_cells_y_middle = 3*x;
nb_cells_y_top = 4*x;
nb_cells_z = 1;

Point(1) = {0, 0, 0, 0.1};
Point(2) = {0.5, 0, 0, 0.1};
Point(3) = {1, 0, 0, 0.1};
Point(4) = {0, 0.3, 0, 0.1};
Point(5) = {0.5, 0.3, 0, 0.1};
Point(6) = {1, 0.3, 0, 0.1};
Point(7) = {0, 0.6, 0, 0.1};
Point(8) = {0.5, 0.6, 0, 0.1};
Point(9) = {1, 0.6, 0, 0.1};
Point(10) = {0, 1, 0, 0.1};
Point(11) = {0.5, 1, 0, 0.1};
Point(12) = {1, 1, 0, 0.1};

Line(1) = {1, 2};
Line(2) = {2, 3};
Line(3) = {4, 5};
Line(4) = {5, 6};
Line(5) = {7, 8};
Line(6) = {8, 9};
Line(7) = {10, 11};
Line(8) = {11, 12};
Line(9) = {1, 4};
Line(10) = {4, 7};
Line(11) = {7, 10};
Line(12) = {2, 5};
Line(13) = {5, 8};
Line(14) = {8, 11};
Line(15) = {3, 6};
Line(16) = {6, 9};
Line(17) = {9, 12};

Transfinite Line{1,3,5,7} = nb_cells_x_left + 1;
Transfinite Line{2,4,6,8} = nb_cells_x_right + 1;
Transfinite Line{9,12,15} = nb_cells_y_bottom + 1;
Transfinite Line{10,13,16} = nb_cells_y_middle + 1;
Transfinite Line{11,14,17} = nb_cells_y_top + 1;

Curve Loop(1) = {1, 12, -3, -9};
Ruled Surface(1) = {1};
Transfinite Surface{1} = {1,2,5,4}; // points indices, ordered
Recombine Surface {1};

Curve Loop(2) = {2,15,-4,-12};
Ruled Surface(2) = {2};
Transfinite Surface{2} = {2,3,6,5}; // points indices, ordered
Recombine Surface {2};

Curve Loop(3) = {3,13,-5,-10};
Ruled Surface(3) = {3};
Transfinite Surface{3} = {4,5,8,7}; // points indices, ordered
Recombine Surface {3};

Curve Loop(4) = {4,16,-6,-13};
Ruled Surface(4) = {4};
Transfinite Surface{4} = {5,6,9,8}; // points indices, ordered
Recombine Surface {4};

Curve Loop(5) = {5,14,-7,-11};
Ruled Surface(5) = {5};
Transfinite Surface{5} = {7,8,11,10}; // points indices, ordered
Recombine Surface {5};

Curve Loop(6) = {6,17,-8,-14};
Ruled Surface(6) = {6};
Transfinite Surface{6} = {8,9,12,11}; // points indices, ordered
Recombine Surface {6};


out[] = Extrude {0,0,0.1} {
  Surface{1,2,3,4,5,6};
  Layers{nb_cells_z};
  Recombine;
};

Physical Volume("block1") = {1};
Physical Volume("block2") = {6};
Physical Volume("matrix") = {2,3,4,5};

Physical Surface("left") = {38,82,126};
Physical Surface("right") = {52,96,140};
Physical Surface("bottom") = {26,48};
Physical Surface("bottom_block1") = {26};
Physical Surface("top") = {122,144};
Physical Surface("front") = {39,61,83,105,127,149};
Physical Surface("back") = {1,2,3,4,5,6};

Physical Surface("fault_vertical") = {30,74,118};
Physical Surface("top_block1") = {34};
Physical Surface("bottom_block2") = {100};
