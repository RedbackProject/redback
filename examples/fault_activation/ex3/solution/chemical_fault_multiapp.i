[Mesh]
  type = FileMesh
  file = ../../fault_mesh.msh
[]

[Variables]
  [./disp_x]
  [../]
  [./disp_y]
  [../]
  [./disp_z]
  [../]
  [./temp]
  [../]
  [./pore_pressure]
  [../]
[]

[AuxVariables]
  [./total_porosity]
    order = CONSTANT
    family = MONOMIAL
  [../]
  [./permeability]
    order = FIRST
    family = LAGRANGE
    initial_condition = 0.1
  [../]
[]

[Kernels]
  [./td_temp]
    type = TimeDerivative
    variable = temp
  [../]
  [./diff_temp]
    type = Diffusion
    variable = temp
  [../]
  [./mh_temp]
    type = RedbackMechDissip
    variable = temp
    block = mid_block
  [../]
  [./td_press]
    type = TimeDerivative
    variable = pore_pressure
  [../]
  [./press_diff]
    type = RedbackMassDiffusion
    variable = pore_pressure
  [../]
  [./chem_press]
    type = RedbackChemPressure
    variable = pore_pressure
    block = mid_block
    temperature = temp
  [../]
  [./Chem_endo_temp]
    type = RedbackChemEndo
    variable = temp
    block = mid_block
  [../]
[]

[AuxKernels]
  [./total_porosity]
    type = RedbackTotalPorosityAux
    variable = total_porosity
    execute_on = linear
  [../]
[]

[Materials]
  [./redback_nomech]
    type = RedbackMaterial
    disp_x = disp_x
    disp_y = disp_y
    disp_z = disp_z
    gr = 0.5
    ar = 11
    da_endo = 2.05e-5
    da_exo = 8.29e-2
    phi0 = 0.03
    Kc = 2e2
    ref_lewis_nb = 10
    ar_F = 20.2
    ar_R = 10.1
    eta1 = 2.27
    eta2 = 0.26
    mu = 3.4e-5
    Aphi = 1
    alpha_2 = 4
    is_chemistry_on = true
    temperature = temp
    pore_pres = pore_pressure
    total_porosity = total_porosity
    inverse_lewis_number_tilde = permeability
  [../]
  [./fault_mech]
    type = RedbackMechMaterialJ2
    block = mid_block
    disp_x = disp_x
    disp_y = disp_y
    disp_z = disp_z
    youngs_modulus = 10000
    poisson_ratio = 0.3
    yield_stress = '0. 1 1. 1'
    pore_pres = pore_pressure
    temperature = temp
    total_porosity = total_porosity
    # outputs = all
  [../]
  [./rock_bottom]
    type = RedbackMechMaterialElastic
    block = 'bot_block base_block'
    disp_x = disp_x
    disp_y = disp_y
    disp_z = disp_z
    youngs_modulus = 20000
    poisson_ratio = 0.3
    pore_pres = pore_pressure
    temperature = temp
    total_porosity = total_porosity
    # outputs = all
  [../]
  [./rock_top]
    type = RedbackMechMaterialElastic
    block = top_block
    disp_x = disp_x
    disp_y = disp_y
    disp_z = disp_z
    youngs_modulus = 10000
    poisson_ratio = 0.3
    yield_stress = '0 1.1 1 1.1'
    pore_pres = pore_pressure
    temperature = temp
    total_porosity = total_porosity
    # outputs = all
  [../]
[]

[Functions]
  [./ramp_pos]
    type = ParsedFunction
    expression = 0.7*tanh(0.1*t)
  [../]
  [./timestep_function]
    type = ParsedFunction
    expression = if(t<19,0.5,0.01)
  [../]
[]

