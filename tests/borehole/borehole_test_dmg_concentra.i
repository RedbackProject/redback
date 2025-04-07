[Mesh]
  type = FileMesh
  file = Cylinder_hollow_perturb.msh
  boundary_name = 'bottom top inside outside'
  boundary_id = '109 110 112 111'
[]

[GlobalParams]
  displacements = 'disp_x disp_y disp_z'
  porepressure = porepressure
  block = 0
[]

[Variables]
  [./disp_x]
    block = 0
  [../]
  [./disp_y]
    block = 0
  [../]
  [./disp_z]
    block = 0
  [../]
  [./porepressure]
    scaling = 1E9 # Notice the scaling, to make porepressure's kernels roughly of same magnitude as disp's kernels
    block = 0
  [../]
  [./damage]
  [../]
  [./temperature]
  [../]
  [./concentration]
  [../]
[]

[Functions]
  [./temp_ic]
    type = ParsedFunction
    value = 'sqrt(x*x+y*y)/0.9 - 1/9' # -sqrt(x*x+y*y)/0.9 + 10/9
  [../]
  [./inner_pressure_fct]
    type = ParsedFunction
    value = 0.01+1000*t
  [../]
  [./inner_displacement]
    type = ParsedFunction
    value = 1e-4+10*t
  [../]
[]

[BCs]
  active = 'Pressure Concentr_borehole Temp_borehole Temp_outside plane_strain fixed_outer_x fixed_outer_y'
  [./fixed_outer_x]
    type = DirichletBC
    variable = disp_x
    value = 0
    boundary = outside
  [../]
  [./fixed_outer_y]
    type = DirichletBC
    variable = disp_y
    value = 0
    boundary = outside
  [../]
  [./plane_strain]
    type = DirichletBC
    variable = disp_z
    value = 0
    boundary = 'top bottom'
  [../]
  [./borehole_wall]
    type = FunctionDirichletBC
    variable = porepressure
    boundary = inside
    function = inner_pressure_fct
  [../]
  [./Temp_outside]
    type = DirichletBC
    variable = temperature
    boundary = outside
    value = 1 # 0
  [../]
  [./Temp_borehole]
    type = DirichletBC
    variable = temperature
    boundary = inside
    value = 0 # 1
  [../]
  [./Pressure]
    [./myPressure]
      function = inner_displacement
      disp_z = disp_z
      disp_y = disp_y
      disp_x = disp_x
      boundary = inside
    [../]
  [../]
  [./Concentr_borehole]
    type = DirichletBC
    variable = concentration
    boundary = inside
    value = 1
  [../]
[]

[Kernels]
  [./dp_dt]
    type = TimeDerivative
    variable = porepressure
    block = 0
  [../]
  [./mass_diff]
    type = RedbackMassDiffusion
    variable = porepressure
  [../]
  [./poromech]
    type = RedbackPoromechanics
    variable = porepressure
  [../]
  [./damage_dt]
    type = TimeDerivative
    variable = damage
  [../]
  [./damage_kernel]
    type = RedbackDamage
    variable = damage
  [../]
  [./dt_temp]
    type = TimeDerivative
    variable = temperature
  [../]
  [./diff_temp]
    type = Diffusion
    variable = temperature
  [../]
  [./mech_dissip]
    type = RedbackMechDissip
    variable = temperature
  [../]
  [./Thermal_press]
    type = RedbackThermalPressurization
    variable = porepressure
    temperature = temperature
  [../]
  [./diff_concentra]
    type = Diffusion
    variable = concentration
  [../]
  [./dC_dt]
    type = TimeDerivative
    variable = concentration
  [../]
  [./Chem_endo]
    type = RedbackChemEndo
    variable = temperature
  [../]
  [./Chem_pressure]
    type = RedbackChemPressure
    variable = porepressure
  [../]
[]

