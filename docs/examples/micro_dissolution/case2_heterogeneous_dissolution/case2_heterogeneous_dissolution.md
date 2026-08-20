# Modelling a heterogeneous shape evolution of a solid with a Phase-Field approach

## Problem description

Even if most of the materials appear homogeneous, they are heterogeneous at smaller scales.
For geomaterials, this smaller scale is at the level of the microstructure and it consists of grains (that can differ in minerals) and pores.
This microstructure is fundamental as the organization and the elements that compose it determine the multiphysical properties at the scale of the material.
Furthermore, this microstructure is affected by external solicitations and evolves with time.
Then, it becomes essential to predict the microstructure evolution.

Among the methods available in the literature, the phase-field approach emerges as an efficient and accurate tool for this prediction of the microstructure evolution due to multiphysical solicitations.

In particular, the microstructure can be described by a single phase variable $\eta$ ($=1$ in the solid phase, $=0$ in the pore phase), see the illustration in [Figure 1](#fig-configuration).

<a id="fig-configuration"></a>

![Definition of the configuration and description of the microstructure with a phase variable](fig_configuration.png)

***Figure 1:** Definition of the configuration and description of the microstructure with a phase variable.*

The Allen-Cahn equation, available in Eq. 1, is applied to solve the evolution of the microstructure.

\[
    \frac{\partial\eta}{\partial t}=-L\,\frac{\partial\left(f_{loc}+E_d\right)}{\partial \eta} + L\cdot\kappa\,\nabla^2\eta
    \label{Equation Allen Cahn}
    \tag{1}
\]

In particular, this equation involves a free energy function $f_{loc}$ that describes the material. 
This free energy function is destabilized by a tilting energy function $E_d$, inducing the evolution of the microstructure.
As depicted in [Figure 2](#fig_floc_ed) and formulated in Eq. 2,

\[
    f_{loc}+E_d(c) = W\times\left(\eta^2(1-\eta)^2\right) + ed(c)\times\left( 3\eta^2-2\eta^3\right)
    \label{Equation free energy}
    \tag{2}
\]

where $W$ is the barrier energy, preventing the phase transition, and $ed$ the tilting amplitude, inducing the phase transition. 
Compared to the [homogeneous dissolution](../case1_homogeneous_dissolution/case1_homogeneous_dissolution.md), this configuration considers a heterogeneous dissolution.
In particular, the amplitude of the tilting is dictated by the value of a new variable $c$ that described the concentration of a reactive speciment in the pore fluid.
The additional parameters $L$ and $\kappa$ from Eq. 1 affect the interface width and the kinetics of the phenomenon.

<a id="fig_floc_ed"></a>

![Destabilization of the free energy $f_{loc}$ by the tilting energy $E_d$](fig_floc_ed.png){ width="60%" }

***Figure 2:** Destabilization of the free energy $f_{loc}$ by the tilting energy $E_d$.*

In the context of this model, the propagation of this solute is described by Eq. 3,

\[
    \frac{\partial c}{\partial t} = -\alpha_\eta \frac{\partial \eta}{\partial t} + \kappa_c \nabla^2 c
    \label{Equation Diffusion c}
    \tag{3}
\]

the terms $\frac{\partial c}{\partial t} = \kappa_c \nabla^2 c$ represent the diffusive propagation of the solute, and the terms $\alpha_\eta \frac{\partial \eta}{\partial t}$ ensure the conservation of the mass during the dissolution/precipitation of the solid.

Subsequently, by assuming the chemical reaction $\eta_s \rightleftharpoons c_l$, the chemical quotient of the reaction is $Q=\frac{\{c\}}{\{\eta\}}=c$ and the equilibrium constant is $K=c_{eq}$.
Comparing $Q$ with $K$, the dissolution occurs for $Q<K$ ($c<c_{eq}$) and the precipitation occurs for $Q>K$ ($c>c_{eq}$).
In the same note, the kinetics of the reaction depends on the distance to the equilibrium, see Eq. 4.

\[
    E_d = \chi\,(c_{eq}-c)\times \left(3\eta^2-2\eta^3\right)
    \label{Equation Ed}
    \tag{4}
\]
where $\chi$ is a constant relative to the global kinetics of the reaction.

---

## Model set up

As depicted in [Figure 1](#fig-configuration), the model simulates a heterogeneous grain dissolution due to the presence of a reactive specie. 
The *Phase-Field* module of MOOSE should be activated during the installation of REDBACK to solve this problem.

### Mesh

The first block called is the Mesh block.
For this example, the domain is a 2D square with a size of 1. 
Then, the domain is meshed with a regular grid, considering 100 elements in the X and Y directions.

In this example, the mesh is created by using a mesh generator from MOOSE. 
The quality of the simulation can be increased with an use of a more sophisticated mesh, involving refinement of the mesh close to the interface of the grain.
Such a mesh can be generated with external softwares.

In this example, the mesh is modified for output reasons: two blocks are generated.

```text
[Mesh]
    [mesh]
        type = GeneratedMesh
        dim = 2
        nx = 100
        ny = 100
        nz = 0
        xmin = 0
        xmax = 1
        ymin = 0
        ymax = 1
        elem_type = QUAD4
    []
    [left]
        type = SubdomainBoundingBoxGenerator
        input = mesh
        block_id = 0
        bottom_left = '0 0 0'
        top_right = '0.5 1 0'
    []
    [right]
        type = SubdomainBoundingBoxGenerator
        input = left
        block_id = 1
        bottom_left = '0.5 0 0'
        top_right = '1 1 0'
    []
[]
```

### Phase-field variable and initial condition

Once the mesh is generated, the variable $\eta$ is generated in the domain. This variable is equal to 1 to represent the solid phase and is equal to 0 to represent the pore phase. 
The interface between the solid and the pore phases is represented by $0<\eta<1$.

In this example, the initial condition of the variable (a circle) is generated by a MOOSE function. 
More complex configurations can be investigated by employing the MOOSE functions that manage the initialization from .txt or .png files.

An additional variable $c$ is generated in the domain to represent the concentration of the reactive specimen. Its initial condition considers that the value is null.

```text
[Variables]
    [./eta]
        order = FIRST
        family = LAGRANGE
        outputs = exodus
        [./InitialCondition]
            type = SmoothCircleIC
            x1 = 0.5
            y1 = 0.5
            z1 = 0
            radius = 0.3
            invalue = 1
            outvalue = 0
            int_width = 0.06
        [../]
    [../]
    [./c]
        outputs = exodus
        [./InitialCondition]
            type = ConstantIC
            value = 0
        [../]
    [../]
[]
```

### Boundary conditions

Even if the concentration of the reactive specimen is null at the initial condition (see block `Variables`), it increases through the boundary conditions defined in the block `BCs`.
In particular, two conditions are available for the variable $c$:

* `NeumannBC`, imposing a flux on the reactive specimen through a boundary
* `DirichletBC`, imposing a constant value for the reactive specimen at a boundary
The selection of the condition is made with the input `active`. 
If the name of a subblock is not listed in this input (`left_Dirichlet` in the following example), this subblock is canceled.

```text
[BCs]
    active = 'left_Neumann'
    [left_Neumann]
        type = NeumannBC
        boundary = left
        variable = c
        value = -1
    []
    [left_Dirichlet]
        type = DirichletBC
        boundary = left
        variable = c
        value = -20
    []
[]
```

### Kernels

Once the variable is defined, the system of partial derivative equations can be specified.
In this case, the system is divided into two equations: Eq. 1 applied on the variable $\eta$, Eq. 3 applied on the variable $c$. 


As described in Eq. 1, an Allen-Cahn equation, applied to the variable `eta`, is solved to predict the dissolution of the solid phase. 
This equation can be divided into three kernels:

* `TimeDerivative`: $\frac{\partial\eta}{\partial t}$
* `AllenCahn`: $-L\frac{\partial(f_{loc}+E_d)}{\partial\eta}$
* `ACInterface`: $L\kappa\nabla^2\eta$

In the same note, the diffusive equation applied to the variable `c`, described in Eq. 3, is solved to predict the propagation of the concentration of the reactive specimen. 
This equation can be divided into three kernels:

* `TimeDerivative`: $\frac{\partial c}{\partial t}$
* `CoefCoupledTimeDerivative`: $\alpha_\eta\;\frac{\partial\eta}{\partial t}$. It is worth noting that this kernel uses the variable `eta` with the input `v` even if it is applied to the equation applied to the variable `c`
* `ACInterface`: $L_c\cdot \kappa_c \; \nabla^2 c$

It is essential to specify that the kernels are applied to the variables `eta` and `c`, which are defined in the `Variables` block.
In the same note, additional parameters are introduced `L_eta`, `g_eta`, `kappa_eta`, `kappa_c`, and `L_c`, defined in the block `Materials` (see below).

```text
[Kernels]
    # kernels on the phase variable eta
    [./detadt]
        type = TimeDerivative
        variable = eta
        block = '0 1'
    [../]
    [./ACBulk_eta]
        type = AllenCahn
        variable = eta
        coupled_variables = 'c' # add a dependence on the variable c
        mob_name = L_eta
        f_name = g_eta
        block = '0 1'
    [../]
    [./ACInterface_eta]
        type = ACInterface
        variable = eta
        mob_name = L_eta
        kappa_name = kappa_eta
        block = '0 1'
    [../]
    # kernels on the diffusive reactive specie c
    [./dcdt]
        type = TimeDerivative
        variable = c
        block = '0 1'
    [../]
    [./eta_c]
        type = CoefCoupledTimeDerivative
        variable = c
        v = 'eta'
        coef = 1
        block = '0 1'
    [../]
    [./c_diffusion]
        type = ACInterface
        variable = c
        kappa_name = kappa_c
        mob_name = L_c
        block = '0 1'
    [../]
[]
```

### Materials

As introduced in the preceding `Kernels` block, the material properties must be assigned in the domain. Here, two types of properties are introduced: the homogeneous properties and the heterogeneous properties.
The function `GenericConstantMaterial` assigns a constant value to a property in the entire domain, while the functions `ParsedMaterial` and `DerivativeParsedMaterial` generate a heterogeneous field for the property.

Concerning these functions `ParsedMaterial` and `DerivativeParsedMaterial`, they are divided into several main inputs:

- `block`: the domain of application of this function, depending on the mesh generated in the block `Mesh`
- `property_name`: the name of the property, used for instance in the block `Kernels`
- `expression`: the expression of the free energy $f_{loc}+E_d$ of the material
- `coupled_variables`: the definition of the variables used in the `expression` of the free energy. These variables can be a material or a variable of the problem
- `constant_names`: the definition of the constants used in the `expression` of the free energy. The values of these constants are specified with `constant_expressions`
- `derivative_order`: (only for `DerivativeParsedMaterial`) the derivatives of this function (until this order) are automatically computed

It is worth noting that the diffusion coefficient depends on the variable $\eta$ to prevent the propagation of the reactive specimen $c$ in the solid phase.

```text
[Materials]
    [./consts]
        type = GenericConstantMaterial
        prop_names  = 'L_eta kappa_eta L_c'
        prop_values = '1 0.00037 1'
        block = '0 1'
    [../]
    [./var]
        type = ParsedMaterial
        property_name = kappa_c
        coupled_variables = 'eta'
        constant_names = 'k_c_0'
        constant_expressions = '0.001'
        expression = 'k_c_0*(1-eta)'
        block = '0 1'
    [../]
    [./energy_eta]
        type = DerivativeParsedMaterial
        property_name = g_eta
        coupled_variables = 'eta c'
        constant_names = 'W x_c'
        constant_expressions = '1 0.1'
        expression = 'W*(eta^2)*((1-eta)^2) - x_c*c*(3*eta^2-2*eta^3)'
        enable_jit = true
        derivative_order = 1
        block = '0 1'
    [../]
[]
```

### Solver preconditioning and executioner

In order to prepare for the resolution of the problem, the block `Preconditioning` is called.

```text
[Preconditioning]
    [./SMP]
        type = SMP
        full = true
    [../]
[]
```

Then, the resolution of the problem is conducted with the block `Executioner`.
Herein, a `Transient` executioner is selected to determine the evolution with time of the solid phase.
In particular, the simulated time is determined by the stopping conditions (here `num_steps`), and the quality of the results depends on the tolerance applied to the residual (here `l_tol`, `l_abs_tol`, `nl_rel_tol`, `nl_abs_tol`).
Furthermore, an adaptive time step is employed with `SolutionTimeAdaptiveDT` to optimize the resolution of the problem.

```text
[Executioner]
    type = Transient
    scheme = 'bdf2'
    solve_type = 'NEWTON'
    l_max_its = 20
    l_tol = 1e-6
    l_abs_tol = 1e-6
    nl_max_its = 10
    nl_rel_tol = 1e-6
    nl_abs_tol = 1e-6
    start_time = 0.0
    num_steps = 50
    [./TimeStepper]
        type = SolutionTimeAdaptiveDT
        dt = 0.5
    [../]
[]
```

### Postprocessors

To help the visualization of the advancement of the simulation, the `Postprocessors` block is called. Here, a `ElementAverageValue` postprocessor is used to estimate the mean value of the variable `eta`, representing the quantity of solid in the domain.
In the same note, the mean value of this variable can be estimated in the two distinct subdomains defined in the block `Mesh`.

```text
[Postprocessors]
    [eta_pp]
        type = ElementAverageValue
        variable = eta
        block = '0 1'
    []
    [eta_left_pp]
        type = ElementAverageValue
        variable = eta
        block = '0'
    []
    [eta_right_pp]
        type = ElementAverageValue
        variable = eta
        block = '1'
    []
[]
```

### Outputs

Finally, the results of this simulation are managed with the block `Outputs`.
Different types of output are generated at the initial condition and at the each time step (see the command `execute_on`) :

- a .e file with the command `exodus` to obtain the field of the variable `eta`. This field can also be obtained with a `VTK` output to generate .vtk files which can be read by python scripts
- a `Console` output for the user to follow the simulation. This output is just for visualization and it is not saved
- a `CSV` output to generate a .csv file to obtain the evolution of a `Postprocessors` with time

```text
[Outputs]
    execute_on = 'initial timestep_end'
    exodus = true
    [console]
        type = Console
        execute_on = 'nonlinear'
        all_variable_norms = true
        max_rows = 5
    []
    [csv]
        type = CSV
        show = 'eta_pp eta_left_pp eta_right_pp'
        execute_on = 'timestep_end'
    []
```

## Results

The first result is the movie showing the dissolution of the solid phase and the propagation of the reactive specimen.

<video controls width="700" src="Case2_Dissolution.mp4"></video>

Then, the `CSV` output generates the time evolution of the mean value of the variable `eta`  in different subdomains of the mesh, depicted in [Figure 3](#fig-time-eta) and available from the generated .csv file.
As expected, the grain dissolves with time, and this dissolution is accelerated in the left part of the domain due to a larger concentration of the reactive specimen.

<a id="fig-time-eta"></a>

![Time evolution of the mean variable eta](fig_time_mEta.png){ width="80%" }

***Figure 3.** Time evolution of the mean variable eta.*

# References

1. N. Moelans, B. Blanpain, P. Wollants (2008), "An introduction to phase-field modeling of microstructure evolution", Computer Coupling of Phase Diagrams and Thermochemistry 32:268–294, DOI: 10.1016/j.calphad.2007.11.003
2. T. Takaki (2014), "Phase-field Modeling and Simulations of Dendrite Growth", ISIJ International 54:437–444, DOI: 10.2355/isijinternational.54.437
3. A. Sac-Morane, K. Ioannidou, M. Veveakis, H. Rattez (2026), "Predicting microstructure and mechanical properties during alite (C3S) hydration using a Phase-Field model", Construction and Building Materials 521:145946, DOI: 10.1016/j.conbuildmat.2026.145946