include Makefile.common


# default target --------------------------------
all: lib plugin_glow test examples


# the lifespace main library --------------------
lib:
	cd src && \
	$(MAKE) && \
	mkdir -p ../lib/ && \
	cp liblifespace.a ../lib/liblifespace.a && \
	for dir in `find ./ -type d` ; do \
	  mkdir -p "../include/lifespace/$$dir" ; \
	done && \
	for file in `find ./ -name '*.hpp'` ; do \
	  cp "$$file" "../include/lifespace/$$file" ; \
	done


# plugins ---------------------------------------

# GLOW plugin
plugin_glow: lib
	cd plugins && \
	$(MAKE) glow && \
	mkdir -p ../lib/ && \
	cp glow/liblifespaceglow.a \
	  ../lib/liblifespaceglow.a && \
	for dir in `find glow/ -type d` ; do \
	  mkdir -p "../include/lifespace/plugins/$$dir" ; \
	done && \
	for file in `find glow.hpp glow/ -name '*.hpp'` ; do \
	  cp "$$file" "../include/lifespace/plugins/$$file" ; \
	done

# ODE plugin




### other targets
### ------------------------------------------------------------- ###

.PHONY: all clean cleantest cleanexamples cleanbin cleanall \
    lib \
    plugin_glow \
    test examples \


# tests
test: lib plugin_glow
	$(MAKE) -C test


# examples
examples: lib plugin_glow
	$(MAKE) -C examples


# remove temporary files from the src directory
# NOTE: the plugin directories will not be cleaned!
clean:
	$(MAKE) -C src clean && \
	$(MAKE) -C plugins clean

# delete all tests
cleantest:
	$(MAKE) -C test clean

# delete all examples
cleanexamples:
	$(MAKE) -C examples clean

# delete all compiled libraries and their copied headers
cleanbin:
	rm -rf include/ lib/

# delete all derived files (does not work completely yet)
cleanall: clean cleantest cleanexamples cleanbin
