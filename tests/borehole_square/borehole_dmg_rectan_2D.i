# Hole problem

[Mesh]
  [./file]
    type = FileMeshGenerator
    file = Plate_Hole_rectang2D.msh
    boundary_name = 'circle top bottom left right'
    boundary_id = '0 1 2 3 4'
  [../]
  [./middle_edge_bottom]
    type = ExtraNodesetGenerator
    input = file
    new_boundary = 101
    coord = '0 -1.5'
  [../]
  [./middle_edge_top]
    type = ExtraNodesetGenerator
    input = middle_edge_bottom
    new_boundary = 102
    coord = '0 1.5'
  [../]
  [./middle_edge_left]
    type = ExtraNodesetGenerator
    input = middle_edge_top
    new_boundary = 103
    coord = '-2 0 '
  [../]
  [./middle_edge_right]
    type = ExtraNodesetGenerator
    input = middle_edge_left
    new_boundary = 104
    coord = '2 0'
  [../]
[]

[Variables]
  active = 'damage disp_y disp_x'
  [./disp_x]
    order = FIRST
    family = LAGRANGE
  [../]
  [./disp_y]
    order = FIRST
    family = LAGRANGE
  [../]
  [./damage]
  [../]
  [./pressure]
  [../]
[]

[GlobalParams]
  time_factor = 1
  pressure = 10
[]

[Materials]
  [./mat_mech]
    type = RedbackMechMaterialDP
    block = 0
    disp_x = disp_x
    disp_y = disp_y
    youngs_modulus = 100
    poisson_ratio = 0.2
    yield_stress = '0 5 1 5'
    total_porosity = 0.
    damage = damage
    damage_coefficient = 0.1
    damage_method = BrittleDamage
  [../]
  [./mat_nomech]
    type = RedbackMaterial
    block = 0
    disp_x = disp_x
    disp_y = disp_y
    eta1 = 0
    phi0 = 0.1
    total_porosity = 0.1
    Peclet_number = 1e-3
    solid_density = 1
    delta = 0
    is_mechanics_on = true
    fluid_density = 0
    eta2 = 0
    solid_compressibility = 3.7037 # 1/(0.9*0.3)
  [../]
[]

[BCs]
  active = 'Pressure confine_x confine_y'
  [./confine_x]
    type = DirichletBC
    variable = disp_x
    boundary = '101 102'
    value = 0
  [../]
  [./confine_y]
    type = DirichletBC
    variable = disp_y
    value = 0
    boundary = '103 104'
  [../]
  [./Pressure]
    [./pressurization_borehole]
      disp_x = disp_x
      disp_y = disp_y
      boundary = circle
      function = pressure_fct
    [../]
    [./pressurization_ext_horizontal]
      disp_x = disp_x
      disp_y = disp_y
      boundary = 'left right'
      function = ext_pressure_horizontal_fct
    [../]
    [./pressurization_ext_vertical]
      disp_x = disp_x
      disp_y = disp_y
      boundary = 'top bottom'
      function = ext_pressure_vertical_fct
    [../]
  [../]
  [./top_load]
    type = NeumannBC
    variable = disp_y
    boundary = top
    value = -10
  [../]
[]

[AuxVariables]
  [./stress_xx]
    order = CONSTANT
    family = MONOMIAL
  [../]
  [./stress_xy]
    order = CONSTANT
    family = MONOMIAL
  [../]
  [./stress_yy]
    order = CONSTANT
    family = MONOMIAL
  [../]
  [./mises_stress]
    order = CONSTANT
    family = MONOMIAL
  [../]
  [./eqv_plastic_strain]
    order = CONSTANT
    family = MONOMIAL
  [../]
  [./mises_strain_rate]
    order = CONSTANT
    family = MONOMIAL
  [../]
  [./elastic_modulus]
    order = CONSTANT
    family = MONOMIAL
  [../]
[]

[Functions]
  [./ext_pressure_horizontal_fct]
    type = ParsedFunction
    value = 1
  [../]
  [./ext_pressure_vertical_fct]
    type = ParsedFunction
    value = 0.1
  [../]
  [./pressure_fct]
    type = ParsedFunction
    value = 1*tanh(5e3*t)
  [../]
  [./timestep_fct]
    type = ParsedFunction
    value = 'min( 1e-4 , max( 1e-7,    dt*max(1.5 - 100*(dmg - dmg_old), 0.1)) )'
    vals = 'max_damage max_damage_old dt_old'
    vars = 'dmg dmg_old dt'
  [../]
[]

[Kernels]
  [./td_damage]
    type = TimeDerivative
    variable = damage
  [../]
  [./damage_kernel]
    type = RedbackDamage
    variable = damage
  [../]
