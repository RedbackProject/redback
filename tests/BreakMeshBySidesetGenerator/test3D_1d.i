[Mesh]
  [./fmg]
    type = FileMeshGenerator
    file = 3Dcase1d_structured_2x2x2.msh
  []

  [./breakmesh]
    type = BreakMeshBySidesetGenerator
    input = fmg
    boundaries = 'top bottom left right back front'
    sidesets =  'fault1 fault2 fault3'
    split_interface = false
    create_lower_D_blocks = false
    connect_T_junctions = true
    verbose = true
  []
[]

[Variables]
  [dummy_var]
  []
[]

[AuxVariables]
  [lewis_fault1_trans]
    initial_condition = 100 #'0.1' # 1/D=1/10
  []
[]

[Kernels]
  # inactive = 'dT_dt_matrix dT_dt_fracs'
  # [dT_dt_matrix]
  #   type = TimeDerivative
  #   variable = dummy_var
  #   block = 'matrix'
  # []
  # [dT_dt_fracs]
  #   type = CoefTimeDerivative
  #   variable = dummy_var
  #   block = 'lowerD_fault1 lowerD_fault2 lowerD_fault3'
  #   Coefficient = 5e-3  # a/2 (half thickness of fault)
  # []
  [diff]
    type = CoeffParamDiffusion
    variable = dummy_var
    D = 1
    block = 'matrix'
  []
  # [diffT_frac1]
  #   type = CoeffParamDiffusion
  #   variable = dummy_var
  #   D = 5e-2 # D*a/2 = 10*1e-2/2
  #   block = 'lowerD_fault1 lowerD_fault2 lowerD_fault3'
  # []
[]

[InterfaceKernels]
  [interface1]
    type = InterfaceDarcy
    variable = dummy_var
    neighbor_var = dummy_var
    boundary = 'interface'
    fault_lewis_number = lewis_fault1_trans
    fault_thickness = 0.01 # a
  []
[]

[Materials]
  [matrix]
    type = RedbackMaterial
    # just to get fluid_gravity_term
  []
[]

[BCs]
  [high]
    type = DirichletBC
    variable = dummy_var
    boundary = 'back'
    value = 1
  []
  [low]
    type = DirichletBC
    variable = dummy_var
    boundary = 'front'
    value = 0
  []
[]

[Executioner]
  type = Steady
  solve_type = NEWTON
  # dt = 0.1
  # end_time = 1
  petsc_options_iname = '-pc_type -pc_factor_mat_solver_package'
  # petsc_options_value = ' lu       mumps'
  petsc_options_value = ' lu       superlu_dist'
[]

[Outputs]
  file_base = test3D_1d
  exodus = true
  csv = true
[]

[VectorPostprocessors]
  [sampler]
    type = LineValueSampler
    variable = 'dummy_var'
    start_point = '0 0 0'
    end_point = '1 1 1'
    num_points = 10
    sort_by = z
  []
[]
