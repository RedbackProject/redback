[GlobalParams]
  fluid_vel_x = vel_x
  fluid_vel_y = vel_y
  pore_pres = p
  use_displaced_mesh = true
  displacements = 'disp_x disp_y'
[]

[Mesh]
  type = GeneratedMesh
  dim = 2
  xmin = 0
  xmax = 1
  ymin = 0
  ymax = 0.03
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
  [./disp_x]
    order = SECOND
  [../]
  [./disp_y]
    order = SECOND
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
  [./diff_disp_x]
    type = Diffusion
    variable = disp_x
  [../]
  [./diff_disp_y]
    type = Diffusion
    variable = disp_y
  [../]
[]

[Materials]
  [./fluid]
    type = RedbackFluidMaterial
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
    boundary = 'left top bottom'
    value = 0.0
  [../]
  [./inlet_p]
    type = DirichletBC
    variable = p
    boundary = left
    value = 1.0
  [../]
  [./outlet_p]
    type = DirichletBC
    variable = p
    boundary = right
    value = 0.0
  [../]
  [./disp_y_top]
    type = PostprocessorDirichletBC
    variable = disp_y
    boundary = top
    postprocessor = porosity
  [../]
  [./disp_y_bottom]
    type = DirichletBC
    variable = disp_y
    boundary = bottom
    value = 0
  [../]
  [./disp_x]
    type = DirichletBC
    variable = disp_x
    boundary = top
    value = 0
  [../]
[]

[Postprocessors]
  [./porosity_receiver]
    type = Receiver
    default = 0.03
    execute_on = 'TIMESTEP_BEGIN'
  [../]
  [./porosity]
    type = ParsedPostprocessor
    expression = 'porosity_receiver-0.03'
    pp_names = 'porosity_receiver'
    execute_on = 'TIMESTEP_BEGIN'
  [../]
  [./vel_x_avg]
    type = ElementAverageValue
    variable = vel_x
  [../]
  [./permeability]
    type = ParsedPostprocessor
    expression = 0.1*vel_x_avg*porosity_receiver/(7.500000e-05*0.03)
    pp_names = 'vel_x_avg porosity_receiver'
  [../]
[]

[Preconditioning]
  [./SMP_PJFNK]
    type = SMP
    full = true
    solve_type = PJFNK
    petsc_options_iname = '-ksp_gmres_restart -pc_type -sub_pc_type -sub_pc_factor_levels'
    petsc_options_value = '300                bjacobi  ilu          4'
  [../]
[]

[Executioner]
  type = Transient
  num_steps = 1

  line_search = none
  nl_abs_tol = 1e-8
  nl_max_its = 6
  l_tol = 1e-6
  l_max_its = 300
[]

[Outputs]
  # file_base = poiseuille_flow_micro
  # exodus = true
[]
