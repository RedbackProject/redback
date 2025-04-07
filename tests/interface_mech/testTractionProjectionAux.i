# Test of AuxKernel TractionProjectionAux.C against analytical solution
# For a stress tensor sigma = / 0  0    \
#                             | 0  1.154/
# and surface angle of 10 degrees (theta = 10)
# The normal (sigma) and shear (tau) stresses are then
#   sigma = 1.154*cos(theta)*cos(theta)
#   tau = 1.154*sin(theta)*cos(theta)
[Mesh]
  type = GeneratedMesh
  uniform_refine = 0
  dim = 2
  nx = 4
  ny = 4
[]

[Variables]
  [disp_x]
  []
  [disp_y]
  []
[]

[AuxVariables]
  [stress_tangential]
    family = MONOMIAL
    order = CONSTANT
  []
  [stress_normal]
    family = MONOMIAL
    order = CONSTANT
  []
  [stress_xx]
    family = MONOMIAL
    order = CONSTANT
  []
  [stress_yy]
    family = MONOMIAL
    order = CONSTANT
  []
  [stress_xy]
    family = MONOMIAL
    order = CONSTANT
  []
[]

[AuxKernels]
  [stress_tangential]
    type = TractionProjectionAux
    stress_tensor = stress
    variable = stress_tangential
    surface_angle = 10
    convert_to_radians = true
    normal = false
  []
  [stress_normal]
    type = TractionProjectionAux
    stress_tensor = stress
    variable = stress_normal
    surface_angle = 10
    convert_to_radians = true
    normal = true
  []
  [stress_yy]
    type = RankTwoAux
    rank_two_tensor = stress
    index_j = 1
    index_i = 1
    variable = stress_yy
  []
[]

[Kernels]
  [disp_x]
    type = StressDivergenceTensors
    component = 0
    variable = disp_x
    displacements = 'disp_x disp_y'
  []
  [disp_y]
    type = StressDivergenceTensors
    component = 1
    variable = disp_y
    displacements = 'disp_x disp_y'
  []
[]

[Materials]
  [Elasticity_tensor]
    type = ComputeIsotropicElasticityTensor
    poissons_ratio = 0.3
    youngs_modulus = 10000
  []
  [mc]
    type = ComputeMultiPlasticityStress
    ep_plastic_tolerance = 1E-9
    plastic_models = 'j2'
  []
  [finite_strain0]
    type = ComputePlaneFiniteStrain
    displacements = 'disp_x disp_y'
  []
[]

[UserObjects]
  [str]
    type = TensorMechanicsHardeningConstant
    value = 1
  []
  [j2]
    type = TensorMechanicsPlasticJ2
    yield_strength = str
    yield_function_tolerance = 1E-9
    internal_constraint_tolerance = 1E-9
  []
[]

[Functions]
  [loading_vel]
    type = ParsedFunction
    value = '0.0002*t'
  []
[]

[BCs]
  [uy_top]
    type = FunctionDirichletBC
    variable = disp_y
    boundary = 'top'
    function = loading_vel
  []
  [no_disp_x]
    type = DirichletBC
    variable = disp_x
    boundary = 'left'
    value = 0.0
  []
  [no_disp_y]
    type = DirichletBC
    variable = disp_y
    boundary = 'bottom'
    value = 0.0
  []
[]

[Preconditioning]
  [SMP]
    type = SMP
    full = true
  []
[]

[Executioner]
  type = Transient
  dt = 1
  l_max_its = 50
  nl_max_its = 10
  solve_type = NEWTON
  petsc_options_iname = '-pc_type -pc_hypre_type -snes_linesearch_type -ksp_gmres_restart'
  petsc_options_value = 'hypre boomeramg cp 201'
  nl_abs_tol = 1e-5
  nl_rel_tol = 1e-5
  reset_dt = true
  line_search = basic
  start_time = 0.0
  end_time = 2
[]

[Outputs]
  exodus = true
  file_base = testTractionProjectionAux
[]
