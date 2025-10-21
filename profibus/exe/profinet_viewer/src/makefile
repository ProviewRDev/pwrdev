include $(pwre_dir_symbols)

-include $(pwre_kroot)/tools/bld/src/$(type_name)_generic.mk

copy :        $(exe_dir)/profinet_devices.xml

$(exe_dir)/profinet_devices.xml : profinet_devices.xml
	@ $(log_l_h_cp)
	@ $(cp) $(cpflags) $(source) $(target)
