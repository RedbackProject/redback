# API Reference

## Overview

Redback provides a comprehensive set of MOOSE kernels, materials, and boundary conditions for multiphysics simulations.

## Kernels

Kernels define the physics equations to be solved. Common kernels include:

- **Thermal kernels**: Heat conduction and transport
- **Mechanical kernels**: Stress and strain calculations
- **Hydraulic kernels**: Pore fluid flow

[API documentation will be auto-generated from source code]

## Materials

Material objects define constitutive relationships:

- **Elastic materials**: Linear elasticity
- **Plastic materials**: Plasticity models
- **Custom materials**: User-defined behaviors

[API documentation will be auto-generated from source code]

## Boundary Conditions

Applied loading and constraints:

- **Dirichlet BC**: Fixed displacement/pressure/temperature
- **Neumann BC**: Applied force/flux
- **Mixed BC**: Combinations of Dirichlet and Neumann

[API documentation will be auto-generated from source code]

---

**For detailed API documentation**, see the source code in `src/` and `include/` directories, or generate it using:

```bash
doxygen
```