[Materials]
  active = 'no_mech plastic_material'
  [./no_mech]
    type = RedbackMaterial
    disp_z = disp_z
    disp_y = disp_y
    disp_x = disp_y
    pore_pres = porepressure
    total_porosity = 0.1
    phi0 = 0.1
    pressurization_coefficient = 1e-7
    temperature = temperature
    gr = 10
    ar = 10
    chemical_ar_F_factor = -5.0 # -0.1; needs to be negative
    is_chemistry_on = true
    mu = 1e-4 # 0
    da_endo = 1e-3 # 0
    concentration = concentration
  [../]
  [./elastic_material]
    type = RedbackMechMaterialElastic
    disp_z = disp_z
    disp_y = disp_y
    disp_x = disp_x
    poisson_ratio = 0.25
    youngs_modulus = 100
    outputs = all
  [../]
  [./plastic_material]
    type = RedbackMechMaterialDP
    disp_z = disp_z
    disp_y = disp_y
    disp_x = disp_x
    outputs = all
    pore_pres = porepressure
    yield_stress = '0 0.006 0.1 0.003' # 0 0.006 1 0.006
    poisson_ratio = 0.25
    youngs_modulus = 100
    damage = damage
    damage_coefficient = 1e4
    damage_method = BrittleDamage
    temperature = temperature
    exponent = 10
    chemo_mechanical_porosity_coeff = 1e10
  [../]
[]

[Postprocessors]
  [./Int_PoreP]
    type = PointValue
    variable = porepressure
    point = '0.1 0 0.1'
    execute_on = timestep_begin
  [../]
  [./PorePress]
    type = PointValue
    variable = porepressure
    point = '0.2 0 0.1'
  [../]
[]

[Preconditioning]
  active = 'andy'
  [./andy]
    # gmres asm 1E0 1E-10 200 500 lu NONZERO
    type = SMP
    full = true
    solve_type = PJFNK
    petsc_options = '-snes_monitor -snes_linesearch_monitor -ksp_monitor'
    petsc_options_iname = '-ksp_type -pc_type  -snes_atol -snes_rtol -snes_max_it -ksp_max_it -ksp_atol -sub_pc_type -sub_pc_factor_shift_type'
    petsc_options_value = 'gmres        asm        1E-2          1E-8        200                500                  1e-4        lu                      NONZERO'
  [../]
  [./manolis]
    type = SMP
    solve_type = NEWTON
    petsc_options_iname = '-ksp_type -pc_type -snes_atol -snes_rtol -snes_max_it'
    petsc_options_value = 'bcgs bjacobi 1E-14 1E-10 10000'
  [../]
  [./default]
    type = SMP
    solve_type = NEWTON
  [../]
  [./Manman]
    type = SMP
    solve_type = NEWTON
    petsc_options_iname = '-ksp_type -pc_type -snes_atol -snes_rtol -snes_max_it'
    petsc_options_value = 'bcgs bjacobi 1E-14 1E-10 10000'
  [../]
[]

[Executioner]
  # [./TimeStepper]
  # type = PostprocessorDT
  # postprocessor = dt
  # dt = 0.003
  # [../]
  type = Transient
  l_max_its = 100
  end_time = 0.3
  dt = 1e-5
  l_tol = 1e-5 # 1e-05
[]

[Outputs]
  active = 'csv_p csv_s csv_t'
  exodus = true
  execute_on = 'timestep_end initial'
  file_base = borehole_test_dmg_concentra
  [./csv_p]
    file_base = borehole_highres_p
    type = CSV
  [../]
  [./csv_s]
    file_base = borehole_highres_s
    type = CSV
  [../]
  [./csv_t]
    file_base = borehole_highres_t
    type = CSV
  [../]
  [./exodus]
    type = Exodus
    elemental_as_nodal = true
  [../]
[]

[RedbackMechAction]
  [./mechanics]
    disp_z = disp_z
    pore_pres = porepressure
    disp_y = disp_y
    disp_x = disp_x
  [../]
[]

[ICs]
  [./random_dmg_ic]
    variable = damage
    max = 0.01
    type = RandomIC
  [../]
  [./random_temp_ic]
    function = temp_ic
    max = 0.1
    type = FunctionWithRandomIC
    variable = temperature
  [../]
[]

