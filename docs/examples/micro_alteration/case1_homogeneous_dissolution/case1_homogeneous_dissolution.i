# this code presents the dissolution of a phase due to the tilting of the free energy
# the "Phase Field" module of MOOSE should be used for the resolution
# last update: 06/19/2026
# authors: Alexandre Sac-Morane, alexandre.sac-morane@enpc.fr

# Domain and mesh descriptions
# the domain is a 2D square with a size of 1 
# the domain is meshed with a regular grid, considering 100 elements in the X and Y directions.
[Mesh]
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

# Definition of the variables
# the variable eta is equal to 1 to represent the solid phase and is equal to 0 to represent the pore phase
# the interface between the solid and the pore phases is represented by 0<eta<1
[Variables]
  # phase variable
  [./eta]
    order = FIRST
    family = LAGRANGE
    outputs = exodus # it is saved in the .e output file
    [./InitialCondition]
      # genere a centerized sphere as initial condition
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
[]

# definition of the problem
# an Allen-Cahn equation is solved to predict the dissolution of the solid phase
[Kernels]
  # time derivative
  [./detadt]
    type = TimeDerivative
    variable = eta
  [../]
  # free energy and tilting energy
  [./ACBulk_eta]
    type = AllenCahn
    variable = eta
    mob_name = L_eta
    f_name = g_eta
  [../]
  # diffusive term
  [./ACInterface_eta]
    type = ACInterface
    variable = eta
    mob_name = L_eta
    kappa_name = kappa_eta
  [../]
[]

# Material description
[Materials]
  # constants of the problem
  [./consts]
    # L_eta can be changed to modify the reaction kinetics
    # The diffusion coefficient (kappa_eta) induce also a dissolution of the phase.
    # the mechanisms due to the tilting energy should be larger than the mechanisms induced by the diffusion
    type = GenericConstantMaterial
    prop_names  = 'L_eta kappa_eta'
    prop_values = '1 0.00037'
  [../]
  # energy of the phase variable
  [./energy_eta]
    type = DerivativeParsedMaterial
    block = 0
    property_name = g_eta
    coupled_variables = 'eta' 
    constant_names = 'W ed'
    # W is the barrier energy (preventing dissolution) 
    # ed is the tilting amplitude (inducing dissolution)
    constant_expressions = '1 0.1' # constants can be changed
    # the energy is the free energy of the phase + the tilting energy
    expression = 'W*(eta^2)*((1-eta)^2) + ed*(3*eta^2-2*eta^3)'
    enable_jit = true
    derivative_order = 1 # automatic derivation from the expression of the function
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
  num_steps = 50
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
    show = 'eta_pp'
    execute_on = 'timestep_end'
  []
[]
