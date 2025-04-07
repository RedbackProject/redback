[Mesh]
  [./generate]
    type = GeneratedMeshGenerator
    dim = 2
    nx = 10
    ny = 5
    nz = 5
    xmin = -1
  [../]
  [./subdomain]
    type = SubdomainBoundingBoxGenerator
    input = generate
    block_id = 1
    top_right = '1 0.5 1'
    bottom_left = '-1 0 0'
  [../]
[]

[Variables]
  inactive = 'disp_x disp_y'
  [./temp]
  [../]
  [./disp_x]
    order = CONSTANT
    family = MONOMIAL
    block = '0'
  [../]
  [./disp_y]
    order = CONSTANT
    family = MONOMIAL
    block = '0'
  [../]
[]

[AuxVariables]
  [./total_porosity]
    order = FIRST
    family = MONOMIAL
  [../]
[]

[Kernels]
  [./td_temp]
    type = TimeDerivative
    variable = temp
  [../]
  [./diff_temp]
    type = RedbackThermalDiffusion
    variable = temp
  [../]
[]

[AuxKernels]
  [./total_porosity]
    type = RedbackTotalPorosityAux
    variable = total_porosity
  [../]
[]

[BCs]
  inactive = 'disp_y disp_x_left disp_x_rigth'
  [./left_temp]
    type = DirichletBC
    variable = temp
    boundary = 'left'
    value = 1
  [../]
  [./right_temp]
    type = DirichletBC
    variable = temp
    boundary = 'right'
    value = 0
  [../]
  [./disp_y]
    type = DirichletBC
    variable = disp_y
    boundary = 'left right'
    value = 0
  [../]
  [./disp_x_left]
    type = DirichletBC
    variable = disp_x
    boundary = 'left'
    value = 1
  [../]
  [./disp_x_rigth]
    type = DirichletBC
    variable = disp_x
    boundary = 'right'
    value = 0
  [../]
[]

[Materials]
  [./zone0]
    type = RedbackMaterial
    block = '0'
    alpha_2 = 1
    ar = 10
    gr = 4.313e-6    # 0.095*exp(-Ar), Ar=10
    pore_pres = '0'
    temperature = 'temp'
    ref_lewis_nb = 1
    ar_F = 40
    ar_R = 1
    Aphi = 0
    phi0 = 0.1
    da_endo = 1
    total_porosity = 'total_porosity'
  [../]
  [./1]
    type = RedbackMaterial
    gr = 4.313e-6
    thermal_diffusivity = 2
    ar = 10
    alpha_2 = 1
    da_endo = 1
    pore_pres = '0'
    total_porosity = 'total_porosity'
    ref_lewis_nb = 1
    Aphi = 0
    ar_F = 40
    phi0 = 0.1
    ar_R = 1
    temperature = 'temp'
    block = '1'
  [../]
[]

[Postprocessors]
  inactive = 'strain'
  [./middle_temp]
    type = PointValue
    variable = temp
    point = '0 0 0'
  [../]
  [./strain]
    type = StrainRatePoint
    variable = 'temp'
    point = '0 0 0'
  [../]
[]

[Executioner]
  type = Transient
  num_steps = 10000
  petsc_options_iname = '-pc_type -pc_hypre_type'
  petsc_options_value = 'hypre boomeramg'
  ss_check_tol = 1e-6
  end_time = 0.1
  dtmax = 0.1
  scheme = bdf2
  dt = 0.01
  inactive = 'TimeStepper'
  [./TimeStepper]
    type = ConstantDT
    dt = 0.1
  [../]
[]

[Outputs]
  file_base = diffusion_2_zones
  exodus = true
  csv = true
  execute_on = 'TIMESTEP_END INITIAL'
  console = true
[]

[ICs]
  [./temp_ic]
    type = ConstantIC
    variable = temp
    value = 0
    block = '0'
  [../]
[]