[]

[AuxKernels]
  active = 'elastic_mod mises_strain_rate stress_yy stress_xx stress_xy mises_stress eqv_plastic_strain'
  [./total_porosity]
    type = RedbackTotalPorosityAux
    variable = total_porosity
    mechanical_porosity = mech_porosity
  [../]
  [./stress_xx]
    type = RankTwoAux
    rank_two_tensor = stress
    variable = stress_xx
    index_i = 0
    index_j = 0
  [../]
  [./stress_xy]
    type = RankTwoAux
    rank_two_tensor = stress
    variable = stress_xy
    index_i = 0
    index_j = 1
  [../]
  [./stress_yy]
    type = RankTwoAux
    rank_two_tensor = stress
    variable = stress_yy
    index_i = 1
    index_j = 1
  [../]
  [./eqv_plastic_strain]
    type = MaterialRealAux
    variable = eqv_plastic_strain
    property = eqv_plastic_strain # eqv_plastic_strain
  [../]
  [./mises_stress]
    type = MaterialRealAux
    variable = mises_stress
    property = mises_stress
  [../]
  [./mises_strain_rate]
    type = MaterialRealAux
    variable = mises_strain_rate
    property = mises_strain_rate
  [../]
  [./elastic_mod]
    type = RankFourAux
    variable = elastic_modulus
    rank_four_tensor = elasticity_tensor
    index_l = 0
    index_j = 0
    index_k = 0
    index_i = 0
  [../]
[]

[Postprocessors]
  # [./p0]
  # type = PointValue
  # point = '0 0 0'
  # variable = pore_pressure
  # [../]
  # [./stress_xx]
  # type = PointValue
  # point = '0 1 0'
  # variable = stress_xx
  # [../]
  # [./stress_yy]
  # type = PointValue
  # point = '0 1 0'
  # variable = stress_yy
  # [../]
  # [./ydisp]
  # type = PointValue
  # variable = disp_y
  # point = '0 1 0'
  # [../]
  [./max_damage]
    type = NodalMaxValue
    variable = damage
    execute_on = timestep_end
  [../]
  [./dt_old]
    type = TimestepSize
    execute_on = timestep_begin
  [../]
  [./max_damage_old]
    type = NodalMaxValue
    variable = damage
    execute_on = timestep_begin
  [../]
  [./timestep]
    type = FunctionValuePostprocessor
    function = timestep_fct
  [../]
  [./diff_dmg]
    type = DifferencePostprocessor
    value1 = max_damage
    value2 = max_damage_old
  [../]
  [./Mises_stress_injection]
    type = PointValue
    variable = mises_stress
    point = '1 0 0'
  [../]
[]

[Preconditioning]
  [./SMP]
    # petsc_options_iname = '-ksp_type -pc_type -snes_atol -snes_rtol -snes_max_it'
    # petsc_options_value = 'bcgs bjacobi 1E-14 1E-10 10000'
    #
    petsc_options = '-snes_monitor -snes_linesearch_monitor -ksp_monitor'
    petsc_options_iname = '-ksp_type -pc_type -snes_atol -snes_rtol -snes_max_it -ksp_max_it -sub_pc_type -sub_pc_factor_shift_type'
    petsc_options_value = 'gmres asm 1E-4 1E-10 200 500 lu NONZERO'
    type = SMP
    full = true
  [../]
[]

[Executioner]
  type = Transient
  num_steps = 50
  solve_type = PJFNK
  end_time = 100
  dt = 1e-4
  petsc_options_iname = '-ksp_type -pc_type -sub_pc_type -ksp_gmres_restart'
  petsc_options_value = 'gmres asm lu 201'
  nl_abs_tol = 1e-10
  nl_rel_step_tol = 1e-10
  nl_rel_tol = 1e-06
  nl_abs_step_tol = 1e-10
  max_xfem_update = 1234567890
  [./TimeStepper]
    type = PostprocessorDT
    dt = 1e-6
    postprocessor = timestep
  [../]
[]

[Outputs]
  file_base = borehole_dmg_rectan_2D
  [./my_console]
    output_linear = true
    type = Console
    output_nonlinear = true
  [../]
  [./my_exodus]
    file_base = borehole_dmg_rectan_2D
    scalar_as_nodal = true
    type = Exodus
    elemental_as_nodal = true
  [../]
[]

[RedbackMechAction]
  [./solid]
    disp_x = disp_x
    disp_y = disp_y
    use_displaced_mesh = true
  [../]
[]

[ICs]
  [./damage_ic]
    variable = damage
    type = ConstantIC
    value = 0
  [../]
[]
