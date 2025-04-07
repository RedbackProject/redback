[Mesh]
  type = FileMesh
  file = Cylinder_hollow_perturb.msh
  boundary_name = 'bottom top inside outside'
  boundary_id = '109 110 112 111'
[]

[GlobalParams]
  displacements = 'disp_x disp_y disp_z'
  porepressure = porepressure
  block = 0
[]

[Variables]
  [./disp_x]
    block = 0
  [../]
  [./disp_y]
    block = 0
  [../]
  [./disp_z]
    block = 0
  [../]
  [./porepressure]
    scaling = 1E9 # Notice the scaling, to make porepressure's kernels roughly of same magnitude as disp's kernels
    block = 0
  [../]
  [./damage]
  [../]
  [./temperature]
  [../]
[]

[AuxVariables]
  [./sigma_12]
    order = CONSTANT
    family = MONOMIAL
  [../]
  [./sigma_r_theta]
    order = CONSTANT
    family = MONOMIAL
  [../]
  [./plastic_strain_r_theta]
    order = CONSTANT
    family = MONOMIAL
  [../]
[]

[Functions]
  [./temp_ic]
    type = ParsedFunction
    value = 'sqrt(x*x+y*y)/0.9 - 1/9'
  [../]
  [./central_rotat]
    type = ParsedFunction
    value = 2e2*t
  [../]
[]

[BCs]
  [./fixed_outer_x]
    type = DirichletBC
    variable = disp_x
    value = 0
    boundary = outside
  [../]
  [./fixed_outer_y]
    type = DirichletBC
    variable = disp_y
    value = 0
    boundary = outside
  [../]
  [./plane_strain]
    type = DirichletBC
    variable = disp_z
    value = 0
    boundary = 'top bottom'
  [../]
  [./borehole_wall]
    type = DirichletBC
    variable = porepressure
    value = 5e-2 # 1e-2
    boundary = inside
  [../]
  [./Temp_outside]
    type = DirichletBC
    variable = temperature
    boundary = outside
    value = 1
  [../]
  [./Temp_borehole]
    type = DirichletBC
    variable = temperature
    boundary = inside
    value = 0
  [../]
  [./borehole_rotat_x]
    type = FunctionDirichletTransverseBC
    variable = disp_x
    boundary = inside
    function = central_rotat
    center = '0 0 0'
    dir_index = 0
    axis = '0 0 1'
    angular_velocity = 1000
  [../]
  [./borehole_rotat_y]
    type = FunctionDirichletTransverseBC
    variable = disp_y
    boundary = inside
    function = central_rotat
    center = '0 0 0'
    dir_index = 1
    axis = '0 0 1'
    angular_velocity = 1000
  [../]
[]

[Kernels]
  [./dp_dt]
    type = TimeDerivative
    variable = porepressure
    block = 0
  [../]
  [./mass_diff]
    type = RedbackMassDiffusion
    variable = porepressure
  [../]
  [./poromech]
    type = RedbackPoromechanics
    variable = porepressure
  [../]
  [./damage_dt]
    type = TimeDerivative
    variable = damage
  [../]
  [./damage_kernel]
    type = RedbackDamage
    variable = damage
  [../]
  [./dt_temp]
    type = TimeDerivative
    variable = temperature
  [../]
  [./diff_temp]
    type = Diffusion
    variable = temperature
  [../]
  [./mech_dissip]
    type = RedbackMechDissip
    variable = temperature
  [../]
  [./Thermal_press]
    type = RedbackThermalPressurization
    variable = porepressure
    temperature = temperature
  [../]
[]

[AuxKernels]
  [./sigma_12]
    type = RankTwoAux
    variable = sigma_12
    rank_two_tensor = stress
    index_j = 1
    index_i = 0
  [../]
  [./sigma_r_theta]
    type = RedbackPolarTensorMaterialAux
    variable = sigma_r_theta
    rank_two_tensor = stress
    index_j = 1
    index_i = 0
  [../]
  [./epsilon_r_theta]
    type = RedbackPolarTensorMaterialAux
    variable = plastic_strain_r_theta
    rank_two_tensor = plastic_strain
    index_j = 1
    index_i = 0
  [../]
