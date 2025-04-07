/****************************************************************/
/*               DO NOT MODIFY THIS HEADER                      */
/*     REDBACK - Rock mEchanics with Dissipative feedBACKs      */
/*                                                              */
/*              (c) 2014 CSIRO and UNSW Australia               */
/*                   ALL RIGHTS RESERVED                        */
/*                                                              */
/*            Prepared by CSIRO and UNSW Australia              */
/*                                                              */
/*            See COPYRIGHT for full restrictions               */
/****************************************************************/
/**
 * Utilities to deal with ellipses for modified Cam-Clay model.
 * Code for distance point-ellipse from Geometric Tools LLC, Redmond WA 98052
 * Copyright (c) 1998-2014
 * Distributed under the Boost Software License, Version 1.0.
 * http://www.boost.org/LICENSE_1_0.txt
 * http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
 * File Version: 1.0.0 (2014/08/11)
 */

#include "Ellipse.h"

Ellipse::Ellipse() {}

Real
Ellipse::sqrDistanceSpecial(Real const e[2], Real const y[2], Real x[2])
{
  Real sqr_distance;
  if (y[1] > (Real)0)
  {
    if (y[0] > (Real)0)
    {
      // Bisect to compute the root of F(t) for t >= -e1*e1.
      Real esqr[2] = {e[0] * e[0], e[1] * e[1]};
      Real ey[2] = {e[0] * y[0], e[1] * y[1]};
      Real t0 = -esqr[1] + ey[1];
      Real t1 = -esqr[1] + sqrt(ey[0] * ey[0] + ey[1] * ey[1]);
      Real t = t0;
      const int imax = 2 * std::numeric_limits<Real>::max_exponent;
      for (int i = 0; i < imax; ++i)
      {
        t = ((Real)0.5) * (t0 + t1);
        if (t == t0 || t == t1)
        {
          break;
        }

        Real r[2] = {ey[0] / (t + esqr[0]), ey[1] / (t + esqr[1])};
        Real f = r[0] * r[0] + r[1] * r[1] - (Real)1;
        if (f > (Real)0)
        {
          t0 = t;
        }
        else if (f < (Real)0)
        {
          t1 = t;
        }
        else
        {
          break;
        }
      }

      x[0] = esqr[0] * y[0] / (t + esqr[0]);
      x[1] = esqr[1] * y[1] / (t + esqr[1]);
      Real d[2] = {x[0] - y[0], x[1] - y[1]};
      sqr_distance = d[0] * d[0] + d[1] * d[1];
    }
    else // y0 == 0
    {
      x[0] = (Real)0;
      x[1] = e[1];
      Real diff = y[1] - e[1];
      sqr_distance = diff * diff;
    }
  }
  else // y1 == 0
  {
    Real denom0 = e[0] * e[0] - e[1] * e[1];
    Real e0y0 = e[0] * y[0];
    if (e0y0 < denom0)
    {
      // y0 is inside the subinterval.
      Real x0de0 = e0y0 / denom0;
      Real x0de0sqr = x0de0 * x0de0;
      x[0] = e[0] * x0de0;
      x[1] = e[1] * sqrt(fabs((Real)1 - x0de0sqr));
      Real d0 = x[0] - y[0];
      sqr_distance = d0 * d0 + x[1] * x[1];
    }
    else
    {
      // y0 is outside the subinterval.  The closest ellipse point has
      // x1 == 0 and is on the domain-boundary interval (x0/e0)^2 = 1.
      x[0] = e[0];
      x[1] = (Real)0;
      Real diff = y[0] - e[0];
      sqr_distance = diff * diff;
    }
  }
  return sqr_distance;
}

Real
Ellipse::sqrDistance(Real const e[2], Real const y[2], Real x[2])
{
  // Determine reflections for y to the first quadrant.
  bool reflect[2];
  int i, j;
  for (i = 0; i < 2; ++i)
  {
    reflect[i] = (y[i] < (Real)0);
  }

  // Determine the axis order for decreasing extents.
  int permute[2];
  if (e[0] < e[1])
  {
    permute[0] = 1;
    permute[1] = 0;
  }
  else
  {
    permute[0] = 0;
    permute[1] = 1;
  }

  int invpermute[2];
  for (i = 0; i < 2; ++i)
  {
    invpermute[permute[i]] = i;
  }

  Real locE[2], locY[2];
  for (i = 0; i < 2; ++i)
  {
    j = permute[i];
    locE[i] = e[j];
    locY[i] = y[j];
    if (reflect[j])
    {
      locY[i] = -locY[i];
    }
  }

  Real locX[2];
  Real sqr_distance = sqrDistanceSpecial(locE, locY, locX);

  // Restore the axis order and reflections.
  for (i = 0; i < 2; ++i)
  {
    j = invpermute[i];
    if (reflect[i])
    {
      locX[j] = -locX[j];
    }
    x[i] = locX[j];
  }

  return sqr_distance;
}

