# this code presents the dissolution of a phase due to the diffusion of a reactive specie
# the "Phase Field" module of MOOSE should be used for the resolution
# last update: 06/19/2026
# authors: Alexandre Sac-Morane, alexandre.sac-morane@enpc.fr

# Domain and mesh descriptions
# the domain is a 2D square with a size of 1 
# the domain is meshed with a regular grid, considering 100 elements in the X and Y directions.
[Mesh]
  [mesh]
    type = GeneratedMeshGenerator
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
  # the mesh is then modified for output reasons
  # the domain is divided into two parts 
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

# Definition of the variables
[Variables]
  # phase variable
  # the variable eta is equal to 1 to represent the solid phase and is equal to 0 to represent the pore phase
  # the interface between the solid and the pore phases is represented by 0<eta<1
  [./eta]
    order = FIRST
    family = LAGRANGE
    outputs = exodus
    [./InitialCondition]
      # genere a centerized sphere
      type = SmoothCircleIC
      x1 = 0.5
      y1 = 0.5
      z1 = 0
      radius = 0.3
      invalue = 1
      outvalue = 0
      int_width = 0.06 #approximatively the interface of the phase variable
    [../]
  [../]
  # reactive specimen
  # the variable c represents the concentration of the specimen
  [./c]
    outputs = exodus
    [./InitialCondition]
      # no reactive specimen at t=0 
      type = ConstantIC
      value = 0
    [../]
  [../]
[]

# Two boundary conditions are available for the variable c
# NeumannBC, imposing a flux on the reactive specimen through a boundary
# DirichletBC, imposing a constant value for the reactive specimen at a boundary
# the input active allows to select one of the two
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

# definition of the problem
# an Allen-Cahn equation is solved to predict the dissolution of the solid phase
# a diffusive conservative equation is solved to predict the propagation of the solute concentration
[Kernels]
  # kernels on the phase variable eta
  # time derivative
  [./detadt]
    type = TimeDerivative
    variable = eta
    block = '0 1'
  [../]
  # free energy and tilting energy
  [./ACBulk_eta]
    type = AllenCahn
    variable = eta
    coupled_variables = 'c' # dependence on the variable c
    mob_name = L_eta
    f_name = g_eta
    block = '0 1'
  [../]
  # diffusive term
  [./ACInterface_eta]
    type = ACInterface
    variable = eta
    mob_name = L_eta
    kappa_name = kappa_eta
    block = '0 1'
  [../]

  # kernels on the diffusive reactive specie c
  # time derivative
  [./dcdt]
    type = TimeDerivative
    variable = c
    block = '0 1'
  [../]
  # conversion term 
  [./eta_c]
    type = CoefCoupledTimeDerivative
    variable = c
    v = 'eta' # even if the kernel is used in the equation applied on c, it uses the variable eta (coupling between the variables)
    coef = 1 # arbitrary value, can be changed
    block = '0 1'
  [../]
  # diffusive term
  [./c_diffusion]
    type = ACInterface
    variable = c
    kappa_name = kappa_c
    mob_name = L_c
    block = '0 1'
  [../]
[]

# Material description
[Materials]
  # constants of the problem
  [./consts]
    # L_eta can be changed to modify the reaction kinetics
    # the mechanisms due to the tilting energy should be larger than the mechanisms induced by the diffusion
    type = GenericConstantMaterial
    prop_names  = 'L_eta kappa_eta L_c'
    prop_values = '1 0.00037 1'
    block = '0 1'
  [../]
  # the diffusion coefficient of the variable c depends on eta
  # c can not diffuse into eta
  [./var]
    type = ParsedMaterial
    property_name = kappa_c
    coupled_variables = 'eta'
    constant_names = 'k_c_0'
    constant_expressions = '0.001'
    expression = 'k_c_0*(1-eta)'
    block = '0 1'
  [../]
  # energy of the phase variable
  [./energy_eta]
    type = DerivativeParsedMaterial
    property_name = g_eta
    coupled_variables = 'eta c'
    constant_names = 'W x_c'
    # W is the barrier energy (preventing dissolution) 
    # x_c*c is the tilting amplitude (inducing dissolution)
    constant_expressions = '1 0.1' # constants can be changed
    # the energy is the free energy of the phase + the tilting energy
    expression = 'W*(eta^2)*((1-eta)^2) - x_c*c*(3*eta^2-2*eta^3)'
    enable_jit = true
    derivative_order = 1 # automatic derivation from the expression of the function
    block = '0 1'
  [../]
[]

[Preconditioning]
  # This preconditioner makes sure the Jacobian Matrix is fully populated. Our
  # kernels compute all Jacobian matrix entries.
  # This allows us to use the Newton solver below.
  [./SMP]
    type = SMP
    full = true
  [../]
[]

# define the resolution scheme
# a Transient executioner is selected to determine the evolution with time of the solid phase. 
[Executioner]
  type = Transient
  scheme = 'bdf2'
  # Automatic differentiation provides a _full_ Jacobian in this example
  # so we can safely use NEWTON for a fast solve
  solve_type = 'NEWTON'
  # criteria for the convergence detection
  l_max_its = 20
  l_tol = 1e-6 
  l_abs_tol = 1e-6
  nl_max_its = 10
  nl_rel_tol = 1e-6
  nl_abs_tol = 1e-6
  # time domain of the simulation
  start_time = 0.0
  num_steps = 100
  # use an adaptative time increment
  [./TimeStepper]
    type = SolutionTimeAdaptiveDT
    dt = 0.5
  [../]
[]

# Definition of the postprocessor
# used for the outputs
[Postprocessors]
  [eta_pp]
    type = ElementAverageValue # compute the mean value of eta in the domain
    variable = eta
    block = '0 1'
  []
  [eta_left_pp]
    type = ElementAverageValue # compute the mean value of eta in the left part of the domain
    variable = eta
    block = '0'
  []
  [eta_right_pp]
    type = ElementAverageValue # compute the mean value of eta in the right part of the domain
    variable = eta
    block = '1'
  []
[]

# Definition of the outputs
[Outputs]
  execute_on = 'initial timestep_end'
  exodus = true # general output readable in Paraview
  [console] # user output to follow the simulation
    type = Console
    execute_on = 'nonlinear'
    all_variable_norms = true
    max_rows = 5
  []
  [csv] # csv output to save the evolution of the postprocessors with time
    type = CSV
    show = 'eta_pp eta_left_pp eta_right_pp'
    execute_on = 'timestep_end'
  []
[]
