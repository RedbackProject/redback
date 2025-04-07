/**
 * Code from Geometric Tools LLC, Redmond WA 98052
 * Copyright (c) 1998-2014
 * Distributed under the Boost Software License, Version 1.0.
 * http://www.boost.org/LICENSE_1_0.txt
 * http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
 * File Version: 1.0.0 (2014/08/11)
 */

#ifndef ELLIPSE_H
#define ELLIPSE_H

#include "GeneralUserObject.h"

/**
 * Utilities to calculate distance from point to ellipse
 * Code from
 * http://www.geometrictools.com/Documentation/DistancePointEllipseEllipsoid.pdf
 * (for modified Cam-Clay model)
 */
class Ellipse
{
public:
  Ellipse();

  /**
   * The ellipse is (x0/e0)^2 + (x1/e1)^2 = 1 with e0 >= e1. The query point
   * is (y0,y1) with y0 >= 0 and y1 >= 0. The function returns the squared
   * distance from the query point to the ellipse. It also computes the
   * ellipse point (x0,x1) in the first quadrant that is closest to (y0,y1).
   * @param e: vector of ellipse axes
   * @param y: coordinates of point to project
   * @param x: coordinates of projection point on the ellipse
   * @return d: distance from y to ellipse
   */
  static Real sqrDistanceSpecial(Real const e[2], Real const y[2], Real x[2]);

  /**
   * The ellipse is (x0/e0)^2 + (x1/e1)^2 = 1. The query point is (y0,y1). The
   * function returns the squared distance from the query point to the ellipse.
   * It also computes the ellipse point (x0,x1) that is closest to (y0,y1).
   * @param e: vector of ellipse axes
   * @param y: coordinates of point to project
   * @param x: coordinates of projection point on the ellipse
   * @return d: distance from y to ellipse
   */
  static Real sqrDistance(Real const e[2], Real const y[2], Real x[2]);

  /**
   * Function to check if given point (y0, y1) is outside of ellipse (plasticity
   * zone for modified Cam-Clay model).
   * @param m: slope of the critical state line
   * @param p_c: pre-consolidation pressure
   * @param y: coordinates of point to check
   * @return result: boolean, true if strictly outside ellipse
   */
  static bool isPointOutsideOfEllipse(Real const m, Real const p_c, Real const y0, Real const y1);

  /**
   * Function to check if given point (y0, y1) is outside of ellipse defined by
   * its horizontal and vertical axes, centered on (0, 0)
   * @param horizontal_axis: horizontal axis
   * @param vertical_axis: vertical axis
   * @param y: coordinates of point to check
   * @return result: boolean, true if strictly outside ellipse
   */
  static bool isPointOutsideOfEllipse2(Real const horizontal_axis,
                                       Real const vertical_axis,
                                       Real const y0,
                                       Real const y1);

  /**
   * Function to check if given point (y0, y1) is outside of rotated and shifted
   * ellipse
   * (plasticity zone for Dafalias 2013 anisotropic modified Cam-Clay model).
   * @param m: slope of the critical state line
   * @param p_c: pre-consolidation pressure
   * @param alpha: Dafalias anisotropic parameter (angle)
   * @param y: coordinates of point to check
   * @return result: boolean, true if strictly outside ellipse
   */
  static bool isPointOutsideOfRotatedEllipse(
      Real const m, Real const p_0, Real const alpha, Real const y0, Real const y1);

  /**
   * Function to compute the distance from a query point to an ellipse
   * defined by the two parameters M and p_c from the modified Cam-Clay model.
   * It also computes the ellipse point (x0,x1) that is closest to (y0,y1).
   * @param m: slope of the critical state line
   * @param p_c: pre-consolidation pressure
   * @param y: coordinates of point to project
   * @param x: coordinates of projection point on the ellipse
   * @return d: distance to ellipse
   */
  static Real distanceCC(Real const m,
                         Real const p_c,
                         Real const y0,
                         Real const y1,
                         Real & x0,
                         Real & x1,
                         Real shift_ellipse);

  /**
   * Function to compute the yield point coordinates from a query point
   * to an ellipse (defined by the two parameters M and p_c from the
   * modified Cam-Clay model) by following the flow vector.
   * @param m: slope of the critical state line
   * @param p_c: pre-consolidation pressure
   * @param y: coordinates of point to project
   * @param x: coordinates of projection point on the ellipse
   * @return d: distance to ellipse
   */
  static void
  getYieldPointCC(Real const m, Real const p_c, Real const y0, Real const y1, Real & x0, Real & x1);

  /**
   * Function to compute the distance from a query point to an ellipse
   * defined by the two parameters M and p_c from the modified Cam-Clay model.
   * It also computes the ellipse point (x0,x1) that is closest to (y0,y1).
   * @param horizontal_axis: length of ellipse's ~horizontal (for alpha=0) axis
   * @param vertical_axis: length of ellipse's ~vertical (for alpha=0) axis
   * @param y: coordinates of point to project
   * @param x: coordinates of projection point on the ellipse
   * @return d: distance to ellipse
   */
  static Real distanceToCenteredEllipse(Real const horizontal_axis,
                                        Real const vertical_axis,
                                        Real const y0,
                                        Real const y1,
                                        Real & x0,
                                        Real & x1);

  /**
   * Function to compute the distance from a query point to a rotated ellipse
   * defined by the 3 parameters M, p_c and alpha from the 2013 Dafalias
   * anisotropic
   * modified Cam-Clay model.
   * It also computes the ellipse point (x0,x1) that is closest to (y0,y1).
   * @param m: slope of the critical state line
   * @param p_c: pre-consolidation pressure
   * @param alpha: Dafalias anisotropic parameter (angle)
   * @param y: coordinates of point to project
   * @param x: coordinates of projection point on the ellipse
   * @return d: distance to ellipse
   */
  static Real distanceCCanisotropic(Real const m,
                                    Real const p_0,
                                    Real const alpha,
                                    Real const y0,
                                    Real const y1,
                                    Real & x0,
                                    Real & x1);

  /**
   * Function to rotate and translate point in new space where Dafalias ellipse
   * is horizontal
   * @param m: slope of the critical state line
   * @param p_c: pre-consolidation pressure
   * @param alpha: Dafalias anisotropic parameter (angle)
   * @param forward: boolean, true to get to rotated space, false to get
   * backwards
   * @param y: coordinates of point to rotate & translate
   * @param z: coordinates of rotated & translated point
   */
  static void rotatePoint(Real const m,
                          Real const p_0,
                          Real const alpha,
                          bool forward,
                          Real const y0,
                          Real const y1,
                          Real & z0,
                          Real & z1);

  /**
   * Function to get axes of Dafalias ellipse
   * @param m: slope of the critical state line
   * @param p_c: pre-consolidation pressure
   * @param alpha: Dafalias anisotropic parameter (angle)
   * @param horizontal_axis: length of ellipse's ~horizontal (for alpha=0) axis
   * @param vertical_axis: length of ellipse's ~vertical (for alpha=0) axis
   * @param center_p: 1st coordinate of ellipse's center
   * @param center_q: 2nd coordinate of ellipse's center
   */
  static void getDafaliasEllipseAxesAndCentre(Real const m,
                                              Real const p_0,
                                              Real const alpha,
                                              Real & horizontal_axis,
                                              Real & vertical_axis,
                                              Real & center_p,
                                              Real & center_q);
};

#endif // ELLIPSE_H
