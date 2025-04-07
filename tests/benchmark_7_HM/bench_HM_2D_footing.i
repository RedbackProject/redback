# Footing problem (extruded in 3D)

[Mesh]
  type = FileMesh
  file = ../../meshes/2d_footing_pb.msh
  boundary_name = 'bottom right top_no_pressure top_pressure left'
  boundary_id = '0 1 2 3 4'
[]

[Variables]
  active = 'pore_pressure disp_y disp_x'
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
[]

[GlobalParams]
  time_factor = 10
[]

[Materials]
  [./mat_mech]
    type = RedbackMechMaterialElastic
    block = 0
    disp_x = disp_x
    disp_y = disp_y
    pore_pres = pore_pressure
    exponent = 0
    youngs_modulus = 3.6
    poisson_ratio = 0.2
    ref_pe_rate = 0
    yield_stress = '0. 1 1. 1'
    total_porosity = 0.1
  [../]
  [./mat_nomech]
    type = RedbackMaterial
    block = 0
    disp_x = disp_x
    disp_y = disp_y
    pore_pres = pore_pressure
    Aphi = 0
    ar = 0
    ar_F = 0
    ar_R = 0
    eta1 = 0
    gr = 0 # exp(-Ar), Ar=10
    phi0 = 0.1
    ref_lewis_nb = 1
    total_porosity = 0.1
    Peclet_number = 1e-6
    solid_density = 0
    confining_pressure = 0
    delta = 0
    is_mechanics_on = true
    fluid_density = 0
    eta2 = 0
    solid_compressibility = 3.7037 # 1/(0.9*0.3)
  [../]
[]

[BCs]
  active = 'Pressure confine_x confine_y pore_pressure_top'
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
  active = 'stress_yy stress_xz stress_xx stress_xy stress_zz stress_yz'
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
[]

[Functions]
  [./applied_load_fct]
    type = ConstantFunction
    value = 1e-1
  [../]
[]

[Kernels]
  active = 'td_press poromech press_diff'
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
  [../]
  [./chem_press]
    type = RedbackChemPressure
    variable = pore_pressure
    block = 0
  [../]
  [./thermal_pressurization]
    type = RedbackThermalPressurization
    variable = pore_pressure
    temperature = temp
  [../]
  [./poromech]
    type = RedbackPoromechanics
    variable = pore_pressure
  [../]
[]

[AuxKernels]
  active = 'stress_yy stress_xz stress_xx stress_xy stress_zz stress_yz'
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
[]

[Postprocessors]
  [./p0]
    type = PointValue
    point = '0 0 0'
    variable = pore_pressure
  [../]
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
  [./SMP]
    #     petsc_options_iname = '-ksp_type -pc_type -snes_atol -snes_rtol -snes_max_it'
    #     petsc_options_value = 'bcgs bjacobi 1E-14 1E-10 10000'
    #
    type = SMP
    full = true
  [../]
[]

[Executioner]
  type = Transient
  num_steps = 50
  solve_type = Newton
  end_time = 10
  dt = 1e-4
  petsc_options_iname = '-ksp_type -pc_type -sub_pc_type -ksp_gmres_restart'
  petsc_options_value = 'gmres asm lu 201'
[]

[Outputs]
  [./my_console]
    output_linear = true
    type = Console
    output_nonlinear = true
  [../]
  [./my_exodus]
    scalar_as_nodal = true
    file_base = footing_2D
    type = Exodus
    elemental_as_nodal = true
  [../]
[]

[RedbackMechAction]
  [./solid]
    disp_x = disp_x
    disp_y = disp_y
    pore_pres = pore_pressure
  [../]
[]

