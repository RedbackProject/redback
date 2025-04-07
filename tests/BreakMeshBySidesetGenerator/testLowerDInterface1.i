[Mesh]
  inactive = 'bmbbg'
  [fmg]
    type = FileMeshGenerator
    file = fractureX2.msh
  []
  [bmbbg]
    type = BreakMeshByBlockGenerator
    input = fmg
  []
  [ifsg]
    type = BreakMeshBySidesetGenerator
    sidesets = 'ss2 ss3'
    create_lower_D_blocks = true
    boundaries = 'bottom top left right'
    input = fmg
    verbose = true
  []
  [x_top_left]
    type = ExtraNodesetGenerator
    new_boundary = '77'
    input = ifsg
    nodes = '10'
  []
  [x_top_left2]
    type = ExtraNodesetGenerator
    new_boundary = '78'
    input = x_top_left
    nodes = '35'
  []
  [x_bottom_right]
    type = ExtraNodesetGenerator
    new_boundary = '79'
    input = x_top_left2
    nodes = '14'
  []
  [x_bottom_right2]
    type = ExtraNodesetGenerator
    new_boundary = '80'
    input = x_bottom_right
    nodes = '36'
  []
[]

[Kernels]
  inactive = 'dT_dt'
  [dT_dt]
    type = TimeDerivative
    variable = T
  []
  [diffT_matrix]
    type = CoeffParamDiffusion
    variable = T
    D = 0.01
    block = 'blockA blockB blockC blockD'
  []
  [diffT_fracs]
    type = CoeffParamDiffusion
    variable = T
    D = 1
    block = 'lowerD_ss2  lowerD_ss3' # why do we have 2. Are they what they should be?
  []
[]

[BCs]
  inactive = 'X_top_left2 X_bottom_right1'
  [X_top_left1]
    type = DirichletBC
    variable = T
    boundary = '77'
    value = 1
  []
  [X_top_left2]
    type = DirichletBC
    variable = T
    boundary = '78'
    value = 1
  []
  [X_bottom_right1]
    type = DirichletBC
    variable = T
    boundary = '79'
    value = 1
  []
  [X_bottom_right2]
    type = DirichletBC
    variable = T
    boundary = '80'
    value = 1
  []
[]

[Executioner]
  # dt = 0.01
  # end_time = 1
  # inactive = 'TimeStepper'
  # [TimeStepper]
  # type = FunctionDT
  # function = timestep_fct
  # []
  type = Steady # Transient
[]

[Variables]
  [T]
  []
[]

[Outputs]
  exodus = true
  csv = true
  file_base = testLowerDInterface1
[]

[Functions]
  [timestep_fct]
    type = ParsedFunction
    vars = 'dt0 dt_max t_ramp' # start at dt0 and reach dt_max when t=t_ramp
    value = 'min(dt_max,  dt0+(dt_max-dt0)*t/t_ramp)'
    vals = '1e-4 5e-3 0.1'
  []
[]

[Postprocessors]
  inactive = 'dt'
  [dt]
    type = TimestepSize
  []
  [T_mid_left]
    type = PointValue
    point = '0 0.5 0'
    variable = T
  []
  [T_mid_right]
    type = PointValue
    point = '1 0.5 0'
    variable = T
  []
  [T_top_centre]
    type = PointValue
    point = '0.5 1 0'
    variable = T
  []
  [T_bottom_centre]
    type = PointValue
    point = '0.5 0 0'
    variable = T
  []
[]
