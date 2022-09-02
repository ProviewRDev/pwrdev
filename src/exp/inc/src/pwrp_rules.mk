
pwrp_ar = $(pwrp_lib)/libpwrp.a

source  	= $<
target  	= $@
tname		= $(notdir $(basename $@))
cc 		:= gcc
cxx             := g++
f77		:= g77
ld		:= gcc
ldxx		:= g++
linkflags	:= $(cross_compile) -g -L/usr/local/lib -L$(pwr_obj) -L$(pwrp_obj) -lm -lrt
cflags		:= $(cross_compile) -c -g -Wall -D_GNU_SOURCE -D_REENTRANT
cxxflags 	:= $(cflags) -Wno-deprecated
f77flags	:=
linkflags	:= $(cross_compile) -g -L/usr/local/lib -L$(pwr_lib) -L$(pwrp_lib) -lrt
csetos		:= -DOS_LINUX=1 -DOS=linux -DHW_X86=1 -DHW=x86
cinc		:= -I$(pwr_inc) -I$(pwrp_inc) $(PWR_EXT_INC) -I/usr/local/include -I/usr/X11R6/include -I$(jdk)/include -I$(jdk)/include/linux -I/usr/include/tirpc
f77inc		:= -I$(pwr_inc) -I$(pwrp_inc)

.SUFFIXES:

#$(pwrp_inc)/pwr_%classes.h : $(pwrp_db)/%.wb_load
#	@ echo "Create struct $(notdir $(source))"
#	@ wb_cmd create struct
#	@ co_convert -x -d $(pwrp_exe) $(source)

$(pwrp_inc)/%.h : %.h
	@ echo "Copy $(notdir $(source))"
	@ cp $(source) $(target)

$(pwrp_obj)/%.o : %.c
	@ echo "cc   $(notdir $(source))"
	@ $(cc) $(cflags) $(csetos) $(cinc) -c -o $(target) $(source)

($(pwrp_obj)/%.o) : %.c
	@ echo "cc ar $(notdir $(source))"
	@ $(cc) $(cflags) $(csetos) $(cinc) -c -o $% $(source)
	@ ar -Ur $(target) $%

$(pwrp_obj)/%.o : %.cpp
	@ echo "cc   $(notdir $(source))"
	@ $(cxx) $(cxxflags) $(csetos) $(cinc) -c -o $(target) $(source)

($(pwrp_obj)/%.o) : %.cpp
	@ echo "cc ar $(notdir $(source))"
	@ $(cxx) $(cxxflags) $(csetos) $(cinc) -c -o $% $(source)
	@ ar -Ur $(target) $%

$(pwrp_obj)/%.o : %.for
	@ echo "fortran77   $(notdir $(source))"
	@ $(f77) $(f77flags) $(f77inc) -c -o $(target) $(source)

($(pwrp_obj)/%.o) : %.for
	@ echo "fortran77 ar $(notdir $(source))"
	@ $(f77) $(f77flags) $(f77inc) -c -o $% $(source)
	@ ar -Ur $(target) $%

$(pwrp_inc)/%.h: %.msg
	@ echo "Msg to h $(source)"
	@ $(pwr_exe)/tools_msg2cmsg $(source) $(pwrp_obj)/$(tname).cmsg $(pwrp_inc)/$(tname).h

$(pwrp_obj)/%.cmsg: %.msg
	@ echo "Msg to h $(source)"
	@ $(pwr_exe)/tools_msg2cmsg $(source) $(pwrp_obj)/$(tname).cmsg $(pwrp_inc)/$(tname).h


%.o : %.cmsg
	@ echo "cmsg to obj $(source)"
	@ $(pwr_exe)/tools_cmsg2c -b $(tname) $(source) $(pwrp_tmp)/$(tname).c
	@ $(cc) $(cflags) $(csetos) $(cinc) -c -o $(target) $(pwrp_tmp)/$(tname).c

$(pwrp_root)/vax_eln/exe/%.exe : %.c
	@ echo "cc eln $< $@"
	@ rsh $(pwr_build_host_vax_eln) @pwr_cc '"'$(pwrp_root)'"' $(pwrp_projectname) "1" "1" $(tname)

$(pwrp_root)/vax_eln/obj/%.obj : %.c
	echo "cc eln $< $@"
	rsh $(pwr_build_host_vax_eln) @pwr_cc '"'$(pwrp_root)'"' $(pwrp_projectname) "1" "2" $(tname)

$(pwrp_root)/vax_eln/exe/%.sys : $(pwrp_load)/%.dat
	@ echo "ebuild $(tname)"
	@ rsh $(pwr_build_host_vax_eln) @pwr_cc '"'$(pwrp_root)'"' $(pwrp_projectname) "1" "3" $(tname)

$(pwrp_exe)/%.gif : %.gif
	@ echo "Copy $(notdir $(source))"
	@ cp $(source) $(target)

$(pwrp_exe)/%.png : %.png
	@ echo "Copy $(notdir $(source))"
	@ cp $(source) $(target)

$(pwrp_exe)/%.jpg : %.jpg
	@ echo "Copy $(notdir $(source))"
	@ cp $(source) $(target)

$(pwrp_exe)/%.gsd : %.gsd
	@ echo "Copy $(notdir $(source))"
	@ cp $(source) $(target)

$(pwrp_exe)/%.pwg : %.pwg
	@ echo "Copy $(notdir $(source))"
	@ cp $(source) $(target)

$(pwrp_exe)/%.xml : %.xml
	@ echo "Copy $(notdir $(source))"
	@ cp $(source) $(target)

$(pwrp_exe)/%.dat : %.dat
	@ echo "Copy $(notdir $(source))"
	@ cp $(source) $(target)