[]

[Materials]
  active = 'no_mech plastic_material'
  [./no_mech]
    type = RedbackMaterial
    disp_z = disp_z
    disp_y = disp_y
    disp_x = disp_y
    pore_pres = porepressure
    total_porosity = 0.1
    phi0 = 0.1
    pressurization_coefficient = 1e-7
    temperature = temperature
    gr = 10
  [../]
  [./elastic_material]
    type = RedbackMechMaterialElastic
    disp_z = disp_z
    disp_y = disp_y
    disp_x = disp_x
    poisson_ratio = 0.25
    youngs_modulus = 100
    outputs = all
  [../]
  [./plastic_material]
    type = RedbackMechMaterialDP
    disp_z = disp_z
    disp_y = disp_y
    disp_x = disp_x
    outputs = all
    pore_pres = porepressure
    yield_stress = '0 0.006 1 0.006'
    poisson_ratio = 0.25
    youngs_modulus = 100
    damage = damage
    damage_coefficient = 1e3 # 1e5
    damage_method = BrittleDamage
    temperature = temperature
    chemo_mechanical_porosity_coeff = 1e18
  [../]
[]

[Postprocessors]
  [./Int_PoreP]
    type = PointValue
    variable = porepressure
    point = '0.1 0 0.1'
    execute_on = timestep_begin
  [../]
  [./PorePress]
    type = PointValue
    variable = porepressure
    point = '0.2 0 0.1'
  [../]
[]

[Preconditioning]
  active = 'andy'
  [./andy]
    # gmres asm 1E0 1E-10 200 500 lu NONZERO
    type = SMP
    full = true
    solve_type = PJFNK
    petsc_options = '-snes_monitor -snes_linesearch_monitor -ksp_monitor'
    petsc_options_iname = '-ksp_type -pc_type  -snes_atol -snes_rtol -snes_max_it -ksp_max_it -ksp_atol -sub_pc_type -sub_pc_factor_shift_type'
    petsc_options_value = 'gmres        asm        1E-2          1E-8        200                500                  1e-4        lu                      NONZERO'
  [../]
  [./manolis]
    type = SMP
    solve_type = NEWTON
    petsc_options_iname = '-ksp_type -pc_type -snes_atol -snes_rtol -snes_max_it'
    petsc_options_value = 'bcgs bjacobi 1E-14 1E-10 10000'
  [../]
  [./default]
    type = SMP
    solve_type = NEWTON
  [../]
  [./Manman]
    type = SMP
    solve_type = NEWTON
    petsc_options_iname = '-ksp_type -pc_type -snes_atol -snes_rtol -snes_max_it'
    petsc_options_value = 'bcgs bjacobi 1E-14 1E-10 10000'
  [../]
[]

[Executioner]
  # [./TimeStepper]
  # type = PostprocessorDT
  # postprocessor = dt
  # dt = 0.003
  # [../]
  type = Transient
  l_max_its = 100
  end_time = 0.3
  dt = 1e-5
  l_tol = 1e-5 # 1e-05
[]

[Outputs]
  execute_on = 'initial timestep_end'
  file_base = borehole_test_dmg_1
  [./my_exodus]
    type = Exodus
  [../]
[]

[RedbackMechAction]
  [./mechanics]
    disp_z = disp_z
    pore_pres = porepressure
    disp_y = disp_y
    disp_x = disp_x
  [../]
[]

[ICs]
  [./random_dmg_ic]
    variable = damage
    max = 0.1
    type = RandomIC
  [../]
  [./random_temp_ic]
    function = temp_ic
    max = 0.1
    type = FunctionWithRandomIC
    variable = temperature
  [../]
[]

