[Mesh]
  [./fmg]
    type = FileMeshGenerator
    file = fractureX.msh
  []

  [./breakmesh]
    type = BreakMeshBySidesetGenerator
    input = fmg
    boundaries = 'top bottom left right'
    sidesets =  'ss2 ss3'
    split_interface = false
    #create_lower_D_blocks = false
  []
[]

[Variables]
  [dymmy_var]
  []
[]

[Kernels]
  [diff]
    type = Diffusion
    variable = dymmy_var
  []
[]

[BCs]
  [left]
    type = DirichletBC
    variable = dymmy_var
    boundary = 'left'
    value = 1
  []
  [right]
    type = DirichletBC
    variable = dymmy_var
    boundary = 'right'
    value = 0
  []
[]

[Executioner]
  type = Steady
  solve_type = NEWTON
[]

[Outputs]
  file_base = testBreakMeshBySidesetDiffusion1
  exodus = true
[]
