[Mesh]
  type = FileMesh
  file = Cylinder_hollow_reg.msh
  dim = 2
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
  active = 'mat_nomech'
  [./mat_mech]
    type = RedbackMechMaterialJ2
    block = 0
    disp_x = disp_x
    disp_y = disp_y
    disp_z = disp_z
    exponent = 1
    youngs_modulus = 1000
    poisson_ratio = 0.3
    ref_pe_rate = 1
    yield_stress = '0. 1 1. 1'
    pore_pres = pore_pressure
    temperature = temp
    total_porosity = total_porosity
  [../]
  [./mat_nomech]
    type = RedbackMaterial
    block = 0
    alpha_2 = 3
    mu = 1e-3
    ar = 10
    gr = 3.632e-4 # 8*exp(-Ar), Ar=10
    Kc = 10
    ar_F = 20
    ar_R = 10
    phi0 = 0.1
    eta1 = 1e3
    Aphi = 1
    da_endo = 1e-4
    is_chemistry_on = true
    disp_z = disp_z
    disp_x = disp_x
    disp_y = disp_y
    total_porosity = 0.1
  [../]
[]

[Functions]
  active = 'central_rotation downfunc r_square'
  [./upfunc]
    type = ParsedFunction
    value = t
  [../]
  [./downfunc]
    type = ParsedFunction
    value = -3e-2*t
  [../]
  [./spline_IC]
    type = ConstantFunction
  [../]
  [./central_rotation]
    type = ParsedFunction
    value = 1e-2*t
  [../]
  [./r_square]
    type = ParsedFunction
    value = '(0.3+x2)*(0.3+x2) + y2*y2'
    vals = 'x_point2 y_point2'
    vars = 'x2 y2'
  [../]
[]

[BCs]
  active = 'central_disp_y central_disp_x left_disp rigth_disp_y left_disp_y'
  [./left_disp]
    type = DirichletBC
    variable = disp_x
    boundary = 2
    value = 0
  [../]
  [./right_disp]
    type = FunctionDirichletBC
    variable = disp_x
    boundary = 1
    function = downfunc
  [../]
  [./left_disp_y]
    type = DirichletBC
    variable = disp_z
    boundary = 2
    value = 0
  [../]
  [./temp_mid_pts]
    type = DirichletBC
    variable = temp
    boundary = '4 5 6 7'
    value = 0
  [../]
  [./rigth_disp_y]
    type = DirichletBC
    variable = disp_y
    boundary = 2
    value = 0
  [../]
  [./press_bc]
    type = DirichletBC
    variable = pore_pressure
    boundary = 2
    value = 0
  [../]
  [./low_temp]
    type = DirichletBC
    variable = temp
    boundary = 2
    value = 0
  [../]
  [./constant_force_inner_x]
    type = NeumannBC
    variable = disp_x
    boundary = 3
    value = -1
  [../]
  [./constant_force_inner_y]
    type = NeumannBC
    variable = disp_y
    boundary = 3
    value = -1
  [../]
  [./central_disp_x]
    type = FunctionDirichletTransverseBC
    variable = disp_x
    boundary = 3
    function = central_rotation
    center = '0 0 0'
    dir_index = 0
    axis = '0 0 1'
    angular_velocity = 5
  [../]
  [./central_disp_y]
    type = FunctionDirichletTransverseBC
    variable = disp_y
    boundary = 3
    function = central_rotation
    center = '0 0 0'
    dir_index = 1
    axis = '0 0 1'
    angular_velocity = 5
  [../]
[]

[Kernels]
  active = 'ddx_dt ddy_dt ddz_dt'
  [./td_temp]
    type = TimeDerivative
    variable = temp
  [../]
  [./diff_temp]
    type = Diffusion
    variable = temp
  [../]
  [./mh_temp]
    type = RedbackMechDissip
    variable = temp
  [../]
  [./td_press]
    type = TimeDerivative
    variable = pore_pressure
  [../]
  [./press_diff]
    type = RedbackMassDiffusion
    variable = pore_pressure
  [../]
  [./chem_press]
    type = RedbackChemPressure
    variable = pore_pressure
    block = 0
  [../]
  [./Chem_endo_temp]
    type = RedbackChemEndo
    variable = temp
    block = 0
  [../]
  [./ddx_dt]
    type = TimeDerivative
    variable = disp_x
  [../]
  [./ddy_dt]
    type = TimeDerivative
    variable = disp_y
  [../]
  [./ddz_dt]
    type = TimeDerivative
    variable = disp_z
  [../]
[]

[Preconditioning]
  # active = ''
  [./SMP]
    type = SMP
    full = true
    line_search = basic
  [../]
[]

[Postprocessors]
  active = 'x_point2 y_point2 r_square_pp'
  [./middle_temp]
    type = PointValue
    variable = temp
    point = '0.51 0 0.25'
  [../]
  [./strain]
    type = StrainRatePoint
    variable = temp
    point = '0 0 0'
  [../]
  [./middle_press]
    type = PointValue
    variable = pore_pressure
    point = '0.51 0 0.25'
  [../]
  [./porosity_middle]
    type = PointValue
    variable = total_porosity
    point = '0.51 0 0.25'
  [../]
  [./Lewis_middle]
    type = PointValue
    variable = Lewis_number
    point = '0.51 0 0.25'
  [../]
  [./strain_rate_middle]
    type = PointValue
    variable = strain_rate
    point = '0.51 0 0.25'
  [../]
  [./solid_ratio_middle]
    type = PointValue
    variable = solid_ratio
    point = '0.51 0 0.25'
  [../]
  [./mises_stress_centre]
    type = PointValue
    variable = mises_stress
    point = '0.51 0 0.25'
  [../]
  [./x_point2]
    type = PointValue
    variable = disp_x
    point = '0.3 0 0'
  [../]
  [./y_point2]
    type = PointValue
    variable = disp_y
    point = '0.3 0 0'
  [../]
  [./r_square_pp]
    type = FunctionValuePostprocessor
    function = r_square
  [../]
[]

[Executioner]
  # Preconditioned JFNK (default)
  start_time = 0.0
  end_time = 100
  num_steps = 30
  dtmax = 1
  dtmin = 1e-7
  type = Transient
  l_max_its = 200
  nl_max_its = 10
  solve_type = PJFNK
  petsc_options_iname = '-pc_type -pc_hypre_type -snes_linesearch_type -ksp_gmres_restart'
  petsc_options_value = 'hypre boomeramg cp 201'
  nl_abs_tol = 1e-10 # 1e-10 to begin with
  reset_dt = true
  line_search = basic
  [./TimeStepper]
    type = ConstantDT
    dt = 5e-3
  [../]
[]

[Outputs]
  file_base = ring_shear_out
  output_initial = true
  exodus = true
  print_linear_residuals = false
  [./console]
    type = Console
    perf_log = true
  [../]
[]

