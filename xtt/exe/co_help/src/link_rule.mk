ifndef link_rule_mk
link_rule_mk := 1

  link = $(ldxx) $(elinkflags) $(domap) -o $(export_exe) \
	$(export_obj) \
	$(pwre_conf_libdir) $(pwre_conf_libpwrco) $(pwre_conf_lib)
endif
