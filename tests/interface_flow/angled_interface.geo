//+
Point(1) = {0, 0, 0, 0.3};
//+
Point(2) = {2, 0, 0, 0.3};
//+
Point(3) = {2, 1, 0, 0.3};
//+
Point(4) = {0, 1, 0, 0.3};
//+
Point(5) = {1-0.1, 0, 0, 0.05};
//+
Point(6) = {1+0.1, 1, 0, 0.05};
//+
Line(1) = {4, 1};
//+
Line(2) = {1, 5};
//+
Line(4) = {5, 2};
//+
Line(5) = {2, 3};
//+
Line(6) = {3, 6};
//+
Line(8) = {6, 4};
//+
Line(9) = {5, 6};
//+
Line Loop(1) = {1, 2, 9, 8};
//+
Plane Surface(1) = {1};
//+
Line Loop(3) = {6, -9, 4, 5};
//+
Plane Surface(3) = {3};
//+
Physical Line("left") = {1};
//+
Physical Line("right") = {5};
//+
Physical Line("top_to_left") = {8};
//+
Physical Line("top_to_right") = {6};
//+
Physical Line("bottom_to_right") = {4};
//+
Physical Line("bottom_to_left") = {2};
//+
Physical Line("fault_left") = {9};
//+
Physical Line("fault_right") = {10};
//+
Physical Surface("left_block") = {1};
//+
Physical Surface("right_block") = {3};
