# this code presents the following reactions:
# dissolution of a source that generates some solute
# diffusion of the solute
# precipitation of a product from the consumption of the solute
# the "Phase Field" module of MOOSE should be used for the resolution
# last update: 06/25/2026
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
[Variables]
  # phase variables (eta = source or product)
  # the phase variables are equal to 1 if the point corresponds to the given mineral, else it is equal to 0
  # the interface is represented by 0<eta<1
  # the pore space is determined with 1-(source+product)

  [./source]
    order = FIRST
    family = LAGRANGE
    outputs = exodus
    [./InitialCondition]
      type = FunctionIC
      function = 'if(x<0.22, 1, if(x>0.28, 0, 0.5 + 0.5*cos(3.1415*(x-0.25+0.03)/0.06)))' # cosine profile
    [../]
  [../]
  [./product]
    order = FIRST
    family = LAGRANGE
    outputs = exodus
    [./InitialCondition]
      type = FunctionIC
      function = 'if(x>0.78, 1, if(x<0.72, 0, 0.5 + 0.5*cos(3.1415*(0.75-x+0.03)/0.06)))' # cosine profile
    [../]
  [../]

  # reactive specimen
  # the variable c represents the concentration of the dissolved specimen
  [./c]
    outputs = exodus
    [./InitialCondition]
      # no reactive specimen at t=0 
      type = ConstantIC
      value = 0
    [../]
  [../]
[]

# definition of the problem
# Allen-Cahn equations are solved to predict the dissolution/precipitation of the solid phases
# a diffusive conservative equation is solved to predict the propagation of the solute concentration
[Kernels]
  # kernels on the phase variable source
  # time derivative
  [./dsourcedt]
    type = TimeDerivative
    variable = source
  [../]
  # free energy and tilting energy
  [./ACBulk_source]
    type = AllenCahn
    variable = source
    coupled_variables = 'c' # dependence on the variable c
    mob_name = L_eta
    f_name = g_source
  [../]
  # diffusive term
  [./ACInterface_source]
    type = ACInterface
    variable = source
    mob_name = L_eta
    kappa_name = kappa_eta
  [../]
  
  # kernels on the phase variable product
  # time derivative
  [./dproductdt]
    type = TimeDerivative
    variable = product
  [../]
  # free energy and tilting energy
  [./ACBulk_product]
    type = AllenCahn
    variable = product
    coupled_variables = 'c' # dependence on the variable c
    mob_name = L_eta
    f_name = g_product
  [../]
  # diffusive term
  [./ACInterface_product]
    type = ACInterface
    variable = product
    mob_name = L_eta
    kappa_name = kappa_eta
  [../]
  
  # kernels on the diffusive reactive specimen c
  # time derivative
  [./dcdt]
    type = TimeDerivative
    variable = c
  [../]
  # conversion term (source -> c) 
  [./source_c]
    type = CoefCoupledTimeDerivative
    variable = c
    v = 'source' # even if the kernel is used in the equation applied on c, it uses the variable source (coupling between the variables)
    coef = 2
  [../]
  # conversion term (c -> product) 
  [./product_c]
    type = CoefCoupledTimeDerivative
    variable = c
    v = 'product' # even if the kernel is used in the equation applied on c, it uses the variable product (coupling between the variables)
    coef = 1
  [../]
  # diffusive term
  [./c_diffusion]
    type = ACInterface
    variable = c
    kappa_name = kappa_c
    mob_name = L_c
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
  [../]
  # the diffusion coefficient of the variable c depends on etas
  # c can not diffuse into eta
  [./var]
    type = ParsedMaterial
    property_name = kappa_c
    coupled_variables = 'source product'
    constant_names = 'k_c_0'
    constant_expressions = '0.001'
    expression = 'k_c_0*(1-source)*(1-product)'
  [../]
  # energy of the source variable
  [./energy_source]
    type = DerivativeParsedMaterial
    property_name = g_source
    coupled_variables = 'source c'
    constant_names = 'W x_c'
    # W is the barrier energy (preventing dissolution) 
    # x_c*c is the tilting amplitude (inducing dissolution)
    constant_expressions = '1 0.1' # constants can be changed
    # the energy is the free energy of the phase + the tilting energy
    expression = 'W*(source^2)*((1-source)^2) + x_c*(1-c)*(3*source^2-2*source^3)'
    enable_jit = true
    derivative_order = 1 # automatic derivation from the expression of the function
  [../]
  # energy of the product variable
  [./energy_product]
    type = DerivativeParsedMaterial
    property_name = g_product
    coupled_variables = 'product c'
    constant_names = 'W x_c'
    # W is the barrier energy (preventing precipitation) 
    # x_c*c is the tilting amplitude (inducing precipitation)
    constant_expressions = '1 0.3' # constants can be changed
    # the energy is the free energy of the phase + the tilting energy
    expression = 'W*(product^2)*((1-product)^2) - x_c*c*(3*product^2-2*product^3)'
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
  l_max_its = 30
  l_tol = 1e-6
  l_abs_tol = 1e-6
  nl_max_its = 20
  nl_rel_tol = 1e-6
  nl_abs_tol = 1e-6
  # time domain of the simulation
  start_time = 0.0
  num_steps = 200
  # use an adaptative time increment
  [./TimeStepper]
    type = SolutionTimeAdaptiveDT
    dt = 0.5
  [../]
[]

# Definition of the postprocessor
# used for the outputs
[Postprocessors]
  [source_pp]
    type = ElementAverageValue # compute the mean value of source in the domain
    variable = source
  []
  [product_pp]
    type = ElementAverageValue # compute the mean value of product in the domain
    variable = product
  []
  [c_pp]
    type = ElementAverageValue # compute the mean value of c in the domain
    variable = c
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
    show = 'source_pp product_pp c_pp'
    execute_on = 'timestep_end'
  []
[]
