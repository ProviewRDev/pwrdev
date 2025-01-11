#  wbl_generic.mk -- generic make file for building wbl components
#
#  PROVIEW/R
#  Copyright (C) 1996 by Comator Process AB.
#
#  <Description>
#
ifndef wbl_generic_mk
wbl_generic_mk := 1

ifndef variables_mk
  include $(pwre_kroot)/tools/bld/src/variables.mk
endif

ifndef rules_mk
  include $(pwre_kroot)/tools/bld/src/rules.mk
endif

vpath %.wb_load $(co_source)

source_dirs := $(co_source)

wblsources := $(sort \
             $(foreach file, \
               $(foreach dir, \
                 $(source_dirs), \
                 $(wildcard $(dir)/*.wb_load) \
               ), $(notdir $(file)) \
             ) \
           )
flwsources := $(sort \
             $(foreach file, \
               $(foreach dir, \
                 $(source_dirs), \
                 $(wildcard $(dir)/*.flw) \
               ), $(notdir $(file)) \
             ) \
           )

pdfsources := $(sort \
             $(foreach file, \
               $(foreach dir, \
                 $(source_dirs), \
                 $(wildcard $(dir)/*.pdf) \
               ), $(notdir $(file)) \
             ) \
           )

wbl_sources := $(filter %.wb_load,$(wblsources))
flw_sources := $(filter %.flw,$(flwsources))
pdf_sources := $(filter %.pdf,$(pdfsources))
export_wbl := $(addprefix $(load_dir)/, $(patsubst %.wb_load, %.dbs, $(wbl_sources)))
export_flw := $(patsubst %.flw,$(load_dir)/%.flw,$(flw_sources))
export_pdf_en_us := $(patsubst %.pdf,$(doc_dir)/en_us/orm/%.pdf,$(pdf_sources))
export_pdf_sv_se := $(patsubst %.pdf,$(doc_dir)/sv_se/orm/%.pdf,$(pdf_sources))
export_wbl_dbs := $(patsubst %.wb_load,$(load_dir)/%.dbs,$(wbl_sources))
export_wbl_h := $(patsubst %.wb_load,$(inc_dir)/pwr_%classes.h,$(wbl_sources))
export_wbl_hpp := $(patsubst %.wb_load,$(inc_dir)/pwr_%classes.hpp,$(wbl_sources))
export_wbl_xtthelp_en_us := $(patsubst %.wb_load,$(exe_dir)/en_us/%_xtthelp.dat,$(wbl_sources))
export_wbl_xtthelp_sv_se := $(patsubst %.wb_load,$(exe_dir)/sv_se/%_xtthelp.dat,$(wbl_sources))
export_wbl_html_en_us := $(patsubst %.wb_load,$(doc_dir)/en_us/orm/%_allclasses.html,$(wbl_sources))
export_wbl_html_sv_se := $(patsubst %.wb_load,$(doc_dir)/sv_se/orm/%_allclasses.html,$(wbl_sources))
export_wbl_ps_en_us := $(patsubst %.wb_load,$(doc_dir)/en_us/%.ps,$(wbl_sources))
export_wbl_ps_sv_se := $(patsubst %.wb_load,$(doc_dir)/sv_se/%.ps,$(wbl_sources))
export_wbl_pdf_en_us := $(patsubst %.wb_load,$(doc_dir)/en_us/%.pdf,$(wbl_sources))
export_wbl_pdf_sv_se := $(patsubst %.wb_load,$(doc_dir)/sv_se/%.pdf,$(wbl_sources))
bld_dir := $(addprefix $(release_root)/bld/$(type_name)/, $(patsubst %.wb_load,%,$(wbl_sources)))
dbs_dependencies := $(bld_dir)/mcomp.d_wbl

clean_dbs := $(patsubst %.wb_load,clean_%.dbs,$(wbl_sources))
clean_h := $(patsubst %.wb_load,clean_%.h,$(wbl_sources))
clean_hpp := $(patsubst %.wb_load,clean_%.hpp,$(wbl_sources))
clean_xtthelp_sv_se := $(patsubst %.wb_load,clean_%_sv_se.xtthelp,$(wbl_sources))
clean_xtthelp_en_us := $(patsubst %.wb_load,clean_%_en_us.xtthelp,$(wbl_sources))
clean_html_sv_se := $(patsubst %.wb_load,clean_%_sv_se.html,$(wbl_sources))
clean_html_en_us := $(patsubst %.wb_load,clean_%_en_us.html,$(wbl_sources))
clean_ps_sv_se := $(patsubst %.wb_load,clean_%_sv_se.ps,$(wbl_sources))
clean_ps_en_us := $(patsubst %.wb_load,clean_%_en_us.ps,$(wbl_sources))

log_load 	= echo "I $(time)  build-$(pwre_bmodule), Generate loadfile $(notdir $@)" | tee -a $(logfile)
log_struct 	= echo "I $(time)  build-$(pwre_bmodule), Generate struct files $(notdir $@)" | tee -a $(logfile)
log_hpp    	= echo "I $(time)  build-$(pwre_bmodule), Generate hpp files $(notdir $@)" | tee -a $(logfile)
log_html   	= echo "I $(time)  build-$(pwre_bmodule), Generate html files $(notdir $@)" | tee -a $(logfile)
log_ps		= echo "I $(time)  build-$(pwre_bmodule), Generate postscript files $(notdir $@)" | tee -a $(logfile)
log_pdf		= echo "I $(time)  build-$(pwre_bmodule), Generate pdf files $(notdir $@)" | tee -a $(logfile)
log_xtthelp	= echo "I $(time)  build-$(pwre_bmodule), Generate xtthelp files $(notdir $@)" | tee -a $(logfile)

.SUFFIXES:

$(bld_dir)$(dir_ext) :
	@ $(mkdir) $(mkdirflags) $(basename $@)

$(load_dir)/%.dbs : %.wb_load
	@ $(log_load)
	@ export pwr_load=$(pwr_eload);\
	  wb_cmd -q -i create snapshot $(wblflags) /file=\"$(source)\"/out=\"$(target)\"/depend=\"$(dbs_dependencies)\"
	@ chmod a+w $(target)

$(inc_dir)/pwr_%classes.h : %.wb_load
	@ $(log_struct)
	@ $(co_convert) -so -d $(inc_dir) "$(source)"

$(inc_dir)/pwr_%classes.hpp : %.wb_load
	@ $(log_hpp)
	@ $(co_convert) -po -d $(inc_dir) "$(source)"

$(doc_dir)/en_us/orm/%.pdf : %.pdf
	@ $(log_cp)
	@ $(cp) $(cpflags) $(source) $(target)

$(doc_dir)/sv_se/orm/%.pdf : %.pdf
	@ $(log_cp)
	@ $(cp) $(cpflags) $(source) $(target)

$(doc_dir)/en_us/orm/%_allclasses.html : %.wb_load
	@ $(log_html)
	@ $(co_convert) -w -d $(doc_dir)/en_us/orm -g $(pwre_sroot)/wbl/mcomp/src/cnv_setup.dat "$(source)"
	@ $(co_convert) -c -d $(doc_dir)/en_us/orm $(inc_dir)/pwr_$(pwre_module)classes.h
	@ $(co_convert) -c -d $(doc_dir)/en_us/orm $(inc_dir)/pwr_$(pwre_module)classes.hpp
	@ $(co_convert) -k -d $(doc_dir)/en_us/orm -l en_us

$(doc_dir)/sv_se/orm/%_allclasses.html : %.wb_load
	@ $(log_html)
	@ $(co_convert) -w -l sv_se -d $(doc_dir)/sv_se/orm -g $(pwre_sroot)/wbl/mcomp/src/cnv_setup.dat "$(source)"
	@ $(co_convert) -c -d $(doc_dir)/sv_se/orm $(inc_dir)/pwr_$(pwre_module)classes.h
	@ $(co_convert) -c -d $(doc_dir)/sv_se/orm $(inc_dir)/pwr_$(pwre_module)classes.hpp
	@ $(co_convert) -k -d $(doc_dir)/sv_se/orm -l sv_se

$(doc_dir)/en_us/%.ps : %.wb_load
	@ $(log_ps)
	@ #$(co_convert) -q -l en_us -d $(doc_dir)/en_us "$(source)"

$(doc_dir)/sv_se/%.ps : %.wb_load
	@ $(log_ps)
	@ #$(co_convert) -q -l sv_se -d $(doc_dir)/sv_se "$(source)"

$(doc_dir)/en_us/%.pdf : %.wb_load
	@ $(log_pdf)
	@ $(co_convert) -Q -l en_us -d $(doc_dir)/en_us "$(source)"

$(doc_dir)/sv_se/%.pdf : %.wb_load
	@ $(log_pdf)
	@ $(co_convert) -Q -l sv_se -d $(doc_dir)/sv_se "$(source)"

$(exe_dir)/en_us/%_xtthelp.dat : %.wb_load
	@ $(log_xtthelp)
	@ $(co_convert) -x -d $(exe_dir)/en_us "$(source)"

$(exe_dir)/sv_se/%_xtthelp.dat : %.wb_load
	@ $(log_xtthelp)
	@ $(co_convert) -x -l sv_se -d $(exe_dir)/sv_se "$(source)"

$(exe_dir)/%.pwg : %.pwg
	@ $(log_h_h)
	@ $(cp) $(cpflags) $(source) $(target)

$(exe_dir)/%.pwsg : %.pwsg
	@ $(log_h_h)
	@ $(cp) $(cpflags) $(source) $(target)

$(load_dir)/%.flw : %.flw
	@ $(log_h_h)
	@ $(cp) $(cpflags) $(source) $(target)

.PHONY : all init copy lib exe dirs clean realclean\
         $(clean_wbl)

all : init copy exe | silent

init : dirs | silent

lib  : $(export_wbl_dbs) | silent

copy : $(export_wbl_h) \
       $(export_wbl_hpp) \
       $(export_pwg) \
       $(export_pwsg) \
       $(export_flw) \
       | silent

exe:   $(export_wbl_xtthelp_en_us) \
       $(export_wbl_xtthelp_sv_se) \
       $(export_wbl_html_en_us) \
       $(export_wbl_html_sv_se) \
       $(export_wbl_ps_en_us) \
       $(export_wbl_ps_sv_se) \
       $(export_wbl_pdf_en_us) \
       $(export_wbl_pdf_sv_se) \
       $(export_pdf_en_us) \
       $(export_pdf_sv_se) \
       | silent

dirs : $(bld_dir)$(dir_ext)

clean : \
       $(clean_xtthelp_sv_se) $(clean_xtthelp_en_us) \
       $(clean_html_sv_se) $(clean_html_en_us) \
       $(clean_ps_sv_se) $(clean_ps_en_us)

realclean : clean \
       $(clean_dbs) \
       $(clean_h) \
       $(clean_hpp) \

silent :
	@ :

$(clean_dbs) : clean_%.dbs : $(load_dir)/%.dbs
	@ echo "rm $(source)"
	@ if [ -e $(source) ]; then \
		 $(rm) $(rmflags) $(source); \
	  fi

$(clean_h) : clean_%.h : $(inc_dir)/pwr_%classes.h
	@ echo "rm $(source)"
	@ if [ -e $(source) ]; then \
		 $(rm) $(rmflags) $(source); \
	  fi

$(clean_hpp) : clean_%.hpp : $(inc_dir)/pwr_%classes.hpp
	@ echo "rm $(source)"
	@ if [ -e $(source) ]; then \
		 $(rm) $(rmflags) $(source); \
	  fi

$(clean_ps_sv_se) : clean_%_sv_se.ps : $(doc_dir)/sv_se/%.ps
	@ echo "rm $(source)"
	@ if [ -e $(source) ]; then \
		 $(rm) $(rmflags) $(source); \
	  fi

$(clean_ps_en_us) : clean_%_en_us.ps : $(doc_dir)/en_us/%.ps
	@ echo "rm $(source)"
	@ if [ -e $(source) ]; then \
		 $(rm) $(rmflags) $(source); \
	  fi

$(clean_html_sv_se) : clean_%_sv_se.html : $(doc_dir)/sv_se/orm/%_allclasses.html
	@ echo "rm $(source)"
	@ if [ -e $(source) ]; then \
		 $(rm) $(rmflags) $(source); \
	  fi

$(clean_html_en_us) : clean_%_en_us.html : $(doc_dir)/en_us/orm/%_allclasses.html
	@ echo "rm $(source)"
	@ if [ -e $(source) ]; then \
		 $(rm) $(rmflags) $(source); \
	  fi

$(clean_xtthelp_sv_se) : clean_%_sv_se.xtthelp : $(exe_dir)/sv_se/%_xtthelp.dat
	@ echo "rm $(source)"
	@ if [ -e $(source) ]; then \
		 $(rm) $(rmflags) $(source); \
	  fi

$(clean_xtthelp_en_us) : clean_%_en_us.xtthelp : $(exe_dir)/en_us/%_xtthelp.dat
	@ echo "rm $(source)"
	@ if [ -e $(source) ]; then \
		 $(rm) $(rmflags) $(source); \
	  fi

-include $(dbs_dependencies)

endif
