[Mesh]
  [./file]
    type = FileMeshGenerator
    file = gold/interface_darcy_test_in.e
    # type = GeneratedMesh
    # dim = 2
    # nx = 4
    # xmax = 2
  [../]
  # [subdomain]
  #   type = SubdomainBoundingBox
  #   bottom_left = '0 0.5 0'
  #   block_id = 1
  #   top_right = '1 1 0'
  # []
  [./break]
    type = BreakMeshByBlockGenerator
    input = file
    split_interface = false
  [../]
[]

[Variables]
  [p]
  []
[]

[AuxVariables]
  [lewis_fault]
  []
[]

[Kernels]
  [diff]
    type = RedbackMassDiffusion
    variable = p
  []
[]

[InterfaceKernels]
  [interface]
    type = InterfaceDarcy
    variable = p
    neighbor_var = p
    boundary = interface
    fault_lewis_number = lewis_fault
    fault_thickness = 0.1
  []
[]

[Materials]
  [mat]
    type = RedbackMaterial
    ref_lewis_nb = 1
  []
[]

[ICs]
  [lewis_fault]
    type = ConstantIC
    variable = lewis_fault
    value = 10
  []
[]

[BCs]
  [left]
    type = DirichletBC
    variable = p
    boundary = 'left'
    value = 1
  []
  [right]
    type = DirichletBC
    variable = p
    boundary = 'right'
    value = 0
  []
[]

[Postprocessors]
  [p]
    type = NodalVariableValue
    nodeid = 5
    variable = p
  []
[]

[Preconditioning]
  [smp]
    type = SMP
    full = true
  []
[]

[Executioner]
  type = Steady
  solve_type = NEWTON
[]

[Outputs]
  file_base = interface_darcy_test
  csv = true
[]
