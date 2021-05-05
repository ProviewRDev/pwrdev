ifndef link_rule_mk
link_rule_mk := 1

ifeq ($(PWRE_CONF_MQTT),1)
  link = $(ldxx) $(elinkflags) $(domap) -o $(export_exe) \
	$(export_obj) $(objects) $(rt_msg_eobjs) \
        $(pwre_conf_libdir) $(pwre_conf_libpwrremote) $(pwre_conf_libpwrrt) \
	-lmosquitto $(pwre_conf_lib)
else 
  log_link_exe = echo "$(exe_flv_name): MQTT not installed"
  link = :
endif

endif
