ifndef link_rule_mk
link_rule_mk := 1

link = $(ld) $(domap) -o $(export_exe) $(export_obj) $(objects) \
	$(rt_msg_eobjs) $(pwr_eobj)/rt_io_user.o $(pwre_conf_libdir) \
	$(pwre_conf_libpwrrt) $(pwre_conf_lib) $(elinkflags)

endif
