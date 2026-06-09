# Test of AuxKernel TractionProjectionAux.C against analytical solution
# For a stress tensor sigma = / 0  0    \
#                             | 0  1.154/
# and surface angle of 10 degrees (theta = 10)
# The normal (sigma) and shear (tau) stresses are then
#   sigma = 1.154*cos(theta)*cos(theta) = 1.11920264219
#   tau = 1.154*sin(theta)*cos(theta) = 0.19734562269
[GlobalParams]
  displacements = 'disp_x disp_y'
[]

[Mesh]
  type = GeneratedMesh
  dim = 2
  nx = 1
  ny = 1
[]

[Variables]
  [disp_x]
  []
  [disp_y]
  []
[]

[Kernels]
  [disp_x]
    type = StressDivergenceTensors
    component = 0
    variable = disp_x
  []
  [disp_y]
    type = StressDivergenceTensors
    component = 1
    variable = disp_y
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
  []
[]

[UserObjects]
  [str]
    type = SolidMechanicsHardeningConstant
    value = 1
  []
  [j2]
    type = SolidMechanicsPlasticJ2
    yield_strength = str
    yield_function_tolerance = 1E-9
    internal_constraint_tolerance = 1E-9
  []
[]

[Functions]
  [loading_vel]
    type = ParsedFunction
    expression = '0.0002*t'
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

[Postprocessors]
  [stress_tangential]
    type = ElementAverageValue
    variable = stress_tangential
  []
  [stress_normal]
    type = ElementAverageValue
    variable = stress_normal
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
  solve_type = NEWTON
  l_max_its = 50
  nl_max_its = 10

  start_time = 0.0
  end_time = 2
  dt = 1
[]

[Outputs]
  csv = true
  file_base = testTractionProjectionAux
  execute_on = 'FINAL'
[]
