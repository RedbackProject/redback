# A thermal loading of an elastic rod
#
[Mesh]
  type = GeneratedMesh
  dim = 3
  xmin = -0.5
  xmax = 0.5
  ymin = -0.5
  ymax = 0.5
  zmin = -5
  zmax = 5
[]

[Variables]
  active = 'temp disp_z disp_y disp_x'
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

[GlobalParams]
  time_factor = 1
[]

[Materials]
  [./mat_mech]
    type = RedbackMechMaterialElastic
    block = 0
    disp_x = disp_x
    disp_y = disp_y
    exponent = 0
    youngs_modulus = 3.6
    poisson_ratio = 0.2
    ref_pe_rate = 0
    yield_stress = '0. 1 1. 1'
    total_porosity = 0.1
    disp_z = disp_z
    temperature = temp
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
    gr = 0 # exp(-Ar), Ar=10
    phi0 = 0.1
    ref_lewis_nb = 1
    total_porosity = 0.1
    Peclet_number = 10
    solid_density = 0
    disp_z = disp_z
    confining_pressure = 0
    delta = 0
    is_mechanics_on = true
    fluid_density = 0
    eta2 = 0
    solid_compressibility = 3.7037 # 1/(0.9*0.3)
    solid_thermal_expansion = 0.005
    temperature = temp
  [../]
[]

[BCs]
  active = 'basefixed_z confinex confiney temperature'
  [./confinex]
    type = DirichletBC
    variable = disp_x
    boundary = 'left right'
    value = 0
  [../]
  [./confiney]
    type = DirichletBC
    variable = disp_y
    value = 0
    boundary = 'bottom top'
  [../]
  [./top_velocity]
    type = FunctionDirichletBC
    variable = disp_z
    function = -0.01*t
    boundary = front
  [../]
  [./temperature]
    type = FunctionDirichletBC
    variable = temp
    boundary = back
    function = 1
  [../]
  [./basefixed_z]
    type = DirichletBC
    variable = disp_z
    boundary = back
    value = 0
  [../]
  [./basedfixed_y]
    type = DirichletBC
    variable = disp_y
    boundary = back
    value = 0
  [../]
  [./basefixed_x]
    type = DirichletBC
    variable = disp_x
    boundary = back
    value = 0
  [../]
[]

[AuxVariables]
  active = 'stress_yy stress_xz stress_xx stress_xy stress_zz stress_yz'
  [./total_porosity]
    order = FIRST
    family = MONOMIAL
  [../]
  [./strain_zz]
    order = CONSTANT
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

[Kernels]
  active = 'temp_diff td_temp'
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
  [./strain_zz]
    type = RankTwoAux
    rank_two_tensor = total_strain
    variable = strain_zz
    index_i = 2
    index_j = 2
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
  active = 'temperature stress_yy stress_xx zdisp stress_zz'
  [./p0]
    type = PointValue
    point = '0 0 0'
    variable = pore_pressure
  [../]
  [./zdisp]
    type = PointValue
    point = '0 0 0.5'
    variable = disp_z
  [../]
  [./strain_zz]
    type = PointValue
    point = '0 0 0'
    variable = strain_zz
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
  [./temperature]
    type = PointValue
    variable = temp
    point = '0 0 0'
  [../]
[]

[Preconditioning]
  [./SMP]
    type = SMP
    full = true
    petsc_options_iname = '-ksp_type -pc_type -snes_atol -snes_rtol -snes_max_it'
    petsc_options_value = 'bcgs bjacobi 1E-14 1E-10 10000'
  [../]
[]

[Executioner]
  type = Transient
  solve_type = Newton
  start_time = 0
  end_time = 10
  dt = 1
[]

[Outputs]
  exodus = true
  execute_on = timestep_end
  file_base = bench_TM_elastic
  csv = true
[]

[RedbackMechAction]
  [./solid]
    disp_x = disp_x
    disp_y = disp_y
    disp_z = disp_z
  [../]
[]

