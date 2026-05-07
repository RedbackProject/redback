/***********************************************/
/* Parameters  */
xmin = 0;
xmax = 1;
ymin = -6;
ybase = -5;
y1 = -0.3; //-0.05; // bottom of middle layer
y2 = 0.3;  //0.05; // top of middle layer
ymax = 5;
/****** MESH ***********/
nb_cells_x = 1; // nb cells in X dir along fault

nb_cells_y_top = 10;
nb_cells_y_middle = 15;
nb_cells_y_bottom = 7;
nb_cells_y_base = 1;
progress_coeff_top = 0.7; // progression coefficient (for denser regular mesh towards fault)
progress_coeff_bottom = 0.5; // progression coefficient (for denser regular mesh towards fault)

/***********************************************/

lc1 = 1.0; // irrelevant
lc2 = 1.0; // irrelevant
Point(1) = {xmin, ybase, 0, lc1};
Point(2) = {xmax, ybase, 0, lc1};
Point(3) = {xmax, y1, 0, lc2};
Point(4) = {xmin, y1, 0, lc2};
Point(5) = {xmax, y2, 0, lc2};
Point(6) = {xmin, y2, 0, lc2};
Point(7) = {xmax, ymax, 0, lc1};
Point(8) = {xmin, ymax, 0, lc1};
Point(9) = {xmin, ymin, 0, lc1};
Point(10) = {xmax, ymin, 0, lc1};

Line(1) = {1,2};
Line(2) = {2,3};
Line(3) = {3,4};
Line(4) = {4,1};
Line(5) = {3,5};
Line(6) = {5,6};
Line(7) = {6,4};
Line(8) = {5,7};
Line(9) = {7,8};
Line(10) = {8,6};
Line(11) = {1,9};
Line(12) = {9,10};
Line(13) = {10,2};

Line Loop(1) = {1,2,3,4};
Line Loop(2) = {5,6,7,-3};
Line Loop(3) = {8,9,10,-6};
Line Loop(4) = {11,12,13,-1};

Printf("Buidling fully regular mesh (with hex)");
Transfinite Line{2,-4} = nb_cells_y_bottom + 1 Using Progression progress_coeff_bottom;
Transfinite Line{5,7} = nb_cells_y_middle + 1;
Transfinite Line{-8,10} = nb_cells_y_top + 1 Using Progression progress_coeff_top;
Transfinite Line{1,3,6,9,12} = nb_cells_x + 1;
Transfinite Line{11,13} = nb_cells_y_base + 1;

Ruled Surface(11) = {1};
Ruled Surface(12) = {2};
Ruled Surface(13) = {3};
Ruled Surface(14) = {4};
Transfinite Surface{11} = {1,2,3,4}; // points indices, ordered
Transfinite Surface{12} = {3,5,6,4};
Transfinite Surface{13} = {5,7,8,6};
Transfinite Surface{14} = {1,9,10,2};

Recombine Surface {11};
Recombine Surface {12};
Recombine Surface {13};
Recombine Surface {14};

Physical Curve("bottom", 15) = {12};      // bottom (in Y)
Physical Curve("right", 16) = {8,5,2};    // right (in X)
Physical Curve("top", 17) = {9};          // top (in Y)
Physical Curve("left", 18) = {10,7,4};    // left, 3 top layers (in X)
Physical Curve("left_base", 19) = {11};   // left, base layer (in X)
Physical Curve("right_base", 20) = {13};  // right, base layer (in X)

Physical Surface("top_block", 21) = {13};  // top block
Physical Surface("mid_block", 22) = {12};  // middle block
Physical Surface("bot_block", 23) = {11};  // bottom block
Physical Surface("base_block", 24) = {14}; // base block
