# Footing problem (extruded in 3D)

[Mesh]
  type = FileMesh
  file = ../../meshes/2d_footing_pb_coarse.msh
  boundary_name = 'bottom right top_no_pressure top_pressure left'
  boundary_id = '0 1 2 3 4'
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
  [./temp]
  [../]
  [./pore_pressure]
  [../]
  [./damage]
  [../]
[]

[GlobalParams]
  time_factor = 1
[]

[Materials]
  [./mat_mech]
    type = RedbackMechMaterialDP
    block = 0
    disp_x = disp_x
    disp_y = disp_y
    youngs_modulus = 100
    poisson_ratio = 0.2
    yield_stress = '0 1 0 1' # 0. 0.01 0.01 0.005 0.1 0.001
    total_porosity = 0.1
    damage = damage
    damage_coefficient = 60
  [../]
  [./mat_nomech]
    type = RedbackMaterial
    block = 0
    disp_x = disp_x
    disp_y = disp_y
    Aphi = 0
    ar = 0
    ar_F = 0
    ar_R = 0
    eta1 = 0
    gr = 1 # exp(-Ar), Ar=10
    phi0 = 0.1
    ref_lewis_nb = 1
    total_porosity = 0.1
    Peclet_number = 1e-3
    solid_density = 0
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
    boundary = 'left right bottom'
    value = 0
  [../]
  [./confine_y]
    type = DirichletBC
    variable = disp_y
    value = 0
    boundary = bottom
  [../]
  [./pore_pressure_top]
    type = DirichletBC
    variable = pore_pressure
    value = 0
    boundary = 'top_pressure top_no_pressure'
  [../]
  [./top_load]
    type = FunctionNeumannBC
    variable = disp_y
    boundary = top_pressure
    function = applied_load_fct
  [../]
  [./Pressure]
    [./top_pressure]
      function = applied_load_fct
      disp_y = disp_y
      disp_x = disp_x
      boundary = top_pressure
    [../]
  [../]
[]

[AuxVariables]
  active = 'stress_yy mises_strain_rate stress_xz stress_xx stress_xy mises_stress stress_zz eqv_plastic_strain stress_yz'
  [./total_porosity]
    order = FIRST
    family = MONOMIAL
  [../]
  [./stress_xx]
    order = CONSTANT
    family = MONOMIAL
  [../]
  [./stress_xy]
    order = CONSTANT
    family = MONOMIAL
  [../]
  [./stress_xz]
    order = CONSTANT
    family = MONOMIAL
  [../]
  [./stress_yy]
    order = CONSTANT
    family = MONOMIAL
  [../]
  [./stress_yz]
    order = CONSTANT
    family = MONOMIAL
  [../]
  [./stress_zz]
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
[]

[Functions]
  [./applied_load_fct]
    type = ParsedFunction
    value = 10*tanh(1e3*t)
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
  active = 'stress_yy mises_strain_rate stress_xz stress_xx stress_xy mises_stress stress_zz eqv_plastic_strain stress_yz'
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
  [./stress_xz]
    type = RankTwoAux
    rank_two_tensor = stress
    variable = stress_xz
    index_i = 0
    index_j = 2
  [../]
  [./stress_yy]
    type = RankTwoAux
    rank_two_tensor = stress
    variable = stress_yy
    index_i = 1
    index_j = 1
  [../]
  [./stress_yz]
    type = RankTwoAux
    rank_two_tensor = stress
    variable = stress_yz
    index_i = 1
    index_j = 2
  [../]
  [./stress_zz]
    type = RankTwoAux
    rank_two_tensor = stress
    variable = stress_zz
    index_i = 2
    index_j = 2
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
[]

[Postprocessors]
  # [./p0]
  # type = PointValue
  # point = '0 0 0'
  # variable = pore_pressure
  # [../]
  [./stress_xx]
    type = PointValue
    point = '0 0 0'
    variable = stress_xx
  [../]
  [./stress_yy]
    type = PointValue
    point = '0 0 0'
    variable = stress_yy
  [../]
  [./stress_zz]
    type = PointValue
    point = '0 0 0'
    variable = stress_zz
  [../]
  [./ydisp]
    type = PointValue
    variable = disp_y
    point = '0 0 0'
  [../]
[]

[Preconditioning]
  active = ''
  [./SMP]
    # petsc_options_iname = '-ksp_type -pc_type -snes_atol -snes_rtol -snes_max_it'
    # petsc_options_value = 'bcgs bjacobi 1E-14 1E-10 10000'
    #
    petsc_options = '-snes_monitor -snes_linesearch_monitor -ksp_monitor'
    petsc_options_iname = '-ksp_type -pc_type -snes_atol -snes_rtol -snes_max_it' # -ksp_type -pc_type -snes_atol -snes_rtol -snes_max_it -ksp_max_it -sub_pc_type -sub_pc_factor_shift_type
    petsc_options_value = 'bcgs bjacobi 1E-14 1E-10 10000' # gmres asm 1E0 1E-6 200 500 lu NONZERO
    type = SMP
    full = true
  [../]
[]

[Executioner]
  type = Transient
  num_steps = 100
  solve_type = PJFNK
  end_time = 10
  dt = 1e-5
  petsc_options_iname = '-ksp_type -pc_type -sub_pc_type -ksp_gmres_restart'
  petsc_options_value = 'gmres asm lu 201'
  nl_abs_tol = 1e-10
  nl_rel_step_tol = 1e-10
  nl_rel_tol = 1e-06
  nl_abs_step_tol = 1e-10
[]

[Outputs]
  file_base = bench_J2_footing_2D_damage_out
  [./my_console]
    output_linear = true
    type = Console
    output_nonlinear = true
  [../]
  [./my_exodus]
    scalar_as_nodal = true
    type = Exodus
    elemental_as_nodal = true
  [../]
[]

[RedbackMechAction]
  [./solid]
    disp_x = disp_x
    disp_y = disp_y
  [../]
[]

[ICs]
  [./damage_ic]
    variable = damage
    type = ConstantIC
    value = 0
  [../]
[]

