[Mesh]
  type = GeneratedMesh
  dim = 1
  nx = 10
  ny = 5
  nz = 5
  xmin = -1
[]

[Variables]
  active = 'temp'
  [./temp]
  [../]
  [./disp_x]
    order = CONSTANT
    family = MONOMIAL
    block = 0
  [../]
  [./disp_y]
    order = CONSTANT
    family = MONOMIAL
    block = 0
  [../]
[]

[AuxVariables]
  [./total_porosity]
    order = FIRST
    family = MONOMIAL
  [../]
[]

[AuxKernels]
  [./total_porosity]
    type = RedbackTotalPorosityAux
    variable = total_porosity
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
  [./mh_temp]
    type = RedbackMechDissip
    variable = temp
  [../]
[]

[BCs]
  active = 'left_temp right_temp'
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
  [./disp_y]
    type = DirichletBC
    variable = disp_y
    boundary = 'left right'
    value = 0
  [../]
  [./disp_x_left]
    type = DirichletBC
    variable = disp_x
    boundary = left
    value = 1
  [../]
  [./disp_x_rigth]
    type = DirichletBC
    variable = disp_x
    boundary = right
    value = 0
  [../]
[]

[Materials]
  [./adim_rock]
    type = RedbackMaterial
    block = 0
    alpha_2 = 1
    ar = 10
    # yield_stress = '0 1 1 1'
    gr = 4.313e-6 # 0.095*exp(-Ar), Ar=10
    pore_pres = 0
    temperature = temp
    is_mechanics_on = false
    ref_lewis_nb = 1
    ar_F = 40
    ar_R = 1
    Aphi = 0
    phi0 = 0.1
    da_endo = 1
    total_porosity = total_porosity
  [../]
[]

[Postprocessors]
  active = 'middle_temp'
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
  end_time = 5
  dtmax = 0.2
  scheme = bdf2
  [./TimeStepper]
    type = ConstantDT
    dt = 1
  [../]
[]

[Outputs]
  file_base = bench1_c_out
  exodus = true
  csv = true
  execute_on = TIMESTEP_END
  console = true
[]

[ICs]
  [./temp_ic]
    variable = temp
    value = 0.25
    type = ConstantIC
    block = 0
  [../]
[]
