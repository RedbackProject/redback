#to create a library by copying after compile in the user files
#ADDITIONAL_LIBS := /libre/hrattez/materials/lib/libDP_cohesion2D.so
#ADDITIONAL_LIBS := /Users/hadrienrattez/projects/materials/lib/myLib.dylib
#COSSERAT_LIBS := /libre/hrattez/cosserat_dynlib/
# obj-suffix := lo

COSSERAT_LIBS := $(HOME)/projects/materials

multisurfaceplasticityhard_DIR := $(COSSERAT_LIBS)
multisurfaceplasticityhard_srcfiles  += $(multisurfaceplasticityhard_DIR)/multisurfaceplasticityhard.f
multisurfaceplasticityhard_objects   := $(patsubst %.f, %.$(obj-suffix), $(multisurfaceplasticityhard_srcfiles))
multisurfaceplasticityhard_deps      := $(patsubst %.$(obj-suffix), %.$(obj-suffix).d, $(multisurfaceplasticityhard_objects))
multisurfaceplasticityhard_LIB       := $(multisurfaceplasticityhard_DIR)/libmultisurfaceplasticityhard-$(METHOD).la

app_INCLUDES += -I$(multisurfaceplasticityhard_DIR)
app_LIBS += $(multisurfaceplasticityhard_LIB)


#command in the case of a Fortran file 77
$(multisurfaceplasticityhard_LIB): $(multisurfaceplasticityhard_objects)
	@echo "Linking Library "$@"..."
	@$(libmesh_LIBTOOL) --tag=F77 $(LIBTOOLFLAGS) --mode=link \
	$(libmesh_F77) $(libmesh_FFLAGS) -o $@ $(multisurfaceplasticityhard_objects) $(libmesh_LDFLAGS) $(EXTERNAL_FLAGS) -rpath $(multisurfaceplasticityhard_DIR) -llapack -lblas
	@$(libmesh_LIBTOOL) --mode=install install -c  $(multisurfaceplasticityhard_LIB) $(multisurfaceplasticityhard_DIR)


$(app_EXEC): $(multisurfaceplasticityhard_LIB)
-include $(multisurfaceplasticityhard_deps)

# %.lo: %.f %.so
# 	$(libmesh_LIBTOOL) --tag=F77 $(LIBTOOLFLAGS) --mode=compile $(libmesh_F77) $(libmesh_FFLAGS) -c  -T  $< -o $@

cleanmultisurfaceplasticityhard:
			@rm -f $(multisurfaceplasticityhard_objects)
			@rm -f $(multisurfaceplasticityhard_deps)
			@rm -f $(multisurfaceplasticityhard_LIB)
			@rm -f $(multisurfaceplasticityhard_DIR)/libmultisurfaceplasticityhard-$(METHOD)*.dylib
			# @rm -f $(multisurfaceplasticityhard_DIR)/libmultisurfaceplasticityhard-$(METHOD)*.so
			@rm -f $(multisurfaceplasticityhard_DIR)/libmultisurfaceplasticityhard-$(METHOD)*.a