Real
Ellipse::distanceCC(Real const m,
                    Real const p_c,
                    Real const y0,
                    Real const y1,
                    Real & x0,
                    Real & x1,
                    Real shift_ellipse)
{
  Real e[2];         // ellipse axes
  Real x[2];         // point coordinates as array
  Real shifted_y[2]; // ellipse axes
  e[0] = fabs(p_c) / 2.0;
  e[1] = m * e[0];
  // Shift by pc/2 to centre the ellipse on (0,0)
  shifted_y[0] = y0 - p_c / 2.0 - shift_ellipse;
  shifted_y[1] = y1;
  Real d = sqrDistance(e, shifted_y, x);
  // Shift coordinates back to real space
  x0 = x[0] + p_c / 2.0 + shift_ellipse;
  x1 = x[1];
  return sqrt(d);
}

void
Ellipse::getYieldPointCC(
    Real const m, Real const p_c, Real const y0, Real const y1, Real & x0, Real & x1)
{
  if (y1 < 0)
  {
    // algorithm below only works for y1 > 0, so use symmetry
    Real neg_x1;
    getYieldPointCC(m, p_c, y0, -y1, x0, neg_x1);
    x1 = -neg_x1;
    return;
  }
  // Deal with easy cases first
  if (y0 == p_c / 2.0)
  {
    // Real t = -std::pow(m, 2)*std::log(-m*p_c/(2.0*y1))/2.0;
    x0 = p_c / 2.0;
    x1 = -m * p_c / 2.0;
    return;
  }
  else if (y1 == 0)
  {
    // Real t = -std::log(std::abs(p_c/(2*y0-p_c)))/2.0;
    if (y0 > p_c / 2.0)
      x0 = 0.0;
    else
      x0 = p_c;
    x1 = 0.0;
    return;
  }
  // For all other cases, we use Newton Raphson
  Real tol = 1e-15;       // tolerance on ellipse potential to be close to 0
  int nb_iter_max = 1000; // number of dichotomies to avoid infinite loops
  Real m2 = m * m;
  Real t = 0;
  Real phi = std::pow(y1 / m, 2) + std::pow(y0 - p_c / 2.0, 2) - p_c * p_c / 4.0;
  Real phi_prime = -4 * std::pow(y1 / m2, 2) - 4 * std::pow(y0 - p_c / 2.0, 2);
  int nb_iter = 0;
  while (std::abs(phi) > tol && nb_iter < nb_iter_max)
  {
    nb_iter += 1;
    t -= phi / phi_prime;
    phi = std::pow(y1 / m, 2) * std::exp(-4 * t / m2) +
          std::pow(y0 - p_c / 2.0, 2) * std::exp(-4 * t) - p_c * p_c / 4.0;
    phi_prime = -4 * std::pow(y1 / m2, 2) * std::exp(-4 * t / m2) -
                4 * std::pow(y0 - p_c / 2.0, 2) * std::exp(-4 * t);
  }
  if (nb_iter == nb_iter_max)
    mooseError("Newton Raphson failed to converge after ", nb_iter, " iterations.");
  x0 = p_c / 2.0 + (y0 - p_c / 2.0) * std::exp(-2 * t);
  x1 = y1 * std::exp(-2 * t / m2);
}

Real
Ellipse::distanceToCenteredEllipse(Real const horizontal_axis,
                                   Real const vertical_axis,
                                   Real const y0,
                                   Real const y1,
                                   Real & x0,
                                   Real & x1)
{
  Real e[2];         // ellipse axes
  Real x[2];         // point coordinates as array
  Real shifted_y[2]; // ellipse axes
  e[0] = horizontal_axis;
  e[1] = vertical_axis;
  shifted_y[0] = y0;
  shifted_y[1] = y1;
  Real d = sqrDistance(e, shifted_y, x);
  x0 = x[0];
  x1 = x[1];
  return sqrt(d);
}

Real
Ellipse::distanceCCanisotropic(Real const m,
                               Real const p_0,
                               Real const alpha,
                               Real const y0,
                               Real const y1,
                               Real & x0,
                               Real & x1)
{
  Real d; // distance to ellipse

  // 1) Get axes of rotated ellipse
  Real horizontal_axis, vertical_axis, center_p, center_q;
  getDafaliasEllipseAxesAndCentre(
      m, p_0, alpha, horizontal_axis, vertical_axis, center_p, center_q);

  // 2) Rotate space to get ellipse horizontal
  Real z0; // p-coordinate of rotated point (in space where ellipse is
           // horizontal)
  Real z1; // q-coordinate of rotated point (in space where ellipse is
           // horizontal)
  rotatePoint(m, p_0, alpha, true, y0, y1, z0, z1);

  // 3) get distance and projection to (normal) ellipse
  Real w0; // p-coordinate of projection point (in space where ellipse is
           // horizontal)
  Real w1; // q-coordinate of projection point (in space where ellipse is
           // horizontal)
  d = distanceToCenteredEllipse(horizontal_axis, vertical_axis, z0, z1, w0, w1);

  // 4) Rotate space back to original space
  rotatePoint(m, p_0, alpha, false, w0, w1, x0, x1);

  return d;
}

bool
Ellipse::isPointOutsideOfEllipse(Real const m, Real const p_c, Real const y0, Real const y1)
{
  return (std::pow(y1 / m, 2) + y0 * (y0 - p_c) > 0);
  /*
  // Check sum of squared distances to ellipse's foci
  Real f; // focal distance
  // Ellipse axes are p_c/2 and m*p_c/2, so major axis is p if m<1
  if (m < 1)
  {
    // p is the major axis
    f = 0.5 * std::sqrt((1 - m * m) * p_c * p_c); // p_c can be negative
    return (std::sqrt(std::pow(y0 - 0.5 * p_c + f, 2) + y1 * y1) +
              std::sqrt(std::pow(y0 - 0.5 * p_c - f, 2) + y1 * y1) >
            std::fabs(p_c));
  }
  else
  {
    // q is the major axis
    f = 0.5 * std::sqrt((m * m - 1) * p_c * p_c); // p_c can be negative
    return (std::sqrt(std::pow(y0 - 0.5 * p_c, 2) + (y1 + f) * (y1 + f)) +
              std::sqrt(std::pow(y0 - 0.5 * p_c, 2) + (y1 - f) * (y1 - f)) >
            m * std::fabs(p_c));
  }*/
}

bool
Ellipse::isPointOutsideOfEllipse2(Real const horizontal_axis,
                                  Real const vertical_axis,
                                  Real const y0,
                                  Real const y1)
{
  // Check sum of squared distances to ellipse's foci
  Real f; // focal distance
  if (horizontal_axis > vertical_axis)
  {
    f = std::sqrt(horizontal_axis * horizontal_axis - vertical_axis * vertical_axis);
    return (std::sqrt((y0 + f) * (y0 + f) + y1 * y1) + std::sqrt((y0 - f) * (y0 - f) + y1 * y1) >
            2 * horizontal_axis);
  }
  else
  {
    f = std::sqrt(vertical_axis * vertical_axis - horizontal_axis * horizontal_axis);
    return (std::sqrt((y1 + f) * (y1 + f) + y0 * y0) + std::sqrt((y1 - f) * (y1 - f) + y0 * y0) >
            2 * vertical_axis);
  }
}

bool
Ellipse::isPointOutsideOfRotatedEllipse(
    Real const m, Real const p_0, Real const alpha, Real const y0, Real const y1)
{
  // 1) Get axes of rotated ellipse
  Real horizontal_axis, vertical_axis, center_p, center_q;
  getDafaliasEllipseAxesAndCentre(
      m, p_0, alpha, horizontal_axis, vertical_axis, center_p, center_q);

  // 2) Rotate space to get ellipse horizontal
  Real z0; // p-coordinate of rotated point (in space where ellipse is
           // horizontal)
  Real z1; // q-coordinate of rotated point (in space where ellipse is
           // horizontal)
  rotatePoint(m, p_0, alpha, true, y0, y1, z0, z1);

  // 3) check if point is outside of (norma) ellipse
  return isPointOutsideOfEllipse2(horizontal_axis, vertical_axis, z0, z1);
}

void
Ellipse::rotatePoint(Real const m,
                     Real const p_0,
                     Real const alpha,
                     bool forward,
                     Real const y0,
                     Real const y1,
                     Real & z0,
                     Real & z1)
{
  Real beta; // angle of ~horizontal (when alpha=0) axis with X-axis
  Real center_p = p_0 / 2.0;
  Real center_q = alpha * p_0 / 2.0;
  if (m < 1)
  {
    beta = 0.5 * std::atan(2 * alpha / (1 - m * m));
  }
  else // m >= 1
  {
    beta = 0.5 * 3.14159265358979323846 + 0.5 * std::atan(2 * alpha / (1 - m * m));
  }
  Real cos_beta = std::cos(beta);
  Real sin_beta = std::sin(beta);
  if (forward)
  {
    z0 = cos_beta * (y0 - center_p) + sin_beta * (y1 - center_q);
    z1 = -sin_beta * (y0 - center_p) + cos_beta * (y1 - center_q);
  }
  else
  {
    z0 = center_p + cos_beta * y0 - sin_beta * y1;
    z1 = center_q + sin_beta * y0 + cos_beta * y1;
  }
}

void
Ellipse::getDafaliasEllipseAxesAndCentre(Real const m,
                                         Real const p_0,
                                         Real const alpha,
                                         Real & horizontal_axis,
                                         Real & vertical_axis,
                                         Real & center_p,
                                         Real & center_q)
{
  Real x = 1 + m * m + std::sqrt((m * m - 1) * (m * m - 1) + 4 * alpha * alpha);
  horizontal_axis = std::sqrt(((x * p_0 * p_0) / 8.0));
  // TODO: what is the correct formula  if ((m + alpha) * (m - alpha) < 0)? (not 0, not 1...)
  vertical_axis = std::sqrt(((m + alpha) * (m - alpha) * p_0 * p_0) / (2 * x));
  center_p = p_0 / 2.0;
  center_q = alpha * p_0 / 2.0;
}
