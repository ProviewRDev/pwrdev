ifndef link_rule_mk
link_rule_mk := 1

link = $(ldxx) -L$(pwr_elib) $(elinkflags) $(domap) -o $(pwr_exe)/profinet_viewer2_gtk \
	$(bld_dir)/profinet_viewer2_gtk.o \
	$(bld_dir)/pn_dcp_tool_gtk.o \
	$(bld_dir)/network_topology_window.o \
	$(bld_dir)/pn_dcp_tool.o \
	$(bld_dir)/topology-crawler.o \
	$(bld_dir)/dcp_protocol.o \
	$(bld_dir)/configured_devices.o \
	$(wb_msg_eobjs) $(rt_msg_eobjs) \
	$(pwr_eobj)/rt_io_user.o \
	$(pwre_conf_libdir) $(pwre_conf_libpwrxttgtk) $(pwre_conf_libpwrxtt) $(pwre_conf_libgtk) \
	$(pwre_conf_libpwrrt) $(pwre_conf_lib) -lnetsnmp

endif