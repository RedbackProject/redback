[Mesh]
  [./generate]
    type = GeneratedMeshGenerator
    dim = 2
    ny = 2
    nx = 3
    xmax = 3
    ymax = 2
    elem_type = QUAD9
  [../]
  [./fluid]
    type = SubdomainBoundingBoxGenerator
    input = generate
    bottom_left = '0 0 0'
    top_right = '3 1 0'
    block_id = 1
  [../]
  [./sideset]
    type = SideSetsBetweenSubdomainsGenerator
    input = fluid
    master_block = 1
    new_boundary = 10
    paired_block = 0
  [../]
[]

[Variables]
  [./disp_x]
    order = SECOND
  [../]
  [./disp_y]
    order = SECOND
  [../]
  [./vel_x]
    order = SECOND
    block = 1
  [../]
  [./vel_y]
    order = SECOND
    block = 1
  [../]
  [./p]
    block = 1
  [../]
[]

[GlobalParams]
  fluid_vel_x = vel_x
  fluid_vel_y = vel_y
  pore_pres = p
  displacements = 'disp_x disp_y'
[]

[Kernels]
  [./mass_fluid_divergence]
    type = RedbackFluidDivergence
    variable = p
    use_displaced_mesh = true
  [../]
  [./stress_div_x]
    type = RedbackFluidStressDivergenceTensors
    variable = vel_x
    component = 0
    use_displaced_mesh = true
  [../]
  [./stress_div_y]
    type = RedbackFluidStressDivergenceTensors
    variable = vel_y
    component = 1
    use_displaced_mesh = true
  [../]
  [./TensorMechanics]
    block = 0
    use_displaced_mesh = true
  [../]
  [./diff_x]
    type = Diffusion
    variable = disp_x
    block = 1
    use_displaced_mesh = true
  [../]
  [./diff_y]
    type = Diffusion
    variable = disp_y
    block = 1
    use_displaced_mesh = true
  [../]
[]

[BCs]
  [./x_no_slip]
    type = DirichletBC
    variable = vel_x
    boundary = 'top bottom 10'
    value = 0.0
  [../]
  [./y_no_slip]
    type = DirichletBC
    variable = vel_y
    boundary = 'top bottom 10'
    value = 0.0
  [../]
  [./inlet_p]
    type = DirichletBC
    variable = p
    boundary = left
    value = 10
  [../]
  [./outlet_p]
    type = DirichletBC
    variable = p
    boundary = right
    value = 0
  [../]
  [./fixed_x]
    type = DirichletBC
    variable = disp_x
    boundary = 'left right'
    value = 0.0
  [../]
  [./fixed_y]
    type = DirichletBC
    variable = disp_y
    boundary = bottom
    value = 0.0
  [../]
  [./p_force_y]
    type = PressureNeumannBC
    variable = disp_y
    boundary = 10
    component = 1
    coupled_var = p
    use_displaced_mesh = true
  [../]
  [./p_force_x]
    type = PressureNeumannBC
    variable = disp_x
    boundary = 10
    component = 0
    coupled_var = p
    use_displaced_mesh = true
  [../]
  [./disp_top]
    type = DirichletBC
    variable = disp_y
    boundary = top
    value = 0
  [../]
[]

[Materials]
  [./small_strain]
    type = ComputePlaneSmallStrain
    block = 0
  [../]
  [./linear_stress]
    type = ComputeLinearElasticStress
    block = 0
  [../]
  [./elasticity_tensor]
    type = ComputeIsotropicElasticityTensor
    block = 0
    poissons_ratio = 0.3
    youngs_modulus = 1e4
  [../]
  [./fluid]
    type = RedbackFluidMaterial
    block = 1
    use_displaced_mesh = true
  [../]
[]

[Preconditioning]
  [./asm_ilu]
    type = SMP
    full = true
    solve_type = PJFNK
    #petsc_options = '-ksp_converged_reason -snes_converged_reason' #-snes_monitor -snes_linesearch_monitor -ksp_monitor'
    #petsc_options_iname = '-ksp_type -pc_type  -snes_atol -snes_rtol -snes_max_it -ksp_max_it -ksp_atol -sub_pc_type -sub_pc_factor_shift_type'
    #petsc_options_value = 'gmres        asm        1E-8      1E-15        200        100         1e-3        lu                   NONZERO'
  [../]
[]

[Executioner]
  type = Steady
[]

[Outputs]
  exodus = true
  file_base = pressure_force
[]
