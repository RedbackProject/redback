[Mesh]
  type = GeneratedMesh
  dim = 2
  nx = 2
  ny = 2
  xmin = -1.5
  xmax = 1.5
  ymin = -1
[]

[Variables]
  inactive = 'temp'
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
[]

[Materials]
  [./mat_mech]
    type = RedbackMechMaterialCC
    block = '0'
    disp_x = 'disp_x'
    disp_y = 'disp_y'
    disp_z = 'disp_z'
    exponent = 1
    fill_method = symmetric_isotropic
    C_ijkl = '576.923076923 384.615384615' # young = 1000, poisson = 0.3
    ref_pe_rate = 1
    slope_yield_surface = -0.6
    yield_stress = '0. 1 1. 1'
    total_porosity = 'total_porosity'
  [../]
  [./mat_nomech]
    type = RedbackMaterial
    block = '0'
    disp_x = 'disp_x'
    disp_y = 'disp_y'
    disp_z = 'disp_z'
    is_mechanics_on = false
    Aphi = 0
    ar = 1
    ar_F = 1
    ar_R = 1
    gr = 0.1
    alpha_2 = 1
    phi0 = 0.1
    ref_lewis_nb = 1
    total_porosity = 'total_porosity'
  [../]
[]

[Functions]
  inactive = 'upfunc spline_IC'
  [./upfunc]
    type = ParsedFunction
    value = 't'
  [../]
  [./downfunc]
    type = ParsedFunction
    value = '-1*t'
  [../]
  [./spline_IC]
    type = ConstantFunction
  [../]
[]

[BCs]
  inactive = 'bottom_temp top_temp temp_mid_pts temp_box constant_force_right'
  [./left_disp]
    type = DirichletBC
    variable = disp_x
    boundary = '3'
    value = 0
  [../]
  [./right_disp]
    type = FunctionDirichletBC
    variable = disp_x
    boundary = '1'
    function = downfunc
  [../]
  [./bottom_temp]
    type = NeumannBC
    variable = temp
    boundary = '0'
    value = -1
  [../]
  [./top_temp]
    type = NeumannBC
    variable = temp
    boundary = '2'
    value = -1
  [../]
  [./left_disp_y]
    type = DirichletBC
    variable = disp_y
    boundary = '3'
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
    boundary = '1'
    value = 0
  [../]
  [./temp_box]
    type = DirichletBC
    variable = temp
    boundary = '0 1 2 3'
    value = 0
  [../]
  [./constant_force_right]
    type = NeumannBC
    variable = disp_x
    boundary = '1'
    value = -2
  [../]
[]

[AuxVariables]
  inactive = 'stress_zz peeq pe11 pe22 pe33 volumetric_strain_rate'
  [./stress_zz]
    order = CONSTANT
    family = MONOMIAL
  [../]
  [./peeq]
    order = CONSTANT
    family = MONOMIAL
  [../]
  [./pe11]
    order = CONSTANT
    family = MONOMIAL
  [../]
  [./pe22]
    order = CONSTANT
    family = MONOMIAL
  [../]
  [./pe33]
    order = CONSTANT
    family = MONOMIAL
  [../]
  [./mises_stress]
    order = CONSTANT
    family = MONOMIAL
  [../]
  [./mises_strain]
    order = CONSTANT
    family = MONOMIAL
  [../]
  [./mises_strain_rate]
    order = CONSTANT
    family = MONOMIAL
    block = '0'
  [../]
  [./Mod_Gruntfest_number]
    order = CONSTANT
    family = MONOMIAL
  [../]
  [./volumetric_strain]
    order = CONSTANT
    family = MONOMIAL
  [../]
  [./volumetric_strain_rate]
    order = CONSTANT
    family = MONOMIAL
  [../]
  [./mean_stress]
    order = CONSTANT
    family = MONOMIAL
    block = '0'
  [../]
  [./returnmap_iter]
    order = CONSTANT
    family = MONOMIAL
    block = '0'
  [../]
  [./total_porosity]
    order = FIRST
    family = MONOMIAL
  [../]
  [./mech_porosity]
    order = FIRST
    family = MONOMIAL
  [../]
[]

