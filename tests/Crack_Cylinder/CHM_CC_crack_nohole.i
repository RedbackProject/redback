[Mesh]
  type = FileMesh
  file = Crack_in_cylinder.msh
  boundary_name = 'top bottom outside inside f_top f_bottom f_outside f_inside interface0 interface'
  boundary_id = '0 1 2 3 4 5 6 7 8 9'
[]

[Variables]
  active = 'pore_pressure disp_z disp_y disp_x'
  [./disp_x]
    block = 0
  [../]
  [./disp_y]
    block = 0
  [../]
  [./disp_z]
    block = 0
  [../]
  [./temp]
  [../]
  [./pore_pressure]
  [../]
[]

[Materials]
  [./mat_mech]
    type = RedbackMechMaterialCC
    block = 0
    disp_x = disp_x
    disp_y = disp_y
    disp_z = disp_z
    pore_pres = pore_pressure
    youngs_modulus = 125
    poisson_ratio = 0.3
    slope_yield_surface = 0.6
    yield_criterion = modified_Cam_Clay
    yield_stress = '0. 0.001 1 0.001'
    total_porosity = total_porosity
    damage = chemical_porosity
    chemo_mechanical_porosity_coeff = 1e3
  [../]
  [./mat_nomech]
    type = RedbackMaterial
    block = 0
    is_chemistry_on = true
    disp_x = disp_x
    disp_y = disp_y
    disp_z = disp_z
    pore_pres = pore_pressure
    Aphi = 1
    Kc = 1
    ar = 10
    ar_F = 11
    ar_R = 10
    da_endo = 1e-5
    gr = 9.08e-6 # 0.2*exp(-Ar), Ar=10
    alpha_2 = 3
    mu = 1e-3
    phi0 = 0.1
    total_porosity = total_porosity
    is_mechanics_on = true
  [../]
[]

[Functions]
  active = 'downfunc'
  [./upfunc]
    type = ParsedFunction
    value = t
  [../]
  [./downfunc]
    type = ParsedFunction
    value = 0.54+1e-1*t # 2e-4+1e-2*t
  [../]
  [./spline_IC]
    type = ConstantFunction
  [../]
[]

[BCs]
  active = 'press_crack_lips press_inside disp_y_inside disp_x_inside'
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
    boundary = 4
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
  [./disp_y_inside]
    type = DirichletBC
    variable = disp_y
    boundary = inside
    value = 0
  [../]
  [./disp_x_inside]
    type = DirichletBC
    variable = disp_x
    boundary = inside
    value = 0
  [../]
  [./press_crack_lips]
    type = FunctionDirichletBC
    variable = pore_pressure
    boundary = 'interface0 interface'
    function = downfunc
  [../]
  [./press_inside]
    type = FunctionDirichletBC
    variable = pore_pressure
    boundary = inside
    function = downfunc
  [../]
[]

[AuxVariables]
  active = 'mech_porosity Mod_Gruntfest_number solid_ratio total_porosity mises_strain Lewis_number mises_strain_rate volumetric_strain_rate mises_stress volumetric_strain mean_stress chemical_porosity'
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
  [../]
  [./total_porosity]
    order = FIRST
    family = MONOMIAL
  [../]
  [./mech_porosity]
    order = FIRST
    family = MONOMIAL
  [../]
  [./Lewis_number]
    order = CONSTANT
    family = MONOMIAL
  [../]
  [./solid_ratio]
    order = CONSTANT
    family = MONOMIAL
  [../]
  [./chemical_porosity]
    order = CONSTANT
    family = MONOMIAL
  [../]
[]

[Kernels]
  active = 'td_press poromech chem_press press_diff'
  [./td_temp]
    type = TimeDerivative
    variable = temp
  [../]
  [./temp_diff]
    type = Diffusion
    variable = temp
  [../]
  [./temp_dissip]
    type = RedbackMechDissip
    variable = temp
    block = 0
  [../]
  [./temp_endo_chem]
    type = RedbackChemEndo
    variable = temp
  [../]
  [./td_press]
    type = TimeDerivative
    variable = pore_pressure
  [../]
  [./press_diff]
    type = RedbackMassDiffusion
    variable = pore_pressure
    block = 0
  [../]
  [./chem_press]
    type = RedbackChemPressure
    variable = pore_pressure
    block = 0
  [../]
  [./poromech]
    type = RedbackPoromechanics
    variable = pore_pressure
    block = 0
  [../]
[]

[AuxKernels]
  active = 'mech_porosity volumetric_strain solid_ratio total_porosity mises_strain Lewis_number mises_strain_rate chemical_porosity volumetric_strain_rate mises_stress mean_stress Gruntfest_Number'
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
    block = 0
  [../]
  [./mises_strain]
    type = MaterialRealAux
    variable = mises_strain
    property = eqv_plastic_strain
    block = 0
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
  [./mean_stress]
    type = MaterialRealAux
    variable = mean_stress
    property = mean_stress
    block = 0
  [../]
  [./volumetric_strain]
    type = MaterialRealAux
    variable = volumetric_strain
    property = volumetric_strain
    block = 0
  [../]
  [./volumetric_strain_rate]
    type = MaterialRealAux
    variable = volumetric_strain_rate
    property = volumetric_strain_rate
    block = 0
  [../]
  [./total_porosity]
    type = RedbackTotalPorosityAux
    variable = total_porosity
    mechanical_porosity = mech_porosity
    block = 0
    is_mechanics_on = true
  [../]
  [./mech_porosity]
    type = MaterialRealAux
    variable = mech_porosity
    execute_on = timestep_end
    property = mechanical_porosity
    block = 0
  [../]
  [./Lewis_number]
    type = MaterialRealAux
    variable = Lewis_number
    property = lewis_number
  [../]
  [./solid_ratio]
    type = MaterialRealAux
    variable = solid_ratio
    property = solid_ratio
  [../]
  [./chemical_porosity]
    type = MaterialRealAux
    variable = chemical_porosity
    execute_on = linear
    property = chemical_porosity
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
  start_time = 0.0
  end_time = 100
  dtmax = 1
  dtmin = 1e-7
  type = Transient
  l_max_its = 100
  nl_max_its = 20
  solve_type = PJFNK
  petsc_options_iname = '-pc_type -pc_hypre_type -snes_linesearch_type -ksp_gmres_restart'
  petsc_options_value = 'hypre boomeramg cp 201'
  nl_abs_tol = 1e-8 # 1e-10 to begin with
  reset_dt = true
  line_search = basic
  nl_rel_tol = 1e-05
  [./TimeStepper]
    type = ConstantDT
    dt = 1e-4 # 1e-4
  [../]
[]

[Outputs]
  file_base = bench_THMC_CC_out
  output_initial = true
  exodus = true
  [./console]
    type = Console
  [../]
[]

[RedbackMechAction]
  [./solid]
    disp_z = disp_z
    disp_y = disp_y
    disp_x = disp_x
    pore_pres = pore_pressure
  [../]
[]

[ICs]
  active = 'press_ic'
  [./temp_IC]
    variable = temp
    type = ConstantIC
    value = 0
  [../]
  [./press_ic]
    variable = pore_pressure
    type = ConstantIC
    value = 0 # 1e-3
    block = 0
  [../]
[]

