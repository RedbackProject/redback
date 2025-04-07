[Mesh]
  [./generate]
    type = GeneratedMeshGenerator
    dim = 2
    nx = 4
    ny = 4
    xmin = -1
    ymin = -1
  []
  [./left_mid_point]
    type = ExtraNodesetGenerator
    input = generate
    coord = '-1 0'
    new_boundary = 4
  [../]
  [./right_mid_point]
    type = ExtraNodesetGenerator
    input = left_mid_point
    coord = '1 0'
    new_boundary = 5
  [../]
  [./top_mid_point]
    type = ExtraNodesetGenerator
    input = right_mid_point
    coord = '0 1'
    new_boundary = 6
  [../]
  [./bottom_mid_point]
    type = ExtraNodesetGenerator
    input = top_mid_point
    coord = '0 -1'
    new_boundary = 7
  [../]
[]

[Variables]
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
  [./mat0]
    type = RedbackMechMaterialCC
    block = 0
    disp_y = disp_y
    disp_x = disp_x
    disp_z = 0
    temperature = temp
    yield_stress = '0. 1 1. 1'
    exponent = 1
    ref_pe_rate = 1
    fill_method = symmetric_isotropic
    C_ijkl = '5.76923076923 3.84615384615' # young = 10, poisson = 0.3
    slope_yield_surface = 0.3
    total_porosity = total_porosity
  [../]
  [./mat1]
    type = RedbackMaterial
    block = 0
    disp_y = disp_y
    disp_x = disp_x
    disp_z = 0
    temperature = temp
    Aphi = 0
    ar = 5
    gr = 6.7e-3 # exp(-Ar)
    pore_pres = 0
    is_mechanics_on = true
    phi0 = 0.5
    ref_lewis_nb = 1
    ar_F = 1
    ar_R = 1
    total_porosity = total_porosity
    alpha_2 = 1
  [../]
[]

[BCs]
  active = 'constant_force_right temp_box left_disp rigth_disp_y left_disp_y'
  [./left_disp]
    type = DirichletBC
    variable = disp_x
    boundary = 3
    value = 0
  [../]
  [./right_disp]
    type = FunctionDirichletBC
    variable = disp_x
    boundary = 1
    function = downfunc
  [../]
  [./bottom_temp]
    type = NeumannBC
    variable = temp
    boundary = 0
    value = -1
  [../]
  [./top_temp]
    type = NeumannBC
    variable = temp
    boundary = 2
    value = -1
  [../]
  [./left_disp_y]
    type = DirichletBC
    variable = disp_y
    boundary = 3
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
    boundary = 1
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
    boundary = 1
    value = -2
  [../]
[]

[AuxVariables]
  active = 'Mod_Gruntfest_number returnmap_iter mises_strain mises_strain_rate mises_stress total_porosity'
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
    block = 0
  [../]
  [./Mod_Gruntfest_number]
    order = CONSTANT
    family = MONOMIAL
  [../]
  [./returnmap_iter]
    order = CONSTANT
    family = MONOMIAL
    block = 0
  [../]
  [./total_porosity]
    family = MONOMIAL
    block = 0
  [../]
[]

[Kernels]
  [./temp_td]
    type = TimeDerivative
    variable = temp
  [../]
  [./temp_diff]
    type = AnisotropicDiffusion
    variable = temp
    tensor_coeff = '1 0 0 0 1 0 0 0 1'
  [../]
  [./temp_dissip]
    type = RedbackMechDissip
    variable = temp
  [../]
[]

[AuxKernels]
  active = 'total_porosity mises_strain mises_strain_rate mises_stress returnmap_iter Gruntfest_Number'
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
    variable = peeq
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
    block = 0
    property = mises_strain_rate
  [../]
  [./Gruntfest_Number]
    type = MaterialRealAux
    variable = Mod_Gruntfest_number
    property = mod_gruntfest_number
    block = 0
  [../]
  [./returnmap_iter]
    type = MaterialRealAux
    variable = returnmap_iter
    property = returnmap_iter
    block = 0
  [../]
  [./total_porosity]
    type = RedbackTotalPorosityAux
    variable = total_porosity
  [../]
[]

[Postprocessors]
  active = 'temp_centre timestep max_returnmap_iter'
  [./test]
    type = StrainRatePoint
    variable = temp
    point = ' 0.5 0.5 0'
  [../]
  [./temp_centre]
    type = PointValue
    variable = temp
    point = '0 0 0'
  [../]
  [./strain_rate]
    type = PointValue
    variable = mises_strain_rate
    point = '0.5 0.5 0'
  [../]
  [./mises_stress]
    type = PointValue
    variable = mises_stress
    point = '0.5 0.5 0'
  [../]
  [./Gruntfest_number]
    type = PointValue
    variable = Mod_Gruntfest_number
    point = '0 0 0'
  [../]
  [./max_returnmap_iter]
    type = ElementExtremeValue
    variable = returnmap_iter
  [../]
  [./timestep]
    type = TimestepSize
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
  start_time = 0.0
  end_time = 3e-2
  dtmax = 1
  dtmin = 1e-7
  type = Transient
  num_steps = 1000000
  l_max_its = 500
  solve_type = PJFNK
  nl_abs_tol = 1e-10 # 1e-10 to begin with
  reset_dt = true
  line_search = basic
  [./TimeStepper]
    type = ReturnMapIterDT
    dt = 1e-2
    min_iter = 10
    ratio = 0.5
    max_iter = 20
    dt_max = 1e-1
    postprocessor = max_returnmap_iter
    dt_min = 1e-5
  [../]
[]

[Outputs]
  file_base = timestep_out
  # output_initial = true
  exodus = true
  csv = true
  [./console]
    type = Console
    perf_log = true
    # linear_output = false
  [../]
[]

[ICs]
  active = 'ic_temp'
  [./ic_temp]
    variable = temp
    value = 0
    type = ConstantIC
  [../]
  [./Spline_IC]
    function = spline_IC
    variable = temp
    type = FunctionIC
    block = 0
  [../]
[]

[RedbackMechAction]
  [./solid]
    disp_z = disp_z
    disp_y = disp_y
    disp_x = disp_x
  [../]
[]
