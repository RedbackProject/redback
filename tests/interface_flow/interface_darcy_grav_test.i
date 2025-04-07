gravity = '0.56 0.98 0'

[Mesh]
  [file]
    type = FileMeshGenerator
    file = angled_interface.msh
  []
  [break]
    type = BreakMeshByBlockGenerator
    input = file
    split_interface = false
  []
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
    gravity = ${gravity}
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
  [p_left]
    type = SideAverageValue
    variable = p
    boundary = interface
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
  file_base = interface_darcy_grav_test
  csv = true
[]
