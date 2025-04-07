[Mesh]
  active = 'file up_point_outside'
  [./file]
    type = FileMeshGenerator
    file = Crack_in_cylinder_half_2D.msh
    dim = 2
    boundary_name = 'inside outside crack_lip sym_axis'
    boundary_id = '0 1 2 3'
    displacements = 'disp_x disp_y'
  [../]
  [./up_point_outside]
    type = ExtraNodesetGenerator
    input = file
    new_boundary = 102
    coord = '0 1'
  [../]
  [./down_point_outside]
    type = ExtraNodesetGenerator
    input = file
    new_boundary = 103
    coord = '0 -1 0'
  [../]
  [./left_point_outside]
    type = ExtraNodesetGenerator
    input = file
    new_boundary = 101
    coord = '-1 0 0'
  [../]
  [./left_point_inside]
    type = ExtraNodesetGenerator
    input = file
    new_boundary = 111
    coord = '-0.05 0 0'
  [../]
  [./up_point_inside]
    type = ExtraNodesetGenerator
    input = file
    new_boundary = 112
    coord = '0 0.05 0'
  [../]
  [./down_point_inside]
    type = ExtraNodesetGenerator
    input = file
    new_boundary = 113
    coord = '0 -0.05 0'
  [../]
[]

[Variables]
  [./disp_x]
    block = 0
  [../]
  [./disp_y]
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
    pore_pres = pore_pressure
    youngs_modulus = 125
    poisson_ratio = 0.3 # 0.3
    slope_yield_surface = 0.6
    yield_criterion = modified_Cam_Clay
    yield_stress = '0. 0.001 1 0.001'
    total_porosity = total_porosity
    damage = chemical_porosity
    chemo_mechanical_porosity_coeff = 1e3 # 1e3
  [../]
  [./mat_nomech]
    type = RedbackMaterial
    block = 0
    is_chemistry_on = true
    disp_x = disp_x
    disp_y = disp_y
    pore_pres = pore_pressure
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
  active = 'downfunc1 func_outside downfunc time_step_func'
  [./downfunc]
    type = ParsedFunction
    value = 0.054+1e-1*t # 0.54+1e-1*t
  [../]
  [./spline_IC]
    type = ConstantFunction
  [../]
  [./downfunc1]
    type = ParsedFunction
    value = 0 # 0.1+t
  [../]
  [./func_outside]
    type = ParsedFunction
    value = 0
  [../]
  [./time_step_func]
    # if(t<1e-3, 1e-4, 1e-3)
    type = ParsedFunction
    value = 'if(t<1e-3, t, 1e-3)' # if(t<0.0002, 2e-5, 1e-4)
  [../]
[]

[BCs]
  active = 'Pressure constrain_y constrain_x'
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
  [./press_crack_lips]
    type = FunctionDirichletBC
    variable = pore_pressure
    boundary = interface0
    function = downfunc1
  [../]
  [./press_inside]
    type = FunctionDirichletBC
    variable = pore_pressure
    boundary = inside
    function = downfunc
  [../]
  [./Pressure]
    [./press_lip]
      function = downfunc1
      disp_y = disp_y
      disp_x = disp_x
      boundary = crack_lip
    [../]
    [./press_inside]
      function = downfunc
      disp_y = disp_y
      disp_x = disp_x
      boundary = inside
    [../]
    [./press_outside]
      function = func_outside
      disp_y = disp_y
      disp_x = disp_x
      boundary = outside
    [../]
  [../]
  [./constrain_x]
    type = DirichletBC
    variable = disp_x
    boundary = 102
    value = 0
  [../]
  [./constrain_y]
    type = DirichletBC
    variable = disp_y
    boundary = sym_axis
    value = 0
  [../]
  [./Plain_strain]
    type = DirichletBC
    variable = disp_z
    boundary = 'top bottom'
    value = 0
  [../]
[]

