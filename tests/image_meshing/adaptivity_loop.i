# first step of creating a mesh from CT scan image with optimal refinement
# This generates the file `***_canvas.e` containing the mesh with 1block but the level of refinement/adaptivity that we want.

[Mesh]
  type = GeneratedMesh
  #dimension of your mesh
  dim = 3
  #choose the element type of your mesh (triangle,tetrahedra,hexagon...)
  elem_type = HEX
  #level of refinement, mesh resolution of 5 in x is equal to nx*2^5
  #the more levels of refinement you have the more you can coarsen the mesh
  uniform_refine = 2
  nx = 5
  ny = 5
  nz = 5
[]

[Functions]
  [./image_func]
    type = ImageFunction
    threshold = 6e4
    file_suffix = png
    #these 2 lines needs to be paste in the next step of the simulation
    #the image folder and the images selected
    file_base = stack/test_0
    #file_range = '32'
  [../]
[]

[Adaptivity]
  #the number of adaptivity steps has to be equal or inferior to the level of refinement
  #max_h_level = 1
  initial_steps = 2
  initial_marker = marker
  [./Indicators]
    [./indicator]
      type = GradientJumpIndicator
      variable = u
    [../]
  [../]
  [./Markers]
    [./marker]
      type = ErrorFractionMarker
      indicator = indicator
      # in percentage. 0 means no refinement/coarsening
      coarsen = 0.3
      refine = 0
    [../]
  [../]
[]

[Variables]
  [./u]
    order = FIRST
    family = LAGRANGE
  [../]
[]

[Problem]
  type = FEProblem
  solve = false
[]

[Executioner]
  type = Steady
[]

[ICs]
  [./u_ic]
    type = FunctionIC
    function = image_func
    variable = u
  [../]
[]

[Outputs]
  file_base = image_mesh_canvas
  execute_on = 'timestep_end'
  exodus = true
[]