[BCs]
  [./Periodic]
    [./xperiodic]
      variable = 'disp_x disp_y disp_z temp pore_pressure'
      translation = '1 0 0'
      primary = left_base
      secondary = right_base
    [../]
    [./xperiodic2]
      variable = 'disp_x disp_y disp_z temp pore_pressure'
      translation = '1 0 0'
      primary = left
      secondary = right
    [../]
  [../]
  [./ux_equals_zero_on_top]
    type = DirichletBC
    variable = disp_x
    boundary = top
    value = 0
  [../]
  [./top_cauchy_zero]
    type = NeumannBC
    variable = disp_x
    boundary = top
  [../]
  [./uy_bottom]
    type = DirichletBC
    variable = disp_y
    boundary = bottom
    value = 0.0
  [../]
  [./uz_bottom]
    type = DirichletBC
    variable = disp_z
    boundary = bottom
    value = 0.0
  [../]
  [./pore_pressure]
    type = DirichletBC
    variable = pore_pressure
    boundary = 'bottom top'
    value = 0
  [../]
  [./ux_force_bottom_base]
    type = FunctionNeumannBC
    variable = disp_x
    boundary = left_base
    function = ramp_pos
  [../]
  [./temperature_top]
    type = DirichletBC
    variable = temp
    boundary = top
    value = 0
  [../]
  [./temperature_bottom]
    type = DirichletBC
    variable = temp
    boundary = bottom
    value = 0.001 # 0.1
  [../]
[]

[Postprocessors]
  [./middle_temp]
    type = PointValue
    variable = temp
    point = '0 0 0'
  [../]
  [./middle_press]
    type = PointValue
    variable = pore_pressure
    point = '0 0 0'
  [../]
  [./middle_porosity]
    type = PointValue
    variable = total_porosity
    point = '0 0 0'
  [../]
  [./middle_perm]
    type = PointValue
    variable = permeability
    point = '0 0 0'
  [../]
  [./middle_perm_kozeny]
    type = ParsedPostprocessor
    expression = '0.1*pow(middle_porosity/0.03,3)*pow((1-0.03)/(1-middle_porosity),2)'
    pp_names = 'middle_porosity'
  [../]
  [./dt]
    type = TimestepSize
  [../]
[]

[Preconditioning]
  [./SMP]
    type = SMP
    full = true
    solve_type = PJFNK
    petsc_options_iname = '-ksp_type -pc_type -sub_pc_type -ksp_gmres_restart'
    petsc_options_value = 'gmres asm lu 201'
  [../]
[]

[Executioner]
  type = Transient
  start_time = 0.0
  end_time = 22
  [./TimeStepper]
    type = FunctionDT
    function = timestep_function
  [../]
  nl_abs_tol = 1e-8
  l_max_its = 100
  nl_max_its = 10
  line_search = bt
[]

[Outputs]
  file_base = chemical_fault_multiapp
  exodus = true
  csv = true
  perf_graph = true
[]

[RedbackMechAction]
  [./solid]
    disp_x = disp_x
    disp_y = disp_y
    disp_z = disp_z
    temp = temp
    pore_pres = pore_pressure
  [../]
[]

[MultiApps]
  [./sub_app]
    type = TransientMultiApp
    app_type = RedbackApp
    input_files = '../poiseuille_flow_micro.i'
    positions = '0.5   0   0
                 0.5   0.25 0
                 0.5  -0.25 0
                 0.5   0.5 0
                 0.5  -0.5 0
                 0.5   1   0
                 0.5  -1   0
                 0.5   2   0
                 0.5  -2   0
                 0.5   5   0
                 0.5  -5   0'
  [../]
[]

[Transfers]
  [./sending_porosity]
    type = MultiAppVariableValueSamplePostprocessorTransfer
    source_variable = total_porosity
    to_multi_app = sub_app
    postprocessor = porosity_receiver
  [../]
  [./receiving_eff_shear_modulus]
    type = MultiAppPostprocessorInterpolationTransfer
    postprocessor = permeability
    from_multi_app = sub_app
    variable = permeability
  [../]
[]

