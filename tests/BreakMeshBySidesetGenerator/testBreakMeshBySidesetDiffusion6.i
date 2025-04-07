[Mesh]
  [fmg]
    type = FileMeshGenerator
    file = fractureXsplit.msh
  []
  [breakmesh]
    # create_lower_D_blocks = false
    type = BreakMeshBySidesetGenerator
    input = fmg
    boundaries = 'top bottom left right'
    sidesets = 'ss1 ss3'
    split_interface = false
    connect_T_junctions = false
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
[]

[Outputs]
  file_base = testBreakMeshBySidesetDiffusion6
  exodus = true
[]
