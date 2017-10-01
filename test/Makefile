include ../Makefile.common


# list of tests ---------------------------------
tests            = \
    system_tests \
    # the following tests are outdated:
    #unit_tests \








### ------------------------------------------------------------- ###
### --- No changes from here on!
### ------------------------------------------------------------- ###
.PHONY: all clean tests always_execute




### default target
### ------------------------------------------------------------- ###
all: $(tests)


### cleanup
### ------------------------------------------------------------- ###
clean:
	for test in $(tests) ; do \
	  $(MAKE) -C $$test clean ; \
	done


### compiling
### ------------------------------------------------------------- ###
$(tests): always_execute
	$(MAKE) -C $@
