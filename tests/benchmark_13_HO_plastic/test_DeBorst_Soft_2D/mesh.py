# -*- coding: utf-8 -*-
"""
Created on Thu Sep 28 15:09:38 2023

@author: delpierrena
"""

import gmsh
import sys

#1) generate the mesh
def generate_mesh():
    gmsh.initialize()
    gmsh.model.add("test")

    lc = 0.75 #taille du mesh
    lc2 = 1 

    gmsh.model.geo.addPoint(0, 0, 0, lc, 1)
    gmsh.model.geo.addPoint(60, 0, 0, lc, 2)
    gmsh.model.geo.addPoint(60, 60, 0, lc, 3)
    gmsh.model.geo.addPoint(0, 60, 0, lc, 4)
 

    #create lines between those points
    gmsh.model.geo.addLine(1, 2, 1)
    gmsh.model.geo.addLine(2, 3, 2)
    gmsh.model.geo.addLine(3, 4, 3)
    gmsh.model.geo.addLine(4, 1, 4)
    
    gmsh.model.geo.addCurveLoop([1, 2, 3, 4], 1)
    # create the surface : it is a list of curve loops 
    pl = gmsh.model.geo.addPlaneSurface([1], 1)

    # it is necessary to synchronise those informations with the Gmsh model, creating the relevant data structure
    gmsh.model.geo.synchronize()
    
    boundary_down = gmsh.model.addPhysicalGroup(1, [1]) #dimension 1, lignes prises
    boundary_right_locked = gmsh.model.addPhysicalGroup(1, [2])
    boundary_top = gmsh.model.addPhysicalGroup(1, [3]) #dimension 1, lignes prises
    boundary_left = gmsh.model.addPhysicalGroup(1, [4]) #dimension 1, lignes prises

    gmsh.model.setPhysicalName(1,boundary_down , "bottom")
    gmsh.model.setPhysicalName(1,boundary_right_locked , "right")
    gmsh.model.setPhysicalName(1,boundary_top , "top")
    gmsh.model.setPhysicalName(1,boundary_left , "left")

    ps1 = gmsh.model.addPhysicalGroup(2, [1])
    gmsh.model.setPhysicalName(2, ps1, "sol")
    
    gmsh.model.mesh.generate(2)
    gmsh.model.mesh.optimize('Laplace2D')  #optimiser le mesh 
    gmsh.write("mesh_075.msh")
        
    if '-nopopup' not in sys.argv:
        gmsh.fltk.run()
    gmsh.finalize()

generate_mesh()
