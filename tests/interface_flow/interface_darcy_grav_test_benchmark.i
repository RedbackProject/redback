gravity = '0.56 0.98 0'

[Mesh]
  type = FileMesh
  file = angled_finite_interface.msh
[]

[Variables]
  [p]
    order = FIRST
    family = LAGRANGE
  []
[]

[Kernels]
  [diff]
    type = RedbackMassDiffusion
    variable = p
  []
[]

[Materials]
  [mat]
    type = RedbackMaterial
    block = 'left_block right_block'
    gravity = ${gravity}
    ref_lewis_nb = 1
  []
  [mat_frac]
    type = RedbackMaterial
    block = 'middle_block'
    ref_lewis_nb = 10
    gravity = ${gravity}
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
    variable = 'p'
    boundary = 'fault_left'
  []
  [p_right]
    type = SideAverageValue
    variable = 'p'
    boundary = 'fault_right'
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
[]

[Outputs]
  file_base = interface_darcy_grav_test_benchmark
  csv = true
[]
