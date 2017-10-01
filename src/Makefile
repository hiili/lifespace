include ../Makefile.common


# Dirs ------------------------------------------
bindir           = .
tmpdir           = Makefile_tmp_ar_directory


# Source files ----------------------------------
libs             = \
    Graphics/libgraphics.a \
    Structures/libstructures.a \
    Control/libcontrol.a \
    Renderers/librenderers.a \
    Utility/libutility.a \
    Content/libcontent.a \


# Main target -----------------------------------
MAINTARGET       = $(bindir)/liblifespace.a








### ------------------------------------------------------------- ###
### --- No changes from here on!
### ------------------------------------------------------------- ###




### standard targets
### ------------------------------------------------------------- ###
.PHONY: all cleanbin clean always_execute

all: $(MAINTARGET)
cleanbin:
	rm -f $(MAINTARGET)
clean: cleanbin
	for libdir in $(dir $(libs)) ; do \
	  $(MAKE) -C $$libdir clean ; \
	done




### linking, compiling and dependency automation
### ------------------------------------------------------------- ###
$(MAINTARGET): $(libs)
	#
	# --------   Creating final target $@   --------
	mkdir $(tmpdir)
	cd $(tmpdir) && for lib in $^ ; do \
	  $(UN_AR_CMD) ../$$lib ; \
	done
	rm -f $@
	$(AR_CMD) $@ $(tmpdir)/*
	rm -f $(tmpdir)/*
	rmdir $(tmpdir)

%.a: always_execute
	$(MAKE) -C $(dir $@) $(notdir $@)
