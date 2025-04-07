//+
Point(1) = {0, 0, 0, 0.2};
//+
Point(2) = {1, 0, 0, 0.2};
//+
Point(3) = {1, 1, 0, 0.2};
//+
Point(4) = {0, 1, 0, 0.2};
//+
Point(5) = {0, 0.75, 0, 0.2};
//+
Point(6) = {1, 0.25, 0, 0.2};
//+
Line(1) = {1, 2};
//+
Line(2) = {2, 6};
//+
Line(3) = {6, 3};
//+
Line(4) = {3, 4};
//+
Line(5) = {4, 5};
//+
Line(6) = {5, 1};
//+
Line(7) = {6, 5};
//+
//Line Loop(1) = {5, 6, 1, 2, 3, 4};
//+
//Plane Surface(1) = {1};
//+
//Line{7} In Surface{1};
//+
Physical Point("top_left_corner") = {4};
//+
Physical Line("top") = {4};
//+
Physical Line("bottom") = {1};
//+
Physical Line("left") = {6, 5};
//+
Physical Line("right") = {2, 3};
//+
Line Loop(2) = {5, -7, 3, 4};
//+
Plane Surface(2) = {2};
//+
Line Loop(3) = {6, 1, 2, 7};
//+
Plane Surface(3) = {3};
//+
Physical Surface("top_block") = {2};
//+
Physical Surface("bottom_block") = {3};
