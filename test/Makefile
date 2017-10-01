include ../Makefile.common


# list of tests ---------------------------------
tests            = \
    leg_basics \
    leg_hopper \








### ------------------------------------------------------------- ###
### --- No changes from here on!
### ------------------------------------------------------------- ###
.PHONY: all clean tests always_execute




### init some variables
### ------------------------------------------------------------- ###
ifeq ($(UNAME),Cygwin)
all_targets      = $(tests:%=%/test.exe)
else
all_targets      = $(tests:%=%/test)
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
$(all_targets): always_execute
	$(MAKE) -C $(dir $@)
