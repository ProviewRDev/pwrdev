include $(pwre_dir_symbols)

ifndef variables_mk
 include $(pwre_croot)/src/tools/bld/src/variables.mk
endif

ifndef rules_mk
 include $(pwre_croot)/src/tools/bld/src/rules.mk
endif

.SUFFIXES:


.PHONY : all init copy lib exe clean realclean

all : init copy lib exe | silent

init :

copy :

lib :

exe : $(doc_dir)/prm/index.html $(doc_dir)/pwrrt.html $(doc_dir)/pwrwb.html $(doc_dir)/pwrtest.html | silent

clean : $(clean_dox)

realclean : $(clean_html)

silent :
	@ :

$(clean_html) : clean_%.html : %.html
	@ $(rm) $(rmflags) $(doc_dir)/prm/$*.html

$(doc_dir)/prm/index.html : doxygen.c
	echo "Here in prm"
	@ $(log_doxy)
	doxygen
#	@ if [ "`eval which pdflatex`" != "" ]; then \
#	    ./repl.sh latex/*.tex; \
#	    cd latex; \
#	    make pdf; \
#	    cp refman.pdf $(pwr_doc)/en_us/man_prm.pdf; \
#	    cp classra__appl.pdf $(pwr_doc)/en_us/; \
#	    cp classrt__appl.pdf $(pwr_doc)/en_us/; \
#	    cd ..; \
#	    rm -f repl.sed; \
#	    rm -f repl.tmp; \
#	  fi; \
#	  rm -r -f latex;

$(doc_dir)/pwrrt.html : $(pwre_croot)/src/exp/pwrrt/src/pwrrtmodule.c
	@ $(log_doxy)
	@ if [ -e $(pwr_eexe)/pwrrt.so ]; then \
	    cd $(doc_dir); \
	    export PYTHONPATH=$(pwr_eexe); \
	    $(python) -m pydoc -w pwrrt; \
	  else \
	     echo "Module pwrrt doesn't exist"; \
	  fi

$(doc_dir)/pwrwb.html : $(pwre_croot)/wb/exp/pwrwb/src/pwrwbmodule.cpp
	@ $(log_doxy)
	@ if [ -e $(pwr_eexe)/pwrwb.so ]; then \
	    cd $(doc_dir); \
	    export PYTHONPATH=$(pwr_eexe); \
	    $(python) -m pydoc -w pwrwb; \
	  else \
	     echo "Module pwrwb doesn't exist"; \
	  fi

$(doc_dir)/pwrtest.html : $(pwre_croot)/test/exp/pwrtest/src/pwrtestmodule.cpp
	@ $(log_doxy)
	@ if [ -e $(pwr_eexe)/pwrtest.so ]; then \
	    cd $(doc_dir); \
	    export PYTHONPATH=$(pwr_eexe); \
	    $(python) -m pydoc -w pwrtest; \
	  else \
	     echo "Module pwrtest doesn't exist"; \
	  fi
