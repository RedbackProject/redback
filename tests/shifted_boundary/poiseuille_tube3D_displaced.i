[GlobalParams]
  gravity = '0 0 0'
  laplace = true
  integrate_p_by_parts = false
  pspg = true
  alpha = 1e-1
  convective_term = false
  use_displaced_mesh = true
[]

[Mesh]
  boundary_name = tube
  boundary_id = 10
  displacements = 'dist_x dist_y dist_z'
  [./mesh]
    type = GeneratedMeshGenerator
    dim = 3
    nx = 30
    ny = 30
    nz = 2
    zmax = 0.2
    elem_type = HEX
  [../]
  [./image]
    type = ImageSubdomainGenerator
    file = tube/tube.png
    threshold = 100
    input = mesh
  [../]
  [./interface]
    type = SideSetsBetweenSubdomainsGenerator
    primary_block = 0
    new_boundary = 10
    paired_block = 1
    input = image
  [../]
  [./delete]
    type = BlockDeletionGenerator
    input = interface
    block = 1
  [../]
[]

[Variables]
  [./vel_x]
  [../]
  [./vel_y]
  [../]
  [./vel_z]
  [../]
  [./p]
  [../]
[]

[Kernels]
  [./mass]
    type = INSMass
    variable = p
    u = vel_x
    v = vel_y
    w = vel_z
    pressure = p
  [../]
  [./x_momentum_space]
    type = INSMomentumLaplaceForm
    variable = vel_x
    u = vel_x
    v = vel_y
    w = vel_z
    pressure = p
    component = 0
  [../]
  [./y_momentum_space]
    type = INSMomentumLaplaceForm
    variable = vel_y
    u = vel_x
    v = vel_y
    w = vel_z
    pressure = p
    component = 1
  [../]
  [./z_momentum_space]
    type = INSMomentumLaplaceForm
    variable = vel_z
    u = vel_x
    v = vel_y
    w = vel_z
    pressure = p
    component = 2
  [../]
[]

[AuxVariables]
  [./d]
    family = LAGRANGE_VEC
  [../]
  [./dist_x]
  [../]
  [./dist_y]
  [../]
  [./dist_z]
  [../]
[]

[AuxKernels]
  [./distance]
    type = DistanceToInterfaceAux
    stl_name = "tube.stl"
    execute_on = 'INITIAL'
    variable = d
    boundary = tube
  [../]
  [./distance_x]
    type = VectorVariableComponentAux
    variable = dist_x
    vector_variable = d
    component = x
    execute_on = TIMESTEP_BEGIN
  [../]
  [./distance_y]
    type = VectorVariableComponentAux
    variable = dist_y
    vector_variable = d
    component = y
    execute_on = TIMESTEP_BEGIN
  [../]
  [./distance_z]
    type = VectorVariableComponentAux
    variable = dist_z
    vector_variable = d
    component = z
    execute_on = TIMESTEP_BEGIN
  [../]
[]

[Materials]
  [./const]
    type = GenericConstantMaterial
    prop_names = 'rho mu'
    prop_values = '1  1'
  [../]
[]

[Functions]
  [linear_p]
    type = ParsedFunction
    expression = '1-5*z'
  []
[]

[ICs]
  [linear_p]
    type = FunctionIC
    function = linear_p
    variable = p
  []
[]

[BCs]
  [./x_no_slip]
    type = DirichletBC
    variable = vel_x
    boundary = 'tube back front'
    value = 0.0
  [../]
  [./y_no_slip]
    type = DirichletBC
    variable = vel_y
    boundary = 'tube back front'
    value = 0.0
  [../]
  [./z_no_slip]
    type = DirichletBC
    variable = vel_z
    boundary = 'tube'
    value = 0.0
  [../]
  [./inlet]
    type = DirichletBC
    variable = p
    boundary = back
    value = 1
  [../]
  # [./inlet]
  #   type = NeumannBC
  #   variable = vel_z
  #   boundary = 'back'
  #   value = 1
  # [../]
  [./outlet]
    type = DirichletBC
    variable = p
    boundary = front
    value = 0
  [../]
[]

[Postprocessors]
  [./vol]
    type = VolumePostprocessor
    execute_on = 'INITIAL TIMESTEP_END'
  [../]
  [./area]
    type = AreaPostprocessor
    execute_on = 'INITIAL TIMESTEP_END'
    boundary = tube
  [../]
  [./integral_in]
    type = SideIntegralVariablePostprocessor
    variable = vel_z
    boundary = back
  [../]
  [./integral_out]
    type = SideIntegralVariablePostprocessor
    variable = vel_z
    boundary = front
  [../]
  [./vel_z_integral]
    type = ElementIntegralVariablePostprocessor
    variable = vel_z
  [../]
  [./vel_z_max]
    type = ElementExtremeValue
    value_type = max
    variable = vel_z
  [../]
[]

[Executioner]
  type = Steady
  solve_type = 'NEWTON'
  line_search = 'bt'
[]

[Preconditioning]
  [./FSP]
    type = FSP
    petsc_options = '-ksp_converged_reason -snes_converged_reason'
    petsc_options_iname = '-snes_type -ksp_type -ksp_rtol -ksp_atol -ksp_max_it -snes_atol -snes_rtol -snes_max_it -snes_max_funcs'
    petsc_options_value = 'newtonls     fgmres     1e-2     1e-15       200       1e-10        1e-15       200           100000'
    topsplit = 'uv'
    [./uv]
      petsc_options_iname = '-pc_fieldsplit_schur_fact_type -pc_fieldsplit_schur_precondition'
      petsc_options_value = 'upper selfp'
      splitting = 'u v'
      splitting_type = schur
    [../]
    [./u]
      vars = 'vel_x vel_y vel_z'
      petsc_options_iname = '-pc_type -ksp_type -pc_hypre_type'
      petsc_options_value = '  hypre    preonly     boomeramg '
    [../]
    [./v]
      vars = 'p'
      petsc_options_iname = '-pc_type -ksp_type -sub_pc_type -sub_pc_factor_levels'
      petsc_options_value = '  jacobi  preonly        ilu            3'
    [../]
  [../]
[]

[Outputs]
  file_base = poiseuille_tube3D_displaced
  csv = true
[]
