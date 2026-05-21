[GlobalParams]
  ar = 12
  gr = 1
[]

[Mesh]
  type = GeneratedMesh
  dim = 1
  nx = 100
  ny = 5
  nz = 5
  xmin = -1
[]

[Variables]
  [./pressure]
  [../]
  [./temp]
  [../]
[]

[AuxVariables]
  [./strain_rate]
    family = MONOMIAL
    order = CONSTANT
  [../]
[]

[Kernels]
  [./td_pressure]
    type = TimeDerivative
    variable = pressure
  [../]
  [./diff_pressure]
    type = CoefDiffusion
    variable = pressure
    coef = 0.25
  [../]
  [./td_temp]
    type = TimeDerivative
    variable = temp
  [../]
  [./diff_temp]
    type = Diffusion
    variable = temp
  [../]
  [./mh_temp]
    type = MechDissip
    variable = temp
  [../]
  [./cp_press]
    type = ChemPressure
    variable = pressure
    ar_c = 40
    temp = temp
    mu = 1e-3
    delta = 1e-3
  [../]
  [./ce_temp]
    type = ChemEndo
    variable = temp
    ar_c = 40
    da = 1
    delta = 1e-3
  [../]
[]

[AuxKernels]
  [./strain_rate]
    type = StrainRateAux
    variable = strain_rate
    execute_on = timestep
    temp = temp
  [../]
[]

[BCs]
  [./left_pressure]
    type = DirichletBC
    variable = pressure
    boundary = left
    value = 0
  [../]
  [./right_pressure]
    type = DirichletBC
    variable = pressure
    boundary = right
    value = 0
  [../]
  [./left_temp]
    type = DirichletBC
    variable = temp
    boundary = left
    value = 0
  [../]
  [./right_temp]
    type = DirichletBC
    variable = temp
    boundary = right
    value = 0
  [../]
[]

[Materials]
  [./adim_rock]
    type = RedbackMaterial
    block = 0
    ar_c = 40
    alpha_2 = 3
    da = 1
    mu = 1e-3
    ar = 20
    gr = 1.2e-7
    delta = 1e-3
    disp_y = 0
    disp_x = 0
    pore_pres = pressure
    temperature = temp
    disp_z = 0
  total_porosity = porosity
  [../]
[]

[Postprocessors]
  [./middle_temp]
    type = PointValue
    variable = temp
    point = '0 0 0'
  [../]
  [./strain]
    type = StrainRatePoint
    variable = temp
    point = '0 0 0'
  [../]
[]

[Executioner]
  type = Transient
  num_steps = 10000
  petsc_options_iname = '-pc_type -pc_hypre_type'
  petsc_options_value = 'hypre boomeramg'
  end_time = 15
  dtmax = 0.1
  scheme = bdf2
  [./TimeStepper]
    type = ConstantDT
    dt = 0.1
  [../]
[]

[Outputs]
  exodus = true
  console = true
[]
