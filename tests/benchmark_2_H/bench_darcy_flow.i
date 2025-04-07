[Mesh]
  type = GeneratedMesh
  dim = 2
  nx = 20
  ny = 10
  ymin = -0.5
  ymax = 0
[]

[Variables]
  active = 'pore_pressure'
  [./temp]
  [../]
  [./pore_pressure]
  [../]
[]

[Materials]
  [./redback_nomech]
    # output_properties = mixture_density
    type = RedbackMaterial
    block = 0
    pore_pres = pore_pressure
    Aphi = 1
    ar = 10
    ar_F = 1
    ar_R = 1
    are_convective_terms_on = true
    delta = 0.0333333333333
    eta1 = 1e3
    fluid_compressibility = 0.0
    fluid_thermal_expansion = 0.0
    gr = 1
    gravity = '0 -0.0 0'
    Peclet_number = 1.0
    phi0 = 0.1
    pressurization_coefficient = 0.0
    ref_lewis_nb = 1.8e-05
    solid_compressibility = 2e-05
    solid_density = 2.0
    solid_thermal_expansion = 0.0
    total_porosity = total_porosity
  [../]
[]

[Functions]
  active = 'init_gradient_P timestep_function'
  [./init_gradient_T]
    type = ParsedFunction
    value = 0.0-y*(1.0-0.0)*1000/500
  [../]
  [./init_gradient_P]
    # P_top-rho*g*y
    type = ParsedFunction
    value = 1.0-0.0*y
  [../]
  [./timestep_function]
    type = ParsedFunction
    value = 'min(max(1e-15, dt*max(0.2, 1-0.05*(n_li-50))), (1e-2)*50.0/max(abs(v_max), abs(v_min)))'
    vals = 'num_li num_nli min_fluid_vel_y max_fluid_vel_y dt'
    vars = 'n_li n_nli v_min v_max dt'
  [../]
[]

[ICs]
  active = 'press_IC'
  [./temp_IC]
    variable = temp
    type = FunctionIC
    function = init_gradient_T
  [../]
  [./press_IC]
    variable = pore_pressure
    type = FunctionIC
    function = init_gradient_P
  [../]
[]

[BCs]
  active = 'p_pressure_right p_pressure_left'
  [./temperature_top]
    type = DirichletBC
    variable = temp
    boundary = top
    value = 0.0
  [../]
  [./temperature_bottom]
    type = DirichletBC
    variable = temp
    boundary = bottom
    value = 1.0
  [../]
  [./pore_pressure_top]
    type = DirichletBC
    variable = pore_pressure
    boundary = top
    value = 1.0
  [../]
  [./p_pressure_left]
    type = DirichletBC
    variable = pore_pressure
    boundary = left
    value = 2.0
  [../]
  [./p_pressure_right]
    type = DirichletBC
    variable = pore_pressure
    boundary = right
    value = 1.0
  [../]
[]

[AuxVariables]
  [./total_porosity]
    family = MONOMIAL
  [../]
  [./Lewis_number]
    order = CONSTANT
    family = MONOMIAL
  [../]
  [./fluid_vel_y]
    order = CONSTANT
    family = MONOMIAL
  [../]
  [./fluid_vel_x]
    order = CONSTANT
    family = MONOMIAL
  [../]
[]

[Kernels]
  active = 'press_diff'
  [./temp_td]
    type = TimeDerivative
    variable = temp
  [../]
  [./temp_diff]
    type = Diffusion
    variable = temp
  [../]
  [./temp_conv]
    type = RedbackThermalConvection
    variable = temp
  [../]
  [./press_td]
    type = TimeDerivative
    variable = pore_pressure
  [../]
  [./press_diff]
    type = RedbackMassDiffusion
    variable = pore_pressure
  [../]
  [./pres_conv]
    type = RedbackMassConvection
    variable = pore_pressure
    temperature = temp
  [../]
  [./press_thermPress]
    type = RedbackThermalPressurization
    variable = pore_pressure
    temperature = temp
  [../]
[]

[AuxKernels]
  [./total_porosity]
    type = RedbackTotalPorosityAux
    variable = total_porosity
    mechanical_porosity = 0
    execute_on = linear
  [../]
  [./Lewis_number]
    type = MaterialRealAux
    variable = Lewis_number
    property = lewis_number
  [../]
  [./fluid_vel_y]
    type = MaterialRealVectorValueAux
    component = 1
    variable = fluid_vel_y
    property = fluid_velocity
  [../]
  [./fluid_vel_x]
    type = MaterialRealVectorValueAux
    variable = fluid_vel_x
    property = fluid_velocity
  [../]
[]

[Preconditioning]
  [./SMP]
    type = SMP
    full = true
  [../]
[]

[Postprocessors]
  active = 'num_nli min_fluid_vel_y num_li fluid_vel_x_pt middle_press new_timestep max_fluid_vel_y dt'
  [./middle_temp]
    type = PointValue
    variable = temp
    point = '0.5 -0.25 0'
  [../]
  [./middle_press]
    type = PointValue
    variable = pore_pressure
    point = '0.5 -0.25 0'
  [../]
  [./middle_porosity]
    type = PointValue
    variable = total_porosity
    point = '0.5 -0.25 0'
  [../]
  [./dt]
    type = TimestepSize
  [../]
  [./num_li]
    type = NumLinearIterations
  [../]
  [./num_nli]
    type = NumNonlinearIterations
  [../]
  [./new_timestep]
    type = FunctionValuePostprocessor
    function = timestep_function
  [../]
  [./max_fluid_vel_y]
    type = ElementExtremeValue
    variable = fluid_vel_y
    execute_on = nonlinear
    value_type = max
  [../]
  [./min_fluid_vel_y]
    type = ElementExtremeValue
    variable = fluid_vel_y
    execute_on = nonlinear
    value_type = min
  [../]
  [./fluid_vel_x_pt]
    type = PointValue
    variable = fluid_vel_x
    point = '0.5 -0.25 0'
  [../]
[]

[Executioner]
  # petsc_options_iname = '-pc_type -pc_hypre_type -snes_linesearch_type -ksp_gmres_restart -snes_converged_reason'
  # petsc_options_value = 'hypre boomeramg cp 201 1'
  type = Steady
  l_max_its = 200
  nl_max_its = 10
  solve_type = PJFNK
  petsc_options_iname = '-ksp_type -pc_type -sub_pc_type -ksp_gmres_restart '
  petsc_options_value = 'gmres asm lu 201'
  nl_abs_tol = 1e-8 # 1e-10 to begin with
  line_search = basic
[]

[Outputs]
  file_base = bench_darcy_flow
  exodus = true
  print_linear_residuals = true
  [./console]
    type = Console
    perf_log = true
  [../]
[]
