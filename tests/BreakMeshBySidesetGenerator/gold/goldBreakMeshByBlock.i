[Mesh]
  [file]
    type = FileMeshGenerator
    file = ../../meshes/fractureXtruncated.msh
  []
  [New_0]
    type = BreakMeshByBlockGenerator
    input = file
  []
[]
