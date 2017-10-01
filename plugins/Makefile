include ../Makefile.common


# list of plugins -------------------------------
plugins          = \
    glow \
    #ode \








### ------------------------------------------------------------- ###
### --- No changes from here on!
### ------------------------------------------------------------- ###
.PHONY: all clean always_execute




### default target
### ------------------------------------------------------------- ###
all: $(plugins)


### cleanup
### ------------------------------------------------------------- ###
clean:
	for plugin in $(plugins) ; do \
	  $(MAKE) -C $$plugin clean ; \
	done


### compiling
### ------------------------------------------------------------- ###
$(plugins): always_execute
	$(MAKE) -C $@
