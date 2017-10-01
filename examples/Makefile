include ../Makefile.common


# list of examples ------------------------------
examples         = \
    #ODE_basics \








### ------------------------------------------------------------- ###
### --- No changes from here on!
### ------------------------------------------------------------- ###
.PHONY: all clean examples




### init some variables
### ------------------------------------------------------------- ###
UNAME            = $(shell uname)
ifneq (,$(findstring CYGWIN,$(UNAME)))
UNAME            = $(shell uname -o)
endif

ifeq ($(UNAME),Cygwin)
all_targets      = $(examples:%=%/lifespace.exe)
else
all_targets      = $(examples:%=%/lifespace)
endif


### default target
### ------------------------------------------------------------- ###
all: $(all_targets)


### cleanup
### ------------------------------------------------------------- ###
clean:
	for example in $(examples) ; do \
	  $(MAKE) -C $$example clean ; \
	done


### compiling
### ------------------------------------------------------------- ###
%:
	$(MAKE) -C $(dir $@)
