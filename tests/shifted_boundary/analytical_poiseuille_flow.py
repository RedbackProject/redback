r''' Analytical solution to compute velocity at the center
    of a circular pipe, assuming Poiseuille flow.
                _______
             .-         -.
           .'             '.
          /                 \
         |                   |
         |<---- D = 2R ----->|
         |                   |
          \                 /
           '.             .'
             '-._______.-'

'''
import math

# Parameters
D = 1 * 400./500.   # diameter
R = D/2.            # radius
mu = 1.0            # viscosity
dp = 1.0            # pressure drop
L = 0.2             # channel length

# Poiseuille solution
u_max = dp * R**2 / (4.0 * mu * L)

# Integral of u over the cross-section (flow rate)
integral_u_cross = math.pi * R**4 * dp / (8.0 * mu * L)

# Integral of u over the whole volume
integral_u_volume = L * integral_u_cross

# Geometry
volume = math.pi * R**2 * L
surface_area = 2.0 * math.pi * R * L

print('u_max = ', u_max)
print('integral_u_cross = ', integral_u_cross)
print('integral_u_volume = ', integral_u_volume)
print('volume = ', volume)
print('surface_area = ', surface_area)