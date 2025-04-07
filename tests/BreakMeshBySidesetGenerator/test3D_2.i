#  Trap charging in  3D on simplified geometry
[Mesh]
  [fmg]
    type = FileMeshGenerator
    file = 3Dcase2.msh
  []
  [./breakmesh]
    type = BreakMeshBySidesetGenerator
    input = fmg
    boundaries = 'top bottom left right back front'
    sidesets =  'fault_vertical'
    split_interface = false
    create_lower_D_blocks = true
    connect_T_junctions = false
    verbose = true
  []
  [bottom_block1]
    type = BreakBoundaryOnSubdomainGenerator
    boundaries = 'bottom'
    input = breakmesh
  []
[]

[Variables]
  [dummy_var]
  []
[]

[AuxVariables]
  [lewis_fault1_trans]
    initial_condition = '10000' # 1/D=1/1e-2
  []
[]

[AuxKernels]
  [fault_evolution]
    type = FunctionAux
    function = 'if(t>0.05, 1, 10000)'
    variable = lewis_fault1_trans
  []
[]

[Kernels]
  [dT_dt_matrix]
    type = TimeDerivative
    variable = dummy_var
    block = 'matrix'
  []
  [dT_dt_fracs]
    type = CoefTimeDerivative
    variable = dummy_var
    block = 'lowerD_fault_vertical'
    Coefficient = 5e-4  # a/2 (half thickness of fault)
  []
  [diff_matrix]
    type = CoeffParamDiffusion
    variable = dummy_var
    D = 0.0001
    block = 'matrix'
  []
  [diff_reservoir]
    type = CoeffParamDiffusion
    variable = dummy_var
    D = 1
    block = 'block1 block2'
  []
  [diffT_frac1]
    type = CoeffParamDiffusion
    variable = dummy_var
    D = 5e-2 # D*a/2
    block = 'lowerD_fault_vertical'
  []
[]

[InterfaceKernels]
  [interface1]
    type = InterfaceDarcy
    variable = dummy_var
    neighbor_var = dummy_var
    boundary = 'interface'
    fault_lewis_number = lewis_fault1_trans
    fault_thickness = 0.001 # a
  []
[]

[Materials]
  [matrix]
    type = RedbackMaterial
    # just to get fluid_gravity_term
  []
[]

[BCs]
  [left]
    type = DirichletBC
    variable = dummy_var
    boundary = 'bottom_to_block1'
    value = 1
  []
[]

[Executioner]
  type = Transient
  dt = 0.0002
  end_time = 0.1
  solve_type = NEWTON
  petsc_options_iname = '-pc_type -pc_factor_mat_solver_package'
  # petsc_options_value = ' lu       mumps'
  petsc_options_value = ' lu       superlu_dist'
[]

[Outputs]
  file_base = test3D_2
  exodus = true
  csv = true
[]

[Postprocessors]
  [top_reservoir]
    type = PointValue
    point = '0.6 0.8 0'
    variable = dummy_var
  []
[]
