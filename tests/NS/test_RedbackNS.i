# This input file tests the behavior of NS equations

[GlobalParams]
  fluid_vel_x = vel_x
  fluid_vel_y = vel_y
  pore_pres = p
[]

[Mesh]
  type = GeneratedMesh
  dim = 2
  uniform_refine = 1
  block_name = pores
  block_id = 0
  elem_type = QUAD9
[]

[Variables]
  [./vel_x]
    order = SECOND
  [../]
  [./vel_y]
    order = SECOND
  [../]
  [./p]
  [../]
[]

[Kernels]
  [./mass_fluid_divergence]
    type = RedbackFluidDivergence
    variable = p
  [../]
  [./stress_div_x]
    type = RedbackFluidStressDivergenceTensors
    variable = vel_x
    component = 0
    pore_pres = p
  [../]
  [./stress_div_y]
    type = RedbackFluidStressDivergenceTensors
    variable = vel_y
    component = 1
  [../]
  [./navier_x]
    type = RedbackNavier
    variable = vel_x
    component = 0
  [../]
  [./navier_y]
    type = RedbackNavier
    variable = vel_y
    component = 1
  [../]
[]

[BCs]
  [./x_no_slip]
    type = DirichletBC
    variable = vel_x
    boundary = 'top bottom'
    value = 0.0
  [../]
  [./y_no_slip]
    type = DirichletBC
    variable = vel_y
    boundary = 'top bottom left right'
    value = 0.0
  [../]
  [./lowp]
    type = DirichletBC
    variable = p
    boundary = right
    value = 0
  [../]
  [./inlet]
    type = DirichletBC
    variable = p
    boundary = left
    value = 1
  [../]
[]

[Materials]
  [./fluid]
    type = RedbackFluidMaterial
    block = pores
  [../]
[]

[Postprocessors]
  [./vel_avg]
    type = ElementAverageValue
    variable = vel_x
  [../]
  [./vel_max]
    type = PointValue
    variable = vel_x
    point = '0.5 0.5 0'
  [../]
[]

[Executioner]
  # This is setup automatically in MOOSE (SetupPBPAction.C)
  # petsc_options = '-snes_mf_operator'
  # petsc_options_iname = '-pc_type'
  # petsc_options_value =  'asm'
  type = Steady
[]

[Preconditioning]
  active = 'FSP'
  [./FSP]
    # It is the starting point of splitting
    type = FSP
    petsc_options = '-ksp_converged_reason -snes_converged_reason'
    petsc_options_iname = '-snes_type -ksp_type -ksp_rtol -ksp_atol -ksp_max_it -snes_atol -snes_rtol -snes_max_it -snes_max_funcs'
    petsc_options_value = 'newtonls     fgmres     1e-2     1e-15       200       1e-8        1e-15       200           100000'
    topsplit = uv
    [./uv]
      petsc_options_iname = '-pc_fieldsplit_schur_fact_type -pc_fieldsplit_schur_precondition'
      petsc_options_value = 'full selfp'
      splitting = 'u v' # 'u' and 'v'
      splitting_type = schur
    [../]
    [./u]
      # PETSc options for this subsolver
      # A prefix will be applied, so just put the options for this subsolver only
      vars = 'vel_x vel_y'
      petsc_options_iname = '-pc_type -ksp_type -pc_hypre_type'
      petsc_options_value = '  hypre    preonly     boomeramg '
    [../]
    [./v]
      # PETSc options for this subsolver
      vars = p
      petsc_options_iname = '-pc_type -ksp_type -sub_pc_type -sub_pc_factor_levels'
      petsc_options_value = '  jacobi  preonly        ilu            3'
    [../]
  [../]
  [./asm_ilu]
    type = SMP
    full = true
    solve_type = PJFNK
    petsc_options = '-ksp_converged_reason -snes_converged_reason' #-snes_monitor -snes_linesearch_monitor -ksp_monitor'
    petsc_options_iname = '-ksp_type -pc_type  -snes_atol -snes_rtol -snes_max_it -ksp_max_it -ksp_atol -sub_pc_type -sub_pc_factor_shift_type'
    petsc_options_value = 'gmres        asm        1E-8      1E-15        200        100         1e-8        lu                   NONZERO'
  [../]
[]

[Outputs]
  file_base = NSRedback_test
  exodus = true
  csv = true
[]
