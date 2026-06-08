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

[Kernels]
  active = 'td_temp diff_temp mh_temp chem_endo'
  [./td_temp]
    type = TimeDerivative
    variable = temp
  [../]
  [./diff_temp]
    type = Diffusion
    variable = temp
  [../]
  [./mh_temp]
    type = RedbackMechDissip
    variable = temp
  [../]
  [./chem_endo]
    type = RedbackChemEndo
    variable = temp
  [../]
  [./Chem_exo]
    type = RedbackChemExo
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
    Aphi = 0
    ar = 10
    ar_F = 20
    ar_R = 10
    da_endo = 1
    gr = 9.08e-5 # 2*exp(-Ar), Ar=10
    is_mechanics_on = false
    is_chemistry_on = true
    alpha_2 = 1
    mu = 1e-3
    ref_lewis_nb = 1
    temperature = temp
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
  end_time = 0.2
  dtmax = 0.1
  scheme = bdf2
  [./TimeStepper]
    type = ConstantDT
    dt = 1e-2
  [../]
[]

[Outputs]
  exodus = true
  console = true
  execute_on = TIMESTEP_END
  file_base = bench_TC_out
[]

[ICs]
  [./temp_ic]
    variable = temp
    value = 0
    type = ConstantIC
    block = 0
  [../]
[]