[AuxKernels]
  inactive = 'stress_zz pe11 pe22 pe33 eqv_plastic_strain volumetric_strain_rate'
  [./stress_zz]
    type = RankTwoAux
    rank_two_tensor = stress
    variable = stress_zz
    index_i = 2
    index_j = 2
  [../]
  [./pe11]
    type = RankTwoAux
    rank_two_tensor = plastic_strain
    variable = pe11
    index_i = 0
    index_j = 0
  [../]
  [./pe22]
    type = RankTwoAux
    rank_two_tensor = plastic_strain
    variable = pe22
    index_i = 1
    index_j = 1
  [../]
  [./pe33]
    type = RankTwoAux
    rank_two_tensor = plastic_strain
    variable = pe33
    index_i = 2
    index_j = 2
  [../]
  [./eqv_plastic_strain]
    type = FiniteStrainPlasticAux
    variable = 'peeq'
  [../]
  [./mises_stress]
    type = MaterialRealAux
    variable = mises_stress
    property = mises_stress
  [../]
  [./mises_strain]
    type = MaterialRealAux
    variable = mises_strain
    property = mises_strain
  [../]
  [./mises_strain_rate]
    type = MaterialRealAux
    variable = mises_strain_rate
    block = '0'
    property = mises_strain_rate
  [../]
  [./Gruntfest_Number]
    type = MaterialRealAux
    variable = Mod_Gruntfest_number
    property = mod_gruntfest_number
    block = '0'
  [../]
  [./mean_stress]
    type = MaterialRealAux
    variable = mean_stress
    property = mean_stress
    block = '0'
  [../]
  [./volumetric_strain]
    type = MaterialRealAux
    variable = volumetric_strain
    property = volumetric_strain
  [../]
  [./volumetric_strain_rate]
    type = MaterialRealAux
    variable = volumetric_strain_rate
    property = volumetric_strain_rate
  [../]
  [./returnmap_iter]
    type = MaterialRealAux
    variable = returnmap_iter
    property = returnmap_iter
    block = '0'
  [../]
  [./total_porosity]
    type = RedbackTotalPorosityAux
    variable = total_porosity
    mechanical_porosity = 'mech_porosity'
  [../]
  [./mech_porosity]
    type = MaterialRealAux
    variable = mech_porosity
    execute_on = 'timestep_end'
    property = mechanical_porosity
  [../]
[]

[Postprocessors]
  inactive = 'temp_middle volumetric_strain_rate'
  [./mises_stress]
    type = PointValue
    variable = mises_stress
    point = '0 0 0'
  [../]
  [./mises_strain]
    type = PointValue
    variable = mises_strain
    point = '0 0 0'
  [../]
  [./mises_strain_rate]
    type = PointValue
    variable = mises_strain_rate
    point = '0 0 0'
  [../]
  [./temp_middle]
    type = PointValue
    variable = temp
    point = '0 0 0'
  [../]
  [./mean_stress]
    type = PointValue
    variable = mean_stress
    point = '0 0 0'
  [../]
  [./volumetric_strain]
    type = PointValue
    variable = volumetric_strain
    point = '0 0 0'
  [../]
  [./volumetric_strain_rate]
    type = PointValue
    variable = volumetric_strain_rate
    point = '0 0 0'
  [../]
  [./max_returnmap_iter]
    type = ElementExtremeValue
    variable = 'returnmap_iter'
  [../]
[]

[Preconditioning]
  # active = ''
  [./SMP]
    type = SMP
    full = true
  [../]
[]

[Executioner]
  # Preconditioned JFNK (default)
  # petsc_options_iname = '-pc_type -pc_hypre_type -snes_linesearch_type -ksp_gmres_restart'
  # petsc_options_value = 'hypre boomeramg cp 201'
  type = Transient
  start_time = 0.0
  end_time = 0.006
  dtmax = 1
  dtmin = 1e-7
  l_max_its = 200
  nl_max_its = 10
  solve_type = PJFNK
  nl_abs_tol = 1e-10  # 1e-10 to begin with
  reset_dt = true
  line_search = basic
  [./TimeStepper]
    type = ReturnMapIterDT
    dt = 1e-3
    min_iter = 10
    ratio = 0.5
    max_iter = 20
    dt_max = 1e-3
    postprocessor = max_returnmap_iter
    dt_min = 1e-5
  [../]
[]

[Outputs]
  # output_initial = true
  file_base = bench_CC_out
  exodus = true
  csv = true
  print_linear_residuals = true
  [./console]
    type = Console
    perf_log = true
  [../]
[]

[RedbackMechAction]
  [./solid]
    disp_z = disp_z
    disp_y = disp_y
    disp_x = disp_x
  [../]
[]
