include ../../Makefile.common


# Dirs ------------------------------------------
bindir           = .
srcdir           = .
objdir           = .
incdirs          = $(incdirs_common)

# Defines ---------------------------------------
DEFS             = $(DEFS_common)

# Flags -----------------------------------------
CPPFLAGS         = $(CPPFLAGS_common)


# Source files ----------------------------------
sources          = \
    OpenGLRenderer/OpenGLRenderer_constants.cpp \
    OpenGLRenderer/FrameState.cpp \
    ODECollisionRenderer/ODECollisionRenderer_constants.cpp \
    ODECollisionRenderer/Collider.cpp \
    WorldSerialization/WorldSerialization_constants.cpp \
    WorldSerialization/WorldSerializer.cpp \
    WorldSerialization/WorldDeserializer.cpp \

# Main target -----------------------------------
MAINTARGET       = $(bindir)/librenderers.a








### ------------------------------------------------------------- ###
### --- No changes from here on!
### ---   (contains: standard targets, linking,
### ---              compiling and dependency automation)
### ------------------------------------------------------------- ###




### variable reformatting
### ------------------------------------------------------------- ###
objects          = $(sources:%.cpp=$(objdir)/%.o)
deps             = $(objects:.o=.d)


### standard targets
### ------------------------------------------------------------- ###
.PHONY: all cleanbin cleandeps cleanobj clean

all: $(MAINTARGET)
cleanbin:
	rm -f $(MAINTARGET)
cleandeps:
	rm -f $(deps)
cleanobj:
	rm -f $(objects)
clean: cleanbin cleandeps cleanobj




### linking, compiling and dependency automation
### ------------------------------------------------------------- ###
$(MAINTARGET): $(objects)
	#
	# --------   Creating the final target $@   --------
	rm -f $@
	$(AR_CMD) $@ $(objects)

ifneq ($(MAKECMDGOALS),clean)
include $(deps)
endif

$(objdir)/%.o: $(srcdir)/%.cpp
	#
	# --------   Compiling object $@   --------
	$(CXX) $(CPPFLAGS) $(DEFS:%=-D%) $(incdirs:%=-I%) -c -o $@ $<

$(objdir)/%.d: $(srcdir)/%.cpp
	#
	# --------   Generating dependencies for $@   --------
	$(DEPCC) $(DEFS:%=-D%) $(incdirs:%=-I%) $< \
	  -MM -MT $@ -MT $(basename $@).o >$@
