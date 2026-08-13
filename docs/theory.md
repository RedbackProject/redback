# Theory
(reload page if equations do not display)

### Conventions

In this document we assume that stresses are taken positive in tension and pore pressure is positive ($p_f > 0$). Index notation is used. The indices used are $i$, $j$, $k$, and $l$. All other letters or character strings appearing as indices belong to the symbol used and are not subject to the index notation rules.

REDBACK uses a set of equations that are written for a biphasic material, namely a solid and a fluid phase irrespective of the processes involved, e.g. chemical reactions that produce different kinds of solids and fluids.

## System of equations

The system in its final form is

\[
\begin{aligned}
0 &= \pdiff{\sigma^{\prime}_{ij}}{j} - \pdiff{p_f}{i} + b_i, \\
0 &= \pdiff{p_f}{t} + Pe\: \bar{v}_i^{\beta} \pdiff{p_f}{i} -
Pe\:\bar{v}^{\Lambda}_i \pdiff{T}{i} - \pdiff{\left[\frac{1}{Le} \pdiff{
p_f}{i} \right]}{i} - \Lambda_m \pdiff{T}{t} +
\frac{Pe\:\dot{\epsilon_V}}{\bar{\beta}^*} -\frac{1}{Le_{chem}}
\omega_F, \\
0 &= \pdiff{T}{t} + Pe\:\bar{v}^{m}_i\pdiff{T}{i} - \pd_{ii}c^*_{th}\pd_{ii} T - Gr
\: \sigma_{ij}^{pl}\dot{\epsilon}_{ij}^{\,pl} + Da_{endo}\: (1 - s)(1 - \phi)e^{\frac{Ar_F\:\delta T}{1+\delta T}} - Da_{exo}\:\: s  (1 - \phi)\Delta \phi_{chem} e^{\frac{Ar_R\:\delta T}{1+\delta T}}.
\end{aligned}
\]

