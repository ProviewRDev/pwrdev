ifndef link_rule_mk
link_rule_mk := 1

link = $(ldxx) $(elinkflags) $(domap) -o $(pwr_exe)/pwr_rtmon_gtk \
	$(bld_dir)/pwr_rtmon_gtk.o \
	$(wb_msg_eobjs) $(rt_msg_eobjs) \
	$(pwre_conf_libdir) $(pwre_conf_libpwrxttgtk) $(pwre_conf_libpwrxtt) $(pwre_conf_libgtk) \
	$(pwre_conf_libpwrrt) $(pwre_conf_lib) $(pwre_conf_libpwrrt) -lrt

endif
