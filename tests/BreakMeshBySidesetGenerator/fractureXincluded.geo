// Generate truncated X pattern across 2D box
lc = 0.3;
lc2 = 0.2;

Point(1) = {0, 0, 0, lc};
Point(2) = {1, 0, 0, lc};
Point(3) = {1, 1, 0, lc};
Point(4) = {0, 1, 0, lc};

Point(5) = {0.5, 0.5, 0, lc2};

Point(6) = {0.3, 0.2, 0, lc2};
Point(7) = {0.7, 0.8, 0, lc2};
Point(8) = {0.7, 0.1, 0, lc2};
Point(9) = {0.3, 0.9, 0, lc2};

Line(1) = {4, 1};
Line(2) = {1, 2};
Line(9) = {2, 3};
Line(10) = {3, 4};

Line(5) = {6, 5};
Line(6) = {5, 7};
Line(7) = {9, 5};
Line(8) = {5, 8};


Curve Loop(1) = {1, 2, 9, 10};
Plane Surface(1) = {1};
SetFactory("OpenCASCADE");
Curve{5, 6, 7, 8} In Surface{1};
Physical Surface("block") = {1};

Physical Curve("ss2") = {5,6};
Physical Curve("ss3") = {7,8};
Physical Curve("bottom") = {2};
Physical Curve("top") = {10};
Physical Curve("right") = {9};
Physical Curve("left") = {1};




