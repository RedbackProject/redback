[Mesh]
  [./mesh]
    type = GeneratedMeshGenerator
    dim = 3
    nx = 10
    ny = 8
    xmax = 2
    zmax = 0.1
  [../]
[]

[Variables]
  [./temperature]
  [../]
  [./disp_x]
  [../]
  [./disp_y]
  [../]
  [./disp_z]
  [../]
[]

[Functions]
  [./top_bc]
    type = ParsedFunction
    value = 0.01*t
  [../]
[]

[Kernels]
  [./dT_dt]
    type = TimeDerivative
    variable = temperature
  [../]
  [./temp_diff]
    type = RedbackThermalDiffusion
    variable = temperature
  [../]
  [./mech_dissip]
    type = RedbackMechDissip
    variable = temperature
  [../]
[]

[BCs]
  [./Periodic]
    [./periodic_x]
      auto_direction = x
    [../]
  [../]
  [./ux_top]
    type = FunctionDirichletBC
    variable = disp_x
    boundary = top
    function = top_bc
  [../]
  [./ux_bottom]
    type = DirichletBC
    variable = disp_x
    boundary = bottom
    value = 0
  [../]
  [./uy_top]
    type = DirichletBC
    variable = disp_y
    boundary = top
    value = 0
  [../]
  [./uy_bottom]
    type = DirichletBC
    variable = disp_y
    boundary = bottom
    value = 0
  [../]
  [./uz_back]
    type = DirichletBC
    variable = disp_z
    boundary = back
    value = 0
  [../]
  [./temp_top_bottom]
    type = DirichletBC
    variable = temperature
    boundary = 'top bottom'
    value = 0
  [../]
[]

[Materials]
  [./no_mech_material]
    type = RedbackMaterial
    ar = 5
    temperature = temperature
  [../]
  [./mech_material]
    type = RedbackMechMaterialJ2
    disp_x = disp_x
    disp_y = disp_y
    disp_z = disp_z
    outputs = all
    youngs_modulus = 100
    poisson_ratio = 0.3
    yield_stress = '0 1 0.1 0.1'
    temperature = temperature
  [../]
[]

[Preconditioning]
  [./SMP]
    type = SMP
    full = true
    solve_type = PJFNK
    petsc_options_iname = '-ksp_type -pc_type -sub_pc_type -ksp_gmres_restart'
    petsc_options_value = 'gmres asm lu 201'
  [../]
[]

[Executioner]
  type = Transient
  num_steps = 200
  [./TimeStepper]
    type = ConstantDT
    dt = 5e-2
  [../]
[]

[Outputs]
  exodus = true
  file_base = simple_shear_TM
  perf_graph = true
[]

[RedbackMechAction]
  [./my_action]
    disp_x = disp_x
    disp_y = disp_y
    disp_z = disp_z
    temp = temperature
  [../]
[]

