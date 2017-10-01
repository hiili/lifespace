include Makefile.common


# default target --------------------------------
all: lib test


# the lifespace main library --------------------
lib:
	cd src && \
	$(MAKE) && \
	mkdir -p ../lib/ && \
	cp libsim.a ../lib/libsim.a && \
	for dir in `find ./ -type d` ; do \
	  mkdir -p "../include/sim/$$dir" ; \
	done && \
	for file in `find ./ -name '*.hpp'` ; do \
	  cp "$$file" "../include/sim/$$file" ; \
	done


### other targets
### ------------------------------------------------------------- ###

.PHONY: all clean cleantest cleanexamples cleanbin cleanall \
    lib \
    test \


# tests
test: lib
	$(MAKE) -C test


# delete all compiled libraries and their copied headers
cleanlib:
	$(MAKE) -C src clean
	rm -rf include/ lib/

# delete all tests
cleantest:
	$(MAKE) -C test clean

# delete all derived files
clean: cleanlib cleantest
