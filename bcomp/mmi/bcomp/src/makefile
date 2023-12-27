include $(pwre_dir_symbols)

ifndef variables_mk
 include $(pwre_kroot)/tools/bld/src/variables.mk
endif

ifndef rules_mk
 include $(pwre_kroot)/tools/bld/src/rules.mk
endif

vpath %.pwg $(co_source)
vpath %.pwsg $(co_source)

source_dirs := $(co_source)

pwg_c_sources := $(sort \
             $(foreach file, \
               $(foreach dir, \
                 $(source_dirs), \
                 $(wildcard $(dir)/pwr_c_*.pwg) \
               ), $(notdir $(file)) \
             ) \
           )

pwsg_sources := $(sort \
             $(foreach file, \
               $(foreach dir, \
                 $(source_dirs), \
                 $(wildcard $(dir)/bcomp_*.pwsg) \
               ), $(notdir $(file)) \
             ) \
           )

ctpwsg_sources := $(sort \
             $(foreach file, \
               $(foreach dir, \
                 $(source_dirs), \
                 $(wildcard $(dir)/bcompct_*.pwsg) \
               ), $(notdir $(file)) \
             ) \
           )


export_pwg := $(addprefix $(exe_dir)/,$(pwg_c_sources)) $(addprefix $(exe_dir)/,$(pwg_t_sources))
export_pwsg := $(addprefix $(exe_dir)/,$(pwsg_sources)) $(addprefix $(exe_dir)/,$(ctpwsg_sources))
clean_pwsg := $(patsubst %.pwg,clean_%.pwsg,$(sources))
clean_pwg := $(patsubst %.pwg,clean_%.pwg,$(sources))
export_ge := $(export_pwg) $(export_pwsg)
clean_ge := $(clean_pwg) $(clean_pwsg)
.SUFFIXES:


.PHONY : all init copy lib exe clean realclean \
         $(clean_ge)

all : init copy lib exe | silent

init : silent

copy : $(export_ge)\
       | silent

lib : silent

exe : silent

clean :

realclean : $(clean_ge)

silent :
	@ :

$(clean_pwg) : clean_%.pwg : %.pwg
	@ $(rm) $(rmflags) $(exe_dir)/$*.pwg

$(clean_pwsg) : clean_%.pwsg : %.pwsg
	@ $(rm) $(rmflags) $(exe_dir)/$*.pwsg
