include $(pwre_dir_symbols)

-include $(pwre_kroot)/tools/bld/src/$(type_name)_generic.mk

ifndef variables_mk
 include $(pwre_kroot)/tools/bld/src/variables.mk
endif

export_cpp = $(bld_dir)/stdsoap2.o
export_h = $(inc_dir)/stdsoap2.h $(inc_dir)/Service.nsmap

# stdsoap2 is third-party (gSOAP) — suppress warnings we cannot fix
stdsoap2_cxxflags := $(cxxflags) -w

lib : $(export_lib) export_lib_local

export_lib_local : $(export_cpp) $(export_h)
	@ $(ar) -r $(export_lib) $(export_cpp)

$(bld_dir)/stdsoap2.o : stdsoap2.cpp
	@ $(log_c_obj)
	@ $(cxx) $(stdsoap2_cxxflags) $(csetos) $(cinc) $(cobj) $(source)

$(inc_dir)/stdsoap2.h : stdsoap2.h

$(inc_dir)/Service.nsmap : Service.nsmap
	@ $(log_exp)
	@ $(cp) $(cpflags) $(source) $(target)
