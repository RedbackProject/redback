# REDBACK Documentation

REDBACK was developed to model multi-physics Rock mEchanics with Dissipative feedBACKs in a tightly coupled manner. It is an application leveraging [MOOSE](https://mooseframework.inl.gov/), which proposes a powerful and flexible platform to solve multi-physics problems implicitly and in a tightly coupled manner on unstructured meshes. MOOSE aims at providing a wide range of modules to model various physical phenomena which are as flexible as possible and can be easily coupled together. REDBACK extends MOOSE non-dimensional formulation geomechanics and provides examples for common and more complex geomechanics problems. 

## Key Features

- **MOOSE Integration**: Built on the MOOSE framework and benefit from all features. Can be used with a selected list of MOOSE modules.
- **Rock Mechanics**: Specialized for geomechanics applications, see list of publications and examples
- **Coupled Multiphysics**: Thermo-hydro-mechanical-chemical (THMC) simulations
- **Flexible**: Extensible for custom physics and materials


- perzyna viscoplasiticty?
- regularised localisation
- return map?
- displaced boundary method for smoothing interfaces
- lower dimensional interfaces for flow in DFN
- chemistry
- shear heating
- cosserat
- Digital Rock Physics framework
- Many geomechanics applications in list of publications and examples

## Gallery


## How to cite

Poulet, T., Paesold, M., & Veveakis, M. (2017). Multi-physics modelling of fault mechanics using REDBACK: a parallel open-source simulator for tightly coupled problems. Rock Mechanics and Rock Engineering, 50(3), 733-749. [https://doi.org/10.1007/s00603-016-0927-y](https://doi.org/10.1007/s00603-016-0927-y)

## List of publications

### Code development

- Poulet, T., & Veveakis, E. (2016). A viscoplastic approach for pore collapse in saturated soft rocks using REDBACK: an open-source parallel simulator for Rock mEchanics with Dissipative feedBACKs, Computers and Geotechnics, 74, 211-221, [https://doi.org/10.1016/j.compgeo.2015.12.015](https://doi.org/10.1016/j.compgeo.2015.12.015)

- Lesueur, M., Casadiego, M. C., Veveakis, M., & Poulet, T. (2017). Modelling fluid-microstructure interaction on elasto-visco-plastic digital rocks. Geomechanics for Energy and the Environment, 12, 1-13. [https://doi.org/10.1016/j.gete.2017.08.001](https://doi.org/10.1016/j.gete.2017.08.001)

- Lesueur, M., Poulet, T., & Veveakis, M. (2020). Three-scale multiphysics finite element framework (FE3) modelling fault reactivation. Computer Methods in Applied Mechanics and Engineering, 365, 112988. [https://doi.org/10.1016/j.cma.2020.112988](https://doi.org/10.1016/j.cma.2020.112988)

- Poulet, T., Lesueur, M., & Kelka, U. (2021). Dynamic modelling of overprinted low-permeability fault cores and surrounding damage zones as lower dimensional interfaces for multiphysics simulations. Computers & Geosciences, 150, 104719. [https://doi.org/10.1016/j.cageo.2021.104719](https://doi.org/10.1016/j.cageo.2021.104719)

- Lesueur, M., Rattez, H., & Colomés, O. (2022). μCT scans permeability computation with an unfitted boundary method to improve coarsening accuracy. Computers & Geosciences, 166, 105118. [https://doi.org/10.1016/j.cageo.2022.105118](https://doi.org/10.1016/j.cageo.2022.105118)

- Zwarts, S., & Lesueur, M. (2024). Homogenisation method based on energy conservation and independent of boundary conditions. Advances in Water Resources, 183, 104603. [https://doi.org/10.1016/j.advwatres.2023.104603](https://doi.org/10.1016/j.advwatres.2023.104603)

- Zwarts, S., & Lesueur, M. (2024). Predicting the Representative Elementary Volume by determining the evolution law of the convergence cone. Geomechanics for Energy and the Environment, 40, 100594. [https://doi.org/10.1016/j.gete.2024.100594](https://doi.org/10.1016/j.gete.2024.100594)


### Applications

- Peters, M., Herwegh, M., Paesold, M. K., Poulet, T., Regenauer-Lieb, K., & Veveakis, E. (2016). Boudinage and folding as the same energy attractor of ductile deformation, J. Geophys. Res., [https://doi.org/10.1002/2016JB012801](https://doi.org/10.1002/2016JB012801)

- Paesold, M., Bassom, A., Regenauer-Lieb, K., & Veveakis, E. (2016). Conditions for the localization of plastic deformation in temperature sensitive visco-plastic materials, J. Mech. Mat. Structures, 11(2):113–136, [https://doi.org/10.2140/jomms.2016.11.113](https://doi.org/10.2140/jomms.2016.11.113)

- Lesueur, M., Poulet, T., & Veveakis, M. (2020). Permeability hysteresis from microchannels opening during dissolution/reprecipitation cycle. Geophysical Research Letters, 47(18), e2020GL088674. [https://doi.org/10.1029/2020GL088674](https://doi.org/10.1029/2020GL088674)

- Guével, A., Rattez, H., & Veveakis, E. (2020). Viscous phase-field modeling for chemo-mechanical microstructural evolution: application to geomaterials and pressure solution. International Journal of Solids and Structures, 207, 230-249. [https://doi.org/10.1016/j.ijsolstr.2020.09.026](https://doi.org/10.1016/j.ijsolstr.2020.09.026) 

- Lesueur, M., Poulet, T., & Veveakis, M. (2021). Predicting the yield strength of a 3D printed porous material from its internal geometry. Additive Manufacturing, 44, 102061. [https://doi.org/10.1016/j.addma.2021.102061](https://doi.org/10.1016/j.addma.2021.102061)

- Lesueur, M., Veveakis, M., & Rattez, H. (2022). Influence of cementation on the yield surface of rocks numerically determined from digital microstructures. International Journal of Plasticity, 156, 103338. [https://doi.org/10.1016/j.ijplas.2022.103338](https://doi.org/10.1016/j.ijplas.2022.103338)

- Smith, R. Y., Lesueur, M., Kelka, U., Poulet, T., & Koehn, D. (2022). Using fractured outcrops to calculate permeability tensors: implications for geothermal fluid flow and the influence of seismic-scale faults. Geological Magazine, 159(11-12), 2262-2278. [https://doi.org/10.1017/S0016756822000309](https://doi.org/10.1017/S0016756822000309)

- Lesueur, M., Guével, A., & Poulet, T. (2022). Reconciling asymmetry observations in the permeability tensor of digital rocks with symmetry expectations. Advances in Water Resources, 170, 104334. [https://doi.org/10.1016/j.advwatres.2022.104334](https://doi.org/10.1016/j.advwatres.2022.104334)

- Sac-Morane, A., Veveakis, M., & Rattez, H. (2024), A Phase-Field Discrete Element Method to study chemo-mechanical coupling in granular materials. Computer Methods in Applied Mechanics and Engineering, 424, 116900, [https://doi.org/10.1016/j.cma.2024.116900](https://doi.org/10.1016/j.cma.2024.116900)

- Sac-Morane, A., Rattez, H., & Veveakis, M. (2025), Importance of the precipitation on the slowdown of the creep behavior due to pressure-solution. Journal of Engineering Mechanics, 151, 04025025, [https://doi.org/10.1061/jenmdt/emeng-8360](https://doi.org/10.1061/jenmdt/emeng-8360)

- Zwarts, S., Lindqwister, W., & Lesueur, M. (2026). Database of Generated Rock Microstructures and their computed Geometrical and Hydraulic properties. Scientific Data. [https://doi.org/10.1038/s41597-026-07321-0](https://doi.org/10.1038/s41597-026-07321-0)

- Sac-Morane, A., Ioannidou, K., Veveakis, M. & Rattez, H. (2026), Predicting microstructure and mechanical properties during alite (C3S) hydration using a Phase-Field model. Construction and Building Materials, 521, 145946, [https://doi.org/10.1016/j.conbuildmat.2026.145946](https://doi.org/10.1016/j.conbuildmat.2026.145946)

- Lindqwister, W. F., Chaudhury, M., Schyck, S. N., Rocha, I., Masania, K., & Lesueur, M. (2026). Topology Optimization of 3D-Printed Mycelium Hydrogels. Biofabrication. []()


## Getting Help

💬 Check the [MOOSE Discussion forum](https://github.com/idaholab/moose/discussions)