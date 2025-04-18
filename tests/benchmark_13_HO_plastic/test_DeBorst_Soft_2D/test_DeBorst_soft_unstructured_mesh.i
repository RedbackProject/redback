
[Mesh]
  type = FileMesh
  file = 'mesh_075.msh'
[]

[GlobalParams]
  disp_y = disp_y
  disp_x = disp_x
  wc_z = wc_z
[]

[Postprocessors]
  [./disp_y_top]
    type = PointValue
    point = '60 60 0'
    variable = disp_y
  [../]
  [./wc_z_top]
    type = PointValue
    point = '60 60 0'
    variable = wc_z
  [../]
  [./antisymmetric_top]
    type = SideAverageValue
    variable = macro_rot
    execute_on = linear
    boundary = top
  [../]
  [./number_nonlin]
    type = NumNonlinearIterations
  [../]
  [./number_lin]
    type = NumLinearIterations
  [../]
  [./tangential_force]
    type = SideAverageValue
    variable = stress_12
    boundary = top
  [../]
  [./disp_x_top]
    type = PointValue
    variable = disp_x
    point = '60 60 0'
  [../]
  [./disp_x_bottom]
    type = PointValue
    variable = disp_x
    point = '60 0 0'
  [../]
  [./dt]
    type = TimestepSize
  [../]
  [./tau]
    type = PointValue
    variable = stress_invariant
    point = '0 60 0'
  [../]
  [./gamma_p]
    type = PointValue
    variable = plastic_strain
    point = '0 60 0'
  [../]
[]

# [Postprocessors]
#   [./u_y]
#     type = PointValue
#     point = '0.05 0.1747 0'
#     variable = disp_y
#   [../]
#   [./react_y]
#     type = SidesetReaction
#     direction = '0 -1 0'
#     stress_tensor = stress
#     boundary = 'top'
#   [../]
# []

[Variables]
  [./disp_x]
  [../]
  [./disp_y]
  [../]
  [./wc_z]
  [../]
[]

[AuxVariables]
  [./plastic_strain]
    order = CONSTANT
    family = MONOMIAL
  [../]
  [./stress_invariant]
    order = CONSTANT
    family = MONOMIAL
  [../]
  [./ecroui]
    order = CONSTANT
    family = MONOMIAL
  [../]
  [./failure]
    order = CONSTANT
    family = MONOMIAL
  [../]
  [./lagrange]
    order = CONSTANT
    family = MONOMIAL
  [../]
  [./stress_11]
    order = CONSTANT
    family = MONOMIAL
  [../]
  [./stress_22]
    order = CONSTANT
    family = MONOMIAL
  [../]
  [./stress_12]
    order = CONSTANT
    family = MONOMIAL
  [../]
  [./stress_21]
    order = CONSTANT
    family = MONOMIAL
  [../]
  [./stress_33]
    order = CONSTANT
    family = MONOMIAL
  [../]
  [./couple_stress_32]
    order = CONSTANT
    family = MONOMIAL
  [../]
  [./couple_stress_23]
    order = CONSTANT
    family = MONOMIAL
  [../]
  [./epsilon_11]
    order = CONSTANT
    family = MONOMIAL
  [../]
  [./epsilon_22]
    family = MONOMIAL
  [../]
  [./strain_12]
    order = CONSTANT
    family = MONOMIAL
  [../]
  [./strain_21]
    order = CONSTANT
    family = MONOMIAL
  [../]
  [./macro_rot]
    order = CONSTANT
    family = MONOMIAL
  [../]
  [./plastic_11]
    order = CONSTANT
    family = MONOMIAL
  [../]
  [./plastic_22]
    order = CONSTANT
    family = MONOMIAL
  [../]
  [./plastic_12]
    order = CONSTANT
    family = MONOMIAL
  [../]
  [./plastic_21]
    order = CONSTANT
    family = MONOMIAL
  [../]
  [./plastic_cur_33]
    order = CONSTANT
    family = MONOMIAL
  [../]
  [./plastic_cur_31]
    order = CONSTANT
    family = MONOMIAL
  [../]
  [./plastic_cur_13]
    order = CONSTANT
    family = MONOMIAL
  [../]
  [./plastic_cur_32]
    order = CONSTANT
    family = MONOMIAL
  [../]
  [./plastic_cur_23]
    order = CONSTANT
    family = MONOMIAL
  [../]
  [./elastic_strain_11]
    order = CONSTANT
    family = MONOMIAL
  [../]
  [./elastic_strain_22]
    order = CONSTANT
    family = MONOMIAL
  [../]
  [./elastic_strain_12]
    order = CONSTANT
    family = MONOMIAL
  [../]
  [./elastic_strain_21]
    order = CONSTANT
    family = MONOMIAL
  [../]
  [./elastic_curvature_23]
    order = CONSTANT
    family = MONOMIAL
  [../]
  [./elastic_curvature_32]
    order = CONSTANT
    family = MONOMIAL
  [../]
[]

[Kernels]
  [./cx_elastic]
    type = RedbackCosseratStressDivergenceTensors
    variable = disp_x
    displacements = 'disp_x disp_y'
    component = 0
  [../]
  [./cy_elastic]
    type = RedbackCosseratStressDivergenceTensors
    variable = disp_y
    displacements = 'disp_x disp_y'
    component = 1
  [../]
  [./z_couple]
    type = RedbackCosseratStressDivergenceTensors
    variable = wc_z
    component = 2
    base_name = coupled
    disp_z = wc_z
  [../]
  [./z_moment]
    type = RedbackMomentBalancing
    variable = wc_z
    component = 2
  [../]
[]

[AuxKernels]
  [./plastic_strain]
    type = MaterialRealAux
    variable = plastic_strain
    property = eqv_plastic_strain
  [../]
  [./stress_invariant]
    type = MaterialRealAux
    variable = stress_invariant
    property = stress_invariant
  [../]
  [./ecroui_param]
    type = MaterialRealAux
    variable = ecroui
    property = hardening_variable
  [../]
  [./failure_surf]
    type = MaterialRealAux
    variable = failure
    property = failure_surface
  [../]
  [./lagrange_mult]
    type = MaterialRealAux
    variable = lagrange
    property = lagrange_multiplier
    execute_on = timestep_end
  [../]
  [./stress_11]
    type = RankTwoAux
    variable = stress_11
    rank_two_tensor = stress
    index_j = 0
    index_i = 0
  [../]
  [./stress_22]
    type = RankTwoAux
    variable = stress_22
    rank_two_tensor = stress
    index_j = 1
    index_i = 1
  [../]
  [./stress_33]
    type = RankTwoAux
    variable = stress_33
    rank_two_tensor = stress
    index_j = 2
    index_i = 2
  [../]
  [./stress_12]
    type = RankTwoAux
    variable = stress_12
    rank_two_tensor = stress
    index_j = 1
    index_i = 0
  [../]
  [./stress_21]
    type = RankTwoAux
    variable = stress_21
    rank_two_tensor = stress
    index_j = 0
    index_i = 1
  [../]
  [./couple_stress_32]
    type = RankTwoAux
    variable = couple_stress_32
    rank_two_tensor = coupled_stress
    index_j = 1
    index_i = 2
    execute_on = nonlinear
  [../]
  [./couple_stress_23]
    type = RankTwoAux
    variable = couple_stress_23
    rank_two_tensor = coupled_stress
    index_j = 2
    index_i = 1
    execute_on = nonlinear
  [../]
  [./epsilon_11]
    type = RankTwoAux
    variable = epsilon_11
    rank_two_tensor = elastic_strain
    index_j = 0
    index_i = 0
  [../]
  [./epsilon_22]
    type = RankTwoAux
    variable = epsilon_22
    rank_two_tensor = elastic_strain
    index_j = 1
    index_i = 1
  [../]
  [./strain_12]
    type = RankTwoAux
    variable = strain_12
    rank_two_tensor = total_strain
    index_j = 1
    index_i = 0
  [../]
  [./strain_21]
    type = RankTwoAux
    variable = strain_21
    rank_two_tensor = total_strain
    index_j = 0
    index_i = 1
  [../]
  [./antisymmetric_strain]
    type = RankTwoAux
    variable = macro_rot
    rank_two_tensor = macro_rotation
    index_j = 1
    index_i = 0
    execute_on = linear
  [../]
  [./plastic_xx]
    type = RankTwoAux
    variable = plastic_11
    rank_two_tensor = plastic_strain
    index_j = 0
    index_i = 0
  [../]
  [./plastic_yy]
    type = RankTwoAux
    variable = plastic_22
    rank_two_tensor = plastic_strain
    index_j = 1
    index_i = 1
  [../]
  [./plastic_xy]
    type = RankTwoAux
    variable = plastic_12
    rank_two_tensor = plastic_strain
    index_j = 1
    index_i = 0
  [../]
  [./plastic_yx]
    type = RankTwoAux
    variable = plastic_21
    rank_two_tensor = plastic_strain
    index_j = 0
    index_i = 1
  [../]
  [./plastic_curv_33]
    type = RankTwoAux
    variable = plastic_cur_33
    rank_two_tensor = plastic_curvature
    index_j = 2
    index_i = 2
  [../]
  [./plastic_curv_32]
    type = RankTwoAux
    variable = plastic_cur_32
    rank_two_tensor = plastic_curvature
    index_j = 1
    index_i = 2
  [../]
  [./plastic_curv_23]
    type = RankTwoAux
    variable = plastic_cur_23
    rank_two_tensor = plastic_curvature
    index_j = 2
    index_i = 1
  [../]
  [./plastic_curv_31]
    type = RankTwoAux
    variable = plastic_cur_31
    rank_two_tensor = plastic_curvature
    index_j = 0
    index_i = 2
  [../]
  [./plastic_curv_13]
    type = RankTwoAux
    variable = plastic_cur_13
    rank_two_tensor = plastic_curvature
    index_j = 2
    index_i = 0
  [../]
  [./elastic_strain_11]
    type = RankTwoAux
    variable = elastic_strain_11
    rank_two_tensor = elastic_strain
    index_j = 0
    index_i = 0
  [../]
  [./elastic_strain_22]
    type = RankTwoAux
    variable = elastic_strain_22
    rank_two_tensor = elastic_strain
    index_j = 1
    index_i = 1
  [../]
  [./elastic_strain_12]
    type = RankTwoAux
    variable = elastic_strain_12
    rank_two_tensor = elastic_strain
    index_j = 1
    index_i = 0
  [../]
  [./elastic_strain_21]
    type = RankTwoAux
    variable = elastic_strain_21
    rank_two_tensor = elastic_strain
    index_j = 0
    index_i = 1
  [../]
 
  [./elastic_curvature_32]
    type = RankTwoAux
    variable = elastic_curvature_32
    rank_two_tensor = elastic_curvature
    index_j = 1
    index_i = 2
  [../]
  [./elastic_curvature_23]
    type = RankTwoAux
    variable = elastic_curvature_23
    rank_two_tensor = elastic_curvature
    index_j = 2
    index_i = 1
  [../]
[]

[BCs]
  # [./uy_top]
  #   type = DirichletBC
  #   variable = disp_y
  #   boundary = 'top'
  #   value = '0'
  # [../]
  [./uy_bottom]
    type = DirichletBC
    variable = disp_y
    boundary = 'bottom'
    value = '0'
  [../]
  [./ux_zero_bottom]
    type = DirichletBC
    variable = disp_x
    boundary = bottom
    value = 0
  [../]
  [footing_displacement]
      type = FunctionDirichletBC
      boundary = 'top'
      variable = disp_x    
      function  = '0.1*t'
  [../]
  [./Periodic]
    [./x_direction]
      variable = 'disp_x disp_y wc_z'
      auto_direction = x
    [../]
  []
  [./wc_z_bottom]
    type = DirichletBC
    variable = wc_z
    boundary = bottom
    value = 0.0
  [../]
  [./wc_z_top]
    type = DirichletBC
    variable = wc_z
    boundary = top
    value = 0.0
  [../]
[]

[Materials]
  active = 'Redbackcosserat redback_mat'
  [./Redbackcosserat]
    # 0 2.6549E3 2.6549E3
    type = RedbackMechMaterialHO
    B_ijkl = '0 5.0E4 5.0E4'
    C_ijkl = '1.3333E3 4.0E3 2.0E3'
    fill_method = general_isotropic
    damage_method = BreakageMechanics
    cohesion = 100
    hardening_mech_modulus = -4
    friction_coefficient = 0.0
    min_stepsize = 1e-8
    plasticity_type = DeBorst_2D
  [../]
  [./redback_mat]
    type = RedbackMaterial
  [../]
[]

[Preconditioning]
  active = 'andy'
  [./andy]
    type = SMP
    full = true
    petsc_options_iname = '-ksp_type -pc_type    -snes_atol -snes_rtol -snes_max_it -ksp_atol -ksp_rtol'
    petsc_options_value = 'gmres          bjacobi     1E-6          1E-7          40                1E-12      1E-10 '
  [../]
  # [./debug_jacob]
  #   type = FDP
  #   full = true
  # [../]
  # [./default]
  #   type = SMP
  #   solve_type = NEWTON
  #   line_search = basic
  # [../]
[]

[Executioner]
  type = Transient
  end_time = 50
  dt = 0.05

  solve_type = NEWTON
  nl_abs_tol = 1e-8
  l_tol = 1e-10
  nl_rel_tol = 1e-04
  line_search = basic
[]

[Outputs]
  execute_on = 'timestep_end initial'
  exodus = true
  file_base = test_DeBorst_soft_unstructured_mesh_075

  [./csv]
    type = CSV
    execute_on = timestep_end
  [../]
  # print_linear_residuals = f
[]

