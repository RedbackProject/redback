# Testing that gravity changes the stress (with depth for example) in the stress divergence kernel
# Based on "K0 test" (http://ascelibrary.org/doi/pdf/10.1061/40785%28164%2924)
# Input:
# L = 1
# rho = 1
# g = -9.81
# E = 5e4
# nu = 0.2
#
# expected output at the location of the postprocessor, which is at the center of the bottom cell (95% down the way):
# K0 = E*nu/(E*(1-nu)) = 0.25
# sigma_zz = rho * g * L = -9.81*0.95 = -9.3195
# sigma_xx = sigma_yy = K0 * sigma_zz = -2.329875

[Mesh]
  type = GeneratedMesh
  dim = 3
  nz = 10
  xmin = -0.5
  xmax = 0.5
  ymin = -0.5
  ymax = 0.5
[]

[Variables]
  active = 'disp_z disp_y disp_x'
  [./disp_x]
    order = FIRST
    family = LAGRANGE
  [../]
  [./disp_y]
    order = FIRST
    family = LAGRANGE
  [../]
  [./disp_z]
    order = FIRST
    family = LAGRANGE
  [../]
  [./temp]
  [../]
  [./pore_pressure]
  [../]
[]

[Materials]
  [./mat_mech]
    type = RedbackMechMaterialElastic
    block = 0
    disp_x = disp_x
    disp_y = disp_y
    disp_z = disp_z
    youngs_modulus = 5e4
    poisson_ratio = 0.2
    initial_stress = 'kxx 0 0  0 kxx 0  0 0 kzz'
  [../]
  [./mat_nomech]
    type = RedbackMaterial
    block = 0
    disp_x = disp_x
    disp_y = disp_y
    disp_z = disp_z
    is_mechanics_on = true
    solid_density = 1
    init_from_functions__params = gravity
    init_from_functions__function_names = gravity_ramp
  [../]
[]

[Functions]
  [./gravity_ramp]
    type = ParsedVectorFunction
    value_z = -9.91*tanh(t)
  [../]
  [./kzz]
    type = ParsedFunction
    value = -9.81*z # initial stress that should result from the weight force
  [../]
  [./kxx]
    type = ParsedFunction
    value = -9.81*0.25*z
  [../]
[]

[BCs]
  active = 'confinex confiney basefixed_z'
  [./confinex]
    type = DirichletBC
    variable = disp_x
    boundary = 'left right'
    value = 0
  [../]
  [./confiney]
    type = DirichletBC
    variable = disp_y
    value = 0
    boundary = 'bottom top'
  [../]
  [./top_velocity]
    type = FunctionDirichletBC
    variable = disp_z
    function = 0
    boundary = front
  [../]
  [./basefixed_z]
    type = DirichletBC
    variable = disp_z
    boundary = back
    value = 0
  [../]
  [./basefixed_x]
    type = DirichletBC
    variable = disp_x
    boundary = back
    value = 0
  [../]
  [./basefixed_y]
    type = DirichletBC
    variable = disp_y
    boundary = back
    value = 0
  [../]
[]

[AuxVariables]
  active = 'stress_yy stress_xz stress_xx stress_xy stress_zz stress_yz'
  [./total_porosity]
    order = FIRST
    family = MONOMIAL
  [../]
  [./stress_xx]
    order = CONSTANT
    family = MONOMIAL
  [../]
  [./stress_xy]
    order = CONSTANT
    family = MONOMIAL
  [../]
  [./stress_xz]
    order = CONSTANT
    family = MONOMIAL
  [../]
  [./stress_yy]
    order = CONSTANT
    family = MONOMIAL
  [../]
  [./stress_yz]
    order = CONSTANT
    family = MONOMIAL
  [../]
  [./stress_zz]
    order = CONSTANT
    family = MONOMIAL
  [../]
[]

[Kernels]
  active = ''
  [./ddispx_dt]
    type = TimeDerivative
    variable = disp_x
  [../]
  [./ddispy_dt]
    type = TimeDerivative
    variable = disp_y
  [../]
  [./ddispz_dt]
    type = TimeDerivative
    variable = disp_z
  [../]
[]

[AuxKernels]
  active = 'stress_yy stress_xz stress_xx stress_xy stress_zz stress_yz'
  [./total_porosity]
    type = RedbackTotalPorosityAux
    variable = total_porosity
    mechanical_porosity = mech_porosity
  [../]
  [./stress_xx]
    type = RankTwoAux
    rank_two_tensor = stress
    variable = stress_xx
    index_i = 0
    index_j = 0
  [../]
  [./stress_xy]
    type = RankTwoAux
    rank_two_tensor = stress
    variable = stress_xy
    index_i = 0
    index_j = 1
  [../]
  [./stress_xz]
    type = RankTwoAux
    rank_two_tensor = stress
    variable = stress_xz
    index_i = 0
    index_j = 2
  [../]
  [./stress_yy]
    type = RankTwoAux
    rank_two_tensor = stress
    variable = stress_yy
    index_i = 1
    index_j = 1
  [../]
  [./stress_yz]
    type = RankTwoAux
    rank_two_tensor = stress
    variable = stress_yz
    index_i = 1
    index_j = 2
  [../]
  [./stress_zz]
    type = RankTwoAux
    rank_two_tensor = stress
    variable = stress_zz
    index_i = 2
    index_j = 2
  [../]
[]

[Postprocessors]
  active = 'stress_yy stress_zz stress_xx zdisp'
  [./p0]
    type = PointValue
    point = '0 0 0'
    variable = pore_pressure
  [../]
  [./zdisp]
    type = PointValue
    point = '0 0 0.5'
    variable = disp_z
  [../]
  [./stress_xx]
    type = PointValue
    point = '0 0 0'
    variable = stress_xx
  [../]
  [./stress_yy]
    type = PointValue
    point = '0 0 0'
    variable = stress_yy
  [../]
  [./stress_zz]
    type = PointValue
    point = '0 0 0'
    variable = stress_zz
  [../]
[]

[Preconditioning]
  [./SMP]
    type = SMP
    full = true
    petsc_options_iname = '-ksp_type -pc_type -snes_atol -snes_rtol -snes_max_it'
    petsc_options_value = 'bcgs bjacobi 1E-14 1E-10 10000'
  [../]
[]

[Executioner]
  type = Transient
  solve_type = Newton
  end_time = 10
  nl_abs_tol = 1e-10
  nl_rel_step_tol = 1e-10
  nl_abs_step_tol = 1e-10
[]

[Outputs]
  exodus = true
  execute_on = 'timestep_end initial'
  file_base = initial_stress
  csv = true
[]

[RedbackMechAction]
  [./solid]
    disp_x = disp_x
    disp_y = disp_y
    disp_z = disp_z
  [../]
[]

