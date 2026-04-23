[Mesh]
  type = GeneratedMesh
  dim = 1
  nx = 200
  xmin = 0.0
  xmax = 10.0
  displacements = 'disp_x disp_y disp_z'
[]

[Variables]
  [disp_x]
    order = FIRST
    family = LAGRANGE
  []
  [disp_y]
    order = FIRST
    family = LAGRANGE
  []
  [disp_z]
    order = FIRST
    family = LAGRANGE
  []
  [rot_x]
    order = FIRST
    family = LAGRANGE
  []
  [rot_y]
    order = FIRST
    family = LAGRANGE
  []
  [rot_z]
    order = FIRST
    family = LAGRANGE
  []
[]

[BCs]
  [fix_disp_x]
    type = DirichletBC
    variable = disp_x
    boundary = left
    value = 0.0
  []
  [fix_disp_y]
    type = DirichletBC
    variable = disp_y
    boundary = left
    value = 0.0
  []
  [fix_disp_z]
    type = DirichletBC
    variable = disp_z
    boundary = left
    value = 0.0
  []
  [fix_rot_x]
    type = DirichletBC
    variable = rot_x
    boundary = left
    value = 0.0
  []
  [fix_rot_y]
    type = DirichletBC
    variable = rot_y
    boundary = left
    value = 0.0
  []
  [fix_rot_z]
    type = DirichletBC
    variable = rot_z
    boundary = left
    value = 0.0
  []
[]

[Kernels]
  [solid_disp_x]
    type = StressDivergenceBeam
    block = 0
    variable = disp_x
    displacements = 'disp_x disp_y disp_z'
    rotations = 'rot_x rot_y rot_z'
    component = 0
  []
  [solid_disp_y]
    type = StressDivergenceBeam
    block = 0
    variable = disp_y
    displacements = 'disp_x disp_y disp_z'
    rotations = 'rot_x rot_y rot_z'
    component = 1
  []
  [solid_disp_z]
    type = StressDivergenceBeam
    block = 0
    variable = disp_z
    displacements = 'disp_x disp_y disp_z'
    rotations = 'rot_x rot_y rot_z'
    component = 2
  []
  [solid_rot_x]
    type = StressDivergenceBeam
    block = 0
    variable = rot_x
    displacements = 'disp_x disp_y disp_z'
    rotations = 'rot_x rot_y rot_z'
    component = 3
  []
  [solid_rot_y]
    type = StressDivergenceBeam
    block = 0
    variable = rot_y
    displacements = 'disp_x disp_y disp_z'
    rotations = 'rot_x rot_y rot_z'
    component = 4
  []
  [solid_rot_z]
    type = StressDivergenceBeam
    block = 0
    variable = rot_z
    displacements = 'disp_x disp_y disp_z'
    rotations = 'rot_x rot_y rot_z'
    component = 5
  []

  [py_reaction]
    type = ADPySimpleKernel
    variable = disp_y
    py_force_name = py_force
  []
[]

[DiracKernels]
  [head_load_y]
    type = ConstantPointSource
    variable = disp_y
    block = 0
    point = '10.0 0.0 0.0'
    value = 1.0
  []
[]

[AuxVariables]
  [beam_force_y]
    order = CONSTANT
    family = MONOMIAL
  []
  [py_force_y]
    order = CONSTANT
    family = MONOMIAL
  []
  [beam_moment_z]
    order = CONSTANT
    family = MONOMIAL
  []
[]

[AuxKernels]
  [beam_force_y]
    type = MaterialRealVectorValueAux
    property = forces
    variable = beam_force_y
    component = 1
    execute_on = timestep_end
  []
  [beam_moment_z]
    type = MaterialRealVectorValueAux
    property = moments
    variable = beam_moment_z
    component = 2
    execute_on = timestep_end
  []
  [py_force_y]
    type = ADMaterialRealAux
    property = py_force
    variable = py_force_y
    execute_on = timestep_end
  []
[]

[Materials]
  [elasticity]
    type = ComputeElasticityBeam
    block = 0
    youngs_modulus = 2.068e11
    poissons_ratio = 0.3
    shear_coefficient = 1.0
  []
  [strain]
    type = ComputeIncrementalBeamStrain
    block = 0
    displacements = 'disp_x disp_y disp_z'
    rotations = 'rot_x rot_y rot_z'
    area = 1.759e-4
    Ay = 0.0
    Az = 0.0
    Iy = 1.7329e-8
    Iz = 1.7329e-8
    y_orientation = '0.0 1.0 0.0'
  []
  [stress]
    type = ComputeBeamResultants
    block = 0
  []

  [py_mat]
    type = ADPyLinearMaterial
    y = disp_y
    py_force_name = py_force
    k0 = 1e5
    k_rate = 0.0
    depth_component = 0
    depth_origin = 0.0
    positive_depth_only = true
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
  line_search = none
  nl_max_its = 20
  nl_rel_tol = 1e-8
  nl_abs_tol = 1e-10
  automatic_scaling = true
  petsc_options_iname = '-ksp_type -pc_type'
  petsc_options_value = 'preonly lu'
[]

[Postprocessors]
  [head_disp_y]
    type = PointValue
    point = '10.0 0.0 0.0'
    variable = disp_y
  []
  [head_rot_z]
    type = PointValue
    point = '10.0 0.0 0.0'
    variable = rot_z
  []
  [near_base_disp_y]
    type = PointValue
    point = '0.05 0.0 0.0'
    variable = disp_y
  []
  [near_base_force_y]
    type = PointValue
    point = '0.05 0.0 0.0'
    variable = beam_force_y
  []
[]

[Outputs]
  csv = true
  exodus = true
[]