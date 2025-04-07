''' rectangle faults intersecting amd not all touching bounding box  '''

import sys, math, copy

sys.path.append('/Applications/gmsh-4.4.1-MacOSX-sdk')
import gmsh # Install Gmsh SDK: see https://pypi.org/project/gmsh-sdk/

def generate3DMesh(mesh_filename='out.msh', params={}, do_show=False):
  ''' Generate 3D faulted mesh (without interfaces) using gmsh
      @param mesh_filename: string, filename of mesh to generate.
      @param params: dictionary of mesh parameters, with potentially:
        ['lc']: float, characteristic length at mesh points
        ["Mesh.CharacteristicLengthMin"]
        ["Mesh.CharacteristicLengthMax"]
      @param do_show: boolean, show resulting mesh in gmsh or not
  '''
  # check inputs
  defaults = {
    "lc":0.1,
    "Mesh.CharacteristicLengthMin":1e-2,
    "Mesh.CharacteristicLengthMax":1,
    "Mesh.ElementOrder":1,
  }
  # Initialise gmsh and set option
  gmsh.initialize()
  gmsh.option.setNumber("General.Terminal", 1) # to print messages on terminal
  model = gmsh.model
  factory = model.occ
  mesh = model.mesh
  #gmsh.option.setNumber("Mesh.Algorithm", 6);
  for key in ["Mesh.CharacteristicLengthMin", "Mesh.CharacteristicLengthMax",
              "Mesh.ElementOrder"]:
    if key in params:
      value = params[key]
      print  'Taking "{0}" from params'.format(key)
    else:
      value = defaults[key]
      print  'Taking "{0}" from default'.format(key)
    gmsh.option.setNumber(key, value)

  model.add("my_model") # otherwise, a new unnamed model will be created on the fly

  # user parameters
  lc = defaults['lc']
  if 'lc' in params:
    lc = params['lc']

  # construct bounding box
  factory.synchronize() # needed before meshing
  tag_box = factory.addBox(0,0,0, 1,1,1, tag=-1) # (xmin,ymin,zmin, Lx,Ly,Lz, tag)
  factory.synchronize() # needed before meshing

  # add rectangular faults
  tag_rect1 = factory.addRectangle(0., 0.1, 0.5, 0.8, 0.9, -1, 0) # x, y, z, dx, dy, tag=-1, roundedRadius=0.
  tag_rect2 = factory.addRectangle(0., 0.1, 0.5, 0.8, 0.9, -1, 0) # x, y, z, dx, dy, tag=-1, roundedRadius=0.
  factory.rotate([(2, tag_rect2)], 0.5, 0, 0.5, 0,1,0, math.pi/2.) # (dimTags, x, y, z, ax, ay, az, angle)

  # fuse all 2D features together
  outDimTags, outDimTagsMap = factory.fragment(
    [(2,tag_rect1), (2,tag_rect2)], []) #  fragment(objectDimTags, toolDimTags, tag=-1, removeObject=True, removeTool=True)
  # recover tags from fault fragments
  dimtags_rect1 = outDimTagsMap[0]
  dimtags_rect2 = outDimTagsMap[1]
  print 'dimtags_rect1:',dimtags_rect1
  print 'dimtags_rect2:',dimtags_rect2
  fault_dimtags = copy.deepcopy(dimtags_rect1)
  fault_dimtags.extend(dimtags_rect2)
  subtags_rect1 = [elt[1] for elt in dimtags_rect1]
  subtags_rect2 = [elt[1] for elt in dimtags_rect2]

  # intersect them with box
  outDimTags, outDimTagsMap = factory.intersect(
    [(3, tag_box)], fault_dimtags, -1, removeObject=False, removeTool=False) # intersect(objectDimTags, toolDimTags, tag=-1, removeObject=True, removeTool=True)
  factory.synchronize() # needed to refresh all elements

  # embed intersecting lines in outer box first
  outDimTags, outDimTagsMap = factory.fragment([(3,tag_box)], fault_dimtags) #  fragment(objectDimTags, toolDimTags, tag=-1, removeObject=True, removeTool=True)

  factory.synchronize() # needed to refresh all elements
  #bnd = model.getBoundary([(3, tag_box)], combined=False, oriented=False, recursive=False)

  # Add physical tags
  model.addPhysicalGroup(2, [11], 1) # (dim, tags, tag)
  model.setPhysicalName(2, 1, 'left') # (dim, tag, name)
  model.addPhysicalGroup(2, [17], 2)
  model.setPhysicalName(2, 2, 'right')
  model.addPhysicalGroup(2, [13], 3)
  model.setPhysicalName(2, 3, 'front')
  model.addPhysicalGroup(2, [14], 4)
  model.setPhysicalName(2, 4, 'back')
  model.addPhysicalGroup(2, [12], 5)
  model.setPhysicalName(2, 5, 'bottom')
  model.addPhysicalGroup(2, [15,16], 6)
  model.setPhysicalName(2, 6, 'top')
  model.addPhysicalGroup(2, subtags_rect1[0:1], 7)
  model.setPhysicalName(2, 7, 'fault1')
  model.addPhysicalGroup(2, subtags_rect1[1:], 8)
  model.setPhysicalName(2, 8, 'fault2')
  model.addPhysicalGroup(2, subtags_rect2, 9)
  model.setPhysicalName(2, 9, 'fault3')

  model.addPhysicalGroup(3, [1,2,3,4], 11)
  model.setPhysicalName(3, 11, 'matrix')

  #model.mesh.setSize(model.getEntities(0), 0.1);

  factory.synchronize() # needed before meshing
  mesh.generate(3) # (dim)

  gmsh.write(mesh_filename)
  print 'Generated mesh "{0}"'.format(mesh_filename)

  if do_show:
    gmsh.fltk.run()  # start gmsh
  gmsh.finalize() #  needed at the end
  # dim=1; print 'Entities of dim {0}: {1}'.format(dim, model.getEntities(dim))

########################################################################
if __name__ == "__main__":
  lc = 0.3
  params={
    "lc":lc,
    "Mesh.CharacteristicLengthMin":lc,
    "Mesh.CharacteristicLengthMax":lc,
  }
  generate3DMesh(mesh_filename='3Dcase1c.msh', params=params, do_show=True)
  print 'Finished'
