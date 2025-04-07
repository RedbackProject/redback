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
  [bottom_corner]
    type = ExtraNodesetGenerator
    input = break
    new_boundary = '10'
    nodes = '11'
  []
[]

[Variables]
  [disp_x]
  []
  [disp_y]
  []
[]

[AuxVariables]
  [stress_yy]
    family = MONOMIAL
    order = CONSTANT
  []
[]

[AuxKernels]
  [stress_yy]
    type = RankTwoAux
    rank_two_tensor = stress
    index_j = 1
    index_i = 1
    variable = stress_yy
  []
[]

[InterfaceKernels]
  [interfacex]
    type = InterfaceDisp
    variable = disp_x
    neighbor_var = disp_x
    penalty = 1e6
    boundary = interface
    tangent_jump = 2e-5
    component = 0
  []
  [interfacey]
    type = InterfaceDisp
    variable = disp_y
    neighbor_var = disp_y
    penalty = 1e6
    boundary = interface
    tangent_jump = 2e-5
    component = 1
  []
[]

[Kernels]
  [disp_y]
    type = StressDivergenceTensors
    component = 1
    variable = disp_y
    displacements = 'disp_x disp_y'
  []
  [disp_x]
    type = StressDivergenceTensors
    component = 0
    variable = disp_x
    displacements = 'disp_x disp_y'
  []
[]

[Materials]
  [Elasticity_tensor]
    type = ComputeIsotropicElasticityTensor
    poissons_ratio = 0.3
    youngs_modulus = 10000
  []
  [el]
    type = ComputeFiniteStrainElasticStress
  []
  [finite_strain]
    type = ComputePlaneFiniteStrain
    displacements = 'disp_x disp_y'
  []
[]

[Functions]
  [loading_vel]
    type = ParsedFunction
    value = '-0.0002*t'
  []
[]

[BCs]
  [uy_top]
    type = FunctionDirichletBC
    variable = disp_y
    boundary = 'top'
    function = loading_vel
  []
  [no_disp_y]
    type = DirichletBC
    variable = disp_y
    boundary = 'bottom'
    value = 0.0
  []
  [no_disp_x]
    boundary = 'top_left_corner'
    type = DirichletBC
    variable = disp_x
    value = 0.0
  []
[]

[Preconditioning]
  [SMP]
    type = SMP
    full = true
  []
[]

[Executioner]
  type = Transient
  dt = 0.1
  l_max_its = 50
  nl_max_its = 10
  solve_type = NEWTON
  petsc_options_iname = '-pc_type -pc_hypre_type -snes_linesearch_type -ksp_gmres_restart'
  petsc_options_value = 'hypre boomeramg cp 201'
  nl_abs_tol = 1e-8
  nl_rel_tol = 1e-8
  reset_dt = true
  line_search = basic
  start_time = 0.0
  end_time = 0.2
[]

[Outputs]
  exodus = true
  file_base = compression_angled_interface
[]
