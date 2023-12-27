include $(pwre_dir_symbols)

ifndef variables_mk
 include $(pwre_kroot)/tools/bld/src/variables.mk
endif

ifndef rules_mk
 include $(pwre_kroot)/tools/bld/src/rules.mk
endif

vpath %.pwsg $(co_source)

source_dirs := $(co_source)

pwsg_sources := $(sort \
             $(foreach file, \
               $(foreach dir, \
                 $(source_dirs), \
                 $(wildcard $(dir)/*.pwsg) \
               ), $(notdir $(file)) \
             ) \
           )


export_pwsg := $(addprefix $(exe_dir)/,$(pwsg_sources))
clean_pwsg := $(patsubst %.pwg,clean_%.pwsg,$(sources))
.SUFFIXES:


.PHONY : all init copy lib exe clean realclean

all : init copy lib exe | silent

init : silent

copy : $(export_pwsg)\
       | silent

lib : silent

exe : silent

clean :

realclean : $(clean_pwsg)

silent :
	@ :

$(clean_pwsg) : clean_%.pwsg : %.pwsg
	@ $(rm) $(rmflags) $(exe_dir)/$*.pwsg

