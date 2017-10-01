include ../../Makefile.common


# Dirs ------------------------------------------
bindir           = .
srcdir           = .
objdir           = .
incdirs          = ../../include $(incdirs_common)
libdirs          = ../../lib $(libdirs_common)


# Libs ------------------------------------------
libs             = lifespace ode glow \
    $(libs_opengl) $(libs_glut) $(libs_std)


# Defines ---------------------------------------
#   - NODEBUG is used by GLOW
DEFS             = $(DEFS_common) \
    GLOW_PLATFORM_LINUX \
    GLOW_PLATFORM_LITTLEENDIAN \
    NODEBUG \


# Flags -----------------------------------------
CPPFLAGS         = $(CPPFLAGS_common)
LINKFLAGS        = $(LINKFLAGS_common)


# Source files ----------------------------------
sources          = \
    main.cpp \
    testBasisMatrix.cpp \
    testObject.cpp \
    testSubspace.cpp \
    testActor.cpp \
    testBasicLocator.cpp \
    testInertiaLocator.cpp \


# Main target -----------------------------------
MAINTARGET       = $(bindir)/test








### ------------------------------------------------------------- ###
### --- No changes from here on!
### ---   (contains: standard targets, linking,
### ---              compiling and dependency automation)
### ------------------------------------------------------------- ###




### variable reformatting
### ------------------------------------------------------------- ###
objects          = $(sources:%.cpp=$(objdir)/%.o)
deps             = $(objects:.o=.d)
ifeq ($(UNAME),Cygwin)
MAINTARGET      := $(MAINTARGET).exe
endif


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
	# --------   Linking the final target $@   --------
	$(CXX) $(LINKFLAGS) -o $@ $(objects) $(libdirs:%=-L%) $(libs:%=-l%)

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
