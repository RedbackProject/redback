//+
Point(1) = {0, 0, 0, 0.3};
//+
Point(2) = {2.1, 0, 0, 0.3};
//+
Point(3) = {2.1, 1, 0, 0.3};
//+
Point(4) = {0, 1, 0, 0.3};
//+
Point(5) = {1-0.1, 0, 0, 0.05};
//+
Point(6) = {1+0.1, 1, 0, 0.05};
//+
Point(7) = {1.1-0.1, 0, 0, 0.05};
//+
Point(8) = {1.1+0.1, 1, 0, 0.05};
//+
Line(1) = {4, 1};
//+
Line(2) = {1, 5};
//+
Line(3) = {5, 7};
//+
Line(4) = {7, 2};
//+
Line(5) = {2, 3};
//+
Line(6) = {3, 8};
//+
Line(7) = {8, 6};
//+
Line(8) = {6, 4};
//+
Line(9) = {5, 6};
//+
Line(10) = {7, 8};
//+
Line Loop(1) = {1, 2, 9, 8};
//+
Plane Surface(1) = {1};
//+
Line Loop(2) = {9, -7, -10, -3};
//+
Plane Surface(2) = {2};
//+
Line Loop(3) = {6, -10, 4, 5};
//+
Plane Surface(3) = {3};
//+
Physical Line("left") = {1};
//+
Physical Line("right") = {5};
//+
Physical Line("top_to_left") = {8};
//+
Physical Line("top_to_middle") = {7};
//+
Physical Line("top_to_right") = {6};
//+
Physical Line("bottom_to_right") = {4};
//+
Physical Line("bottom_to_middle") = {3};
//+
Physical Line("bottom_to_left") = {2};
//+
Physical Line("fault_left") = {9};
//+
Physical Line("fault_right") = {10};
//+
Physical Surface("left_block") = {1};
//+
Physical Surface("middle_block") = {2};
//+
Physical Surface("right_block") = {3};
