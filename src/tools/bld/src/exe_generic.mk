#  exe_generic.mk -- generic makefile for executables
#
#  PROVIEW/R
#  Copyright (C) 1996 by Comator Process AB.
#
#  To build an executable.
#
ifndef exe_generic_mk
exe_generic_mk := 1

ifndef variables_mk
  include $(pwre_kroot)/tools/bld/src/variables.mk
endif

ifndef rules_mk
  include $(pwre_kroot)/tools/bld/src/rules.mk
endif

ifndef link_rule_mk
  link_rule := link_rule.mk
  -include $(link_rule)
endif
ifndef link_rule_mk
  link_rule := $(pwre_kroot)/tools/bld/src/link_rule_generic.mk
  include $(link_rule)
endif

vpath %.c $(co_source)
vpath %.cpp $(co_source)
vpath %.cqt $(co_source)
vpath %.l $(co_source)
vpath %.h $(co_source)
vpath %.hpp $(co_source)

source_dirs := $(co_source)

c_sources := $(sort \
             $(foreach file, \
               $(foreach dir, \
                 $(source_dirs), \
                 $(wildcard $(dir)/*.c $(dir)/*.cpp $(dir)/*.cqt) \
               ), $(notdir $(file)) \
             ) \
           )

cqt_sources := $(sort \
             $(foreach file, \
               $(foreach dir, \
                 $(source_dirs), \
                 $(wildcard $(dir)/*.cqt) \
               ), $(notdir $(file)) \
             ) \
           )

l_sources := $(sort \
             $(foreach file, \
               $(foreach dir, \
                 $(source_dirs), \
                 $(wildcard $(dir)/*.l) \
               ), $(notdir $(file)) \
             ) \
           )

h_includes := $(sort \
             $(foreach file, \
               $(foreach dir, \
                 $(source_dirs), \
                 $(wildcard $(dir)/$(comp_name)*.h) \
               ), $(notdir $(file)) \
             ) \
           )

hpp_includes := $(sort \
             $(foreach file, \
               $(foreach dir, \
                 $(source_dirs), \
                 $(wildcard $(dir)/$(comp_name)*.hpp) \
               ), $(notdir $(file)) \
             ) \
           )

css_sources := $(sort \
             $(foreach file, \
               $(foreach dir, \
                 $(source_dirs), \
                 $(wildcard $(dir)/$(comp_name)*.css) \
               ), $(notdir $(file)) \
             ) \
           )

exe_name   := $(comp_name)
exe_flv_name   := $(comp_name)
ifneq ($(src_name),src)
  exe_flv_name := $(comp_name)_$(src_name)
endif
exe_source := $(filter $(exe_name).%, $(c_sources))

c_sources  := $(filter-out $(exe_source), $(c_sources))

export_exe := $(exe_dir)/$(pre_so)$(exe_flv_name)$(exe_ext)
#export_obj := $(obj_dir)/$(exe_name)$(obj_ext)
export_obj := $(bld_dir)/$(exe_name)$(obj_ext)
export_flv_obj := $(bld_dir)/$(exe_name)$(obj_ext)
ifneq ($(src_name),src)
  export_flv_obj := $(bld_dir)/$(exe_name)_$(src_name)$(obj_ext)
endif
export_includes := $(addprefix $(inc_dir)/,$(h_includes) $(hpp_includes))
export_css := $(addprefix $(load_dir)/,$(css_sources))

l_targets := $(addprefix $(bld_dir)/,$(basename $(l_sources)))
l_targets := $(addsuffix $(c_ext),$(l_targets))

cqt_objects := $(patsubst %.cqt, %_moc.o, $(cqt_sources))
cqt_objects := $(addprefix $(bld_dir)/,$(basename $(cqt_objects)))

objects := $(addprefix $(bld_dir)/,$(basename $(c_sources)))
objects += $(cqt_objects)
objects += $(addprefix $(bld_dir)/,$(basename $(l_sources)))
objects := $(strip $(addsuffix $(obj_ext),$(objects)))

lc_src_dep := $(objects:$(obj_ext)=$(d_ext))
exe_src_dep := $(obj_dir)/$(exe_name)$(d_ext)
source_dependencies := $(lc_src_dep) $(exe_src_dep)
link_dependencies := $(bld_dir)/$(exe_flv_name)$(d_ld_ext)

clean_h_includes := $(patsubst %.h,clean_%.h, $(h_includes))
clean_hpp_includes := $(patsubst %.hpp,clean_%.hpp, $(hpp_includes))

.PHONY : all init copy lib exe clean realclean \
         dirs $(clean_includes) clean_bld clean_dirs\
	 clean_exe

all : init copy lib exe | silent

init : dirs | silent

copy : $(export_includes) $(export_css) | silent

lib :

exe : $(l_targets) $(objects) $(export_flv_obj) $(export_exe) | silent

dirs : $(bld_dir)$(dir_ext)

$(bld_dir)$(dir_ext) :
	@ $(mkdir) $(mkdirflags) $(basename $@)

clean : clean_exe clean_bld

realclean : clean $(clean_h_includes) $(clean_hpp_includes) clean_dirs

$(export_exe) : $(link_rule) $(export_flv_obj) $(objects)
	@ $(log_link_exe)
	@ if [ "$(link)" != ":" ]; then \
	  $(link)  | grep $(pwre_broot) | awk -f $(pwre_kroot)/tools/bld/src/ld_trace_to_d.awk -v target=$(target) > $(link_dependencies); \
	fi
	@ $(linkcp);

clean_exe :
	@ if [ -e $(export_exe) ]; then \
		$(rm) $(export_exe); \
	fi
	@ if [ -e $(export_flv_obj) ]; then \
		$(rm) $(export_flv_obj); \
	fi

clean_bld :
	@ $(rm) $(rmflags) $(bld_dir)/*

clean_dirs :
	@ $(rm) $(rmflags) $(bld_dir)

silent:
	@ :

$(clean_h_includes) : clean_%.h : %.h
	@ $(rm) $(rmflags) $(inc_dir)/$*.h

$(clean_hpp_includes) : clean_%.hpp : %.hpp
	@ $(rm) $(rmflags) $(inc_dir)/$*.hpp

-include $(source_dependencies)
-include $(link_dependencies)

endif
