[GlobalParams]
  displacements = 'disp_x disp_y disp_z'
[]


[Mesh]
  type = GeneratedMesh
  dim = 3
  nx = 2
  ny = 2
  nz = 2
  xmin = -0.5
  xmax = 0.5
  ymin = -0.5
  ymax = 0.5
  zmin = -0.5
  zmax = 0.5
[]

[Physics/SolidMechanics/QuasiStatic]
  [all]
    strain = FINITE
    incremental = true
    add_variables = true
    generate_output = 'vonmises_stress'
  []
[]

[BCs]
  [left_x]
    type = DirichletBC
    variable = disp_x
    boundary = 'left right'
    value = 0
  []
  [bottom_y]
    type = DirichletBC
    variable = disp_y
    boundary = bottom
    value = 0
  []
  [back_z]
    type = DirichletBC
    variable = disp_z
    boundary = 'back front'
    value = 0
  []
  [top_y]
    type = FunctionDirichletBC
    variable = disp_y
    boundary = top
    function = '-1.0 * t'
  []
[]

[Materials]
  [elasticity_tensor_hard]
    type = ComputeIsotropicElasticityTensor
    youngs_modulus = 100e9
    poissons_ratio = 0.17
  []
  [creep_plas_hard]
    type = ComputeMultipleInelasticStress
    inelastic_models = 'plast'
    max_iterations = 50
    relative_tolerance = 1e-6
    absolute_tolerance = 1e-6
  []
  [plast]
    type = PerzynaStressUpdate
    yield_stress = 350e6
    temperature = 500
    activation_energy = 110e3
    viscosity = 5e18
    n_exponent = 5
    melting_viscosity = 1e4
    alpha_yield = 0.00035
  []
[]


[Executioner]
  end_time = 1e99
  dt = 2e-3
  dtmin = 1e-5
  num_steps = 10

  type = Transient
  solve_type = NEWTON
  petsc_options_iname = '-pc_type -pc_factor_mat_solver_package'
  petsc_options_value = 'lu       superlu_dist'
  line_search = bt
  line_search_package = petsc

  l_abs_tol = 1e-5
  nl_max_its = 10
  nl_rel_tol = 1e-10      
  nl_abs_tol = 3e-2 
[]

[Outputs]
  exodus = true
  file_base = test_1
[]