include $(pwre_dir_symbols)

-include $(pwre_kroot)/tools/bld/src/$(type_name)_generic.mk

-include special.mk

copy :        $(exe_dir)/profinet_devices.xml

$(exe_dir)/profinet_devices.xml : profinet_devices.xml
	@ $(log_h_h)
	@ $(cp) $(cpflags) $(source) $(target)