[AuxVariables]
  active = 'mech_porosity plastic_strain_r_theta plastic_strain_theta_theta stress_r_theta chemical_porosity plastic_strain_r_r mises_strain_rate Mod_Gruntfest_number total_porosity mises_stress stress_theta_theta solid_ratio mises_strain Lewis_number mean_stress volumetric_strain stress_r_r volumetric_strain_rate'
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
  [./stress_r_r]
    order = CONSTANT
    family = MONOMIAL
    block = 0
  [../]
  [./stress_r_theta]
    order = CONSTANT
    family = MONOMIAL
    block = 0
  [../]
  [./stress_theta_theta]
    order = CONSTANT
    family = MONOMIAL
  [../]
  [./plastic_strain_r_r]
    order = CONSTANT
    family = MONOMIAL
    block = 0
  [../]
  [./plastic_strain_r_theta]
    order = CONSTANT
    family = MONOMIAL
  [../]
  [./plastic_strain_theta_theta]
    order = CONSTANT
    family = MONOMIAL
    block = 0
  [../]
[]

[Kernels]
  active = 'td_press td_temp temp_endo_chem chem_press press_diff'
  [./td_temp]
    type = TimeDerivative
    variable = temp
    block = 0
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
    block = 0
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
  active = 'mech_porosity plastic_strain_r_theta plastic_strain_theta_theta stress_r_theta chemical_porosity plastic_strain_r_r mises_strain_rate total_porosity mises_stress stress_theta_theta solid_ratio mises_strain Lewis_number mean_stress volumetric_strain stress_r_r volumetric_strain_rate Gruntfest_Number'
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
  [./plastic_strain_r_r]
    type = RedbackPolarTensorMaterialAux
    variable = plastic_strain_r_r
    rank_two_tensor = plastic_strain
    index_j = 0
    index_i = 0
  [../]
  [./plastic_strain_r_theta]
    type = RedbackPolarTensorMaterialAux
    variable = plastic_strain_r_theta
    rank_two_tensor = plastic_strain
    index_j = 1
    index_i = 0
    block = 0
  [../]
  [./plastic_strain_theta_theta]
    type = RedbackPolarTensorMaterialAux
    variable = plastic_strain_theta_theta
    rank_two_tensor = plastic_strain
    index_j = 1
    index_i = 1
    block = 0
  [../]
  [./stress_r_r]
    type = RedbackPolarTensorMaterialAux
    variable = stress_r_r
    rank_two_tensor = stress
    index_j = 0
    index_i = 0
    block = 0
  [../]
  [./stress_r_theta]
    type = RedbackPolarTensorMaterialAux
    variable = stress_r_theta
    rank_two_tensor = stress
    index_j = 1
    index_i = 0
    block = 0
  [../]
  [./stress_theta_theta]
    type = RedbackPolarTensorMaterialAux
    variable = stress_theta_theta
    rank_two_tensor = stress
    index_j = 1
    index_i = 1
    block = 0
  [../]
[]

[Postprocessors]
  [./disp_y]
    type = PointValue
    variable = disp_y
    point = '0 0.05 0'
  [../]
  [./strain_rr_tip]
    type = PointValue
    variable = plastic_strain_r_r
    point = '-0.05 0 0'
  [../]
  [./new_timestep]
    type = FunctionValuePostprocessor
    function = time_step_func
  [../]
  [./disp_x_left]
    type = PointValue
    variable = disp_x
    point = '-0.05 0 0'
  [../]
  [./disp_x_right]
    type = PointValue
    variable = disp_x
    point = '0.05 0 0'
  [../]
  [./strain_rr_top]
    type = PointValue
    variable = plastic_strain_r_r
    point = '0 0.05 0'
  [../]
  [./strain_rr_right]
    type = PointValue
    variable = plastic_strain_r_r
    point = '0.05 0 0'
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
    # 2e-5
    type = PostprocessorDT
    postprocessor = new_timestep
  [../]
[]

[Outputs]
  file_base = CHM_half_2D_test3
  output_initial = true
  exodus = true
  [./console]
    type = Console
  [../]
[]

[RedbackMechAction]
  [./solid]
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
