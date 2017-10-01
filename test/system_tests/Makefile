include ../../Makefile.common


# list of tests ---------------------------------
tests            = \
    MultiActor \
    ObjectDeletion \
    WorldSerializer \
    WorldDeserializer \

    # the following tests are not yet updated to use the new shared pointer \
    # conventions
    #SliderActor \
    #RecursiveCameras \
    #CollisionFeedback \
    #PrecomputedShape \
    #GeometryChanging \
    #MotoredBallJoint \
    #UniversalJoint \
    #SubspaceOffsets \
    #Performance \
    #ODELocator_performance \

    # the following tests need the erp-patch:
    #BallJoint \
    #FixedJoint \
    #ODECollisionRenderer \
    #ConnectorAligning \
    #StaticConnectors \
    #ODECollisionRenderer_rescan \
    #ConnectorInhibitCollisions \
    # the following tests are outdated:
    #UserInterface \
    #ODEWorld \








### ------------------------------------------------------------- ###
### --- No changes from here on!
### ------------------------------------------------------------- ###
.PHONY: all clean tests




### init some variables
### ------------------------------------------------------------- ###
ifeq ($(UNAME),Cygwin)
all_targets      = $(tests:%=%/lifespace.exe)
else
all_targets      = $(tests:%=%/lifespace)
endif


### default target
### ------------------------------------------------------------- ###
all: $(all_targets)


### cleanup
### ------------------------------------------------------------- ###
clean:
	for test in $(tests) ; do \
	  $(MAKE) -C $$test clean ; \
	done


### compiling
### ------------------------------------------------------------- ###
$(all_targets):
	$(MAKE) -C $(dir $@)