All dimensionless numbers are defined [below](#dimensionless-numbers) and

\[
\begin{aligned}
\bar{\beta} &= (1-\phi)\beta_s + \phi\beta_f, \\
\bar{\beta}^* &= \bar{\beta}\,\sigma_{ref}, \\
\bar{v}_i^{\beta} &= (1-\phi)\frac{\beta_s}{\bar{\beta}} v_i^s
  + \phi\frac{\beta_f}{\bar{\beta}}v_i^f, \\
\bar{v}^{\Lambda}_i &= (1-\phi)\Lambda_s v_i^s
  + \phi \Lambda_f v_i^f, \\
\bar{v}^{m}_i &= (1-\phi)\frac{\rho_s}{\bar{\rho}} v_i^s
  + \phi\frac{\rho_f}{\bar{\rho}} v_i^f, \\
\omega_F^{*} &= \frac{\omega_F M_{AB}}{\rho_{AB} k_F} (1-\phi)(1-s)
  \exp\left(\frac{Ar_F \delta T}{1 + \delta T}\right), \\
\dot{\epsilon}^{vp}_{ij} &= \dot{\epsilon}_0
  \exp\left(\frac{Ar \delta T}{1 + \delta T}\right)
  \sqrt{
    \left\langle \frac{q - q_Y}{\sigma_{ref}} \right\rangle^{2m}
    +
    \left\langle \frac{p - p_Y}{\sigma_{ref}} \right\rangle^{2m}
  }
  \frac{\partial f}{\partial \sigma_{ij}}.
\end{aligned}
\]

### Rescaling

A particularity of REDBACK is that it works with dimensionless parameters, in line with its purpose of studying system stability. As such, the variables used in the final system of equations are all dimensionless and are defined as:

\[
\begin{aligned}
p^* = \frac{p_f}{\sigma_{ref}} \\
T^* = \frac{T - T_{ref}}{\delta T_{ref}} \\
x^* = \frac{x}{x_{ref}} \\
t^* = \frac{c_{th,ref}}{x_{ref}^2} t \\
V^* = \frac{V}{V_{ref}} \\
\end{aligned}
\]

where $c_{th} = \frac{\alpha}{(\rho C_p)_m}$.

The derivation of these dimensionless variables is detailed in [Derivation](#derivation).

Note that the time can be rescaled a second time for numerical reasons. See the [Time rescaling](#time-rescaling) section for details.

### Dimensionless numbers

The coefficient $\delta$ is defined such that:

$$
T^{\star} = \frac{T - T_{ref}}{\delta T_{ref}}
$$

| Group | Name | Definition | Interpretation |
|------|------|------------|----------------|
| $Gr$ | Gruntfest number | $\frac{\chi\sigma_{ref}}{\alpha \delta T_{ref} (\rho C_p)_m}$ | Ratio of mechanical energy converted into heat over diffusive processes |
| $Da_{endo}$ | Endothermic Damköhler number | $\frac{A_{endo} h_{endo} \rho_{AB} x_{ref}^2}{\delta T_{ref} c_{th,ref}(\rho C_p)_m}$ | Ratio of endothermic reaction rate over diffusion |
| $Da_{exo}$ | Exothermic Damköhler number | $\frac{A_{exo} h_{exo} \rho_{AB} x_{ref}^2}{\delta T_{ref} c_{th,ref}(\rho C_p)_m}$ | Ratio of exothermic reaction rate over diffusion |
| $Ar$ | Arrhenius number | $\frac{\Delta H_{mech}}{R T_{ref}}$ | Ratio of activation enthalpy over thermal energy |
| $Ar_F$ | Forward Arrhenius number | $\frac{\Delta H_{act}^F}{R T_{ref}}$ | Forward reaction activation energy ratio |
| $Ar_R$ | Reverse Arrhenius number | $\frac{\Delta H_{act}^R}{R T_{ref}}$ | Reverse reaction activation energy ratio |
| $Le$ | Lewis number | $\frac{c_{th}}{c_{hy}} = \frac{\mu_f c_{th,ref} \beta_m^*}{\kappa \sigma_{ref}}$ | Thermal vs mass diffusivity ratio |
| $Le_{chem}$ | Chemical Lewis number | $\frac{c_{th}\,\sigma_{ref}\,\beta_m}{x_{ref}^2 A_{endo}}\frac{\rho_{B}}{\rho_{AB}}\frac{M_{AB}}{M_{B}}\left(\frac{\rho_B}{\rho_f} - \frac{\rho_B}{\rho_s}\right)e^{-Ar_F}$ | Ratio of thermal to chemical diffusivity |
| $\bar{\Lambda}_a$ | Thermal pressurisation coefficient | $\frac{\lambda_a}{\beta_m} \frac{\delta T_{ref}}{\sigma_{ref}}$ | Thermal pressurisation coefficient |
| $Pe$ | Péclet number | $\frac{x_{ref} V_{ref}}{c_{th,ref}}$ | Advection vs diffusion ratio |

## Code architecture

### Kernels

#### Momentum balance

- [RedbackStressDivergenceTensor](https://github.com/RedbackProject/redback/blob/master/src/kernels/RedbackStressDivergenceTensors.C):  
  $\pdiff{\sigma^{\prime}_{ij}}{j} - \pdiff{p_f}{i} + b_i$


#### Mass balance

- TimeDerivative:  
  $\pdiff{p_f}{t}$

- [RedbackMassConvection](https://github.com/RedbackProject/redback/blob/master/src/kernels/RedbackMassConvection.C):  
  $Pe\, v_i^p \pdiff{p_f}{i} - Pe\, v_i^T \pdiff{T}{i}$

- [RedbackMassDiffusion](https://github.com/RedbackProject/redback/blob/master/src/kernels/RedbackMassDiffusion.C):  
  $- \pdiff{}{i}\left(\frac{1}{Le}\pdiff{p_f}{i}\right)$

- [RedbackThermalPressurization](https://github.com/RedbackProject/redback/blob/master/src/kernels/RedbackThermalPressurization.C):  
  $- \Lambda \pdiff{T}{t}$

- [RedbackPoromechanics](https://github.com/RedbackProject/redback/blob/master/src/kernels/RedbackPoromechanics.C):  
  $\frac{Pe\,\dot{\epsilon}_V}{\bar{\beta}\sigma_{ref}}$

- [RedbackChemPressure](https://github.com/RedbackProject/redback/blob/master/src/kernels/RedbackChemPressure.C):  
  $- \frac{1}{Le_{chem}}\omega_F$


#### Energy balance

- TimeDerivative:  
  $\pdiff{T}{t}$

- [RedbackThermalConvection](https://github.com/RedbackProject/redback/blob/master/src/kernels/RedbackThermalConvection.C):  
  $Pe\,\bar{v}_i \pdiff{T}{i}$

- [RedbackThermalDiffusion](https://github.com/RedbackProject/redback/blob/master/src/kernels/RedbackThermalDiffusion.C):  
  $- c_{th}^* \partial_{ii}T$

- [RedbackMechDissip](https://github.com/RedbackProject/redback/blob/master/src/kernels/RedbackMechDissip.C):  
  $- Gr\,\sigma_{ij}^{pl}\dot{\epsilon}_{ij}^{pl}$

- [RedbackChemEndo](https://github.com/RedbackProject/redback/blob/master/src/kernels/RedbackChemEndo.C):  
  $+ Da_{endo}\,\omega_F$

- [RedbackChemExo](https://github.com/RedbackProject/redback/blob/master/src/kernels/RedbackChemExo.C):  
  $- Da_{exo}\,\omega_R$

### Time rescaling

The time used in REDBACK is dimensionless and defined in the [Rescaling](#rescaling) section.  
For numerical reasons, it can sometimes be useful to rescale time by introducing $t'$ such that:

$$
t^* = t' \cdot time \_ factor
$$

Using the newly defined time $t'$ is equivalent to multiplying all kernels — except time derivative kernels — in the governing equations by *time_factor*.

This is useful when the initial residual is too low and prevents convergence in MOOSE. In such cases (e.g. convection-dominated simulations), a larger *time_factor* increases the initial residual magnitude, allowing the nonlinear solver to reduce it to a converged tolerance.

The *time_factor* is defined per kernel where needed, but should only be specified globally in the input file:

```
[GlobalParams]
  time_factor = 1.e-3
[]
```

The real physical time $t$ is related to the simulation time $t'$ by:

$$
t = time \_ factor \cdot \frac{x_{ref}^2}{c_{th,ref}} t'
$$

### Porosity

Porosity plays a special role because its evolution depends on mechanical, thermal, and hydraulic processes. As a result, total porosity cannot be handled entirely within a single material class.

Since [RedbackMechMaterial](https://github.com/RedbackProject/redback/blob/master/src/materials/RedbackMechMaterial.C) already derives from [RedbackMaterial](https://github.com/RedbackProject/redback/blob/master/src/materials/RedbackMaterial.C), introducing a reverse dependency would create a circular dependency problem.

There are two main approaches to resolve this:

1. Porosity is treated as an additional solved variable.
2. Porosity is treated as an `AuxVariable`, updated via an `AuxKernel`.

In the first case, porosity evolution is solved fully consistently, but at a higher computational cost. This is the most general and physically rigorous approach, particularly when porosity strongly depends on all coupled processes.

However, in the current implementation of REDBACK, we focus on the regime described in the governing equations section, where porosity is primarily controlled by chemistry (fluid production leading potentially to $\phi \to 1$), while thermal and pore-pressure effects induce only minor variations.
Therefore, we adopt the second approach: porosity is treated as an `AuxVariable` updated by an `AuxKernel`. This provides flexibility to control update frequency and computational cost.

In practice, we need to create *AuxVariables* for the total and mechanical porosity (the chemical porosity is computed as a material property).

```
[AuxVariables]
  [./total_porosity]
    order = FIRST
    family = MONOMIAL
  [../]
  [./mech_porosity]
    order = FIRST
    family = MONOMIAL
  [../]
[]
```

Those AuxVariables are then computed in AuxKernels. The total_porosity needs to be told where to find the mechanical_porosity, but will find the chemical one directly as a material property.

```
[AuxKernels]
  [./total_porosity]
    type = RedbackTotalPorosityAux
    variable = total_porosity
    mechanical_porosity = mech_porosity
  [../]
  [./mech_porosity]
    type = MaterialRealAux
    variable = mech_porosity
    execute_on = timestep_end
    property = mechanical_porosity
  [../]
[]
```

The materials then need to be told where to find the total_porosity:

```
[Materials]
  [./mat_mech]
    type = RedbackMechMaterialJ2
    ...
    total_porosity = total_porosity
  [../]
  [./mat_nomech]
    type = RedbackMaterial
    ...
    total_porosity = total_porosity
  [../]
[]
```

Note that the `mechanical_porosity` is only computed at the end of the timestep (explicit). This effectively neglects mechanical porosity updates within a single step and retains only chemical contributions, which dominate in the regimes of interest.
The user can opt for the fully implicit method, in which case the mechanical_porosity needs to be solved at residual/linear/nonlinear. 

## Derivation

For the detailed derivation, please refer to the document [theory.pdf]()
