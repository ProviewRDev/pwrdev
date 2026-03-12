include $(pwre_dir_symbols)

-include $(pwre_kroot)/tools/bld/src/$(type_name)_generic.mk

copy :        $(exe_dir)/pn_dcp_tool_style.css

$(exe_dir)/pn_dcp_tool_style.css : pn_dcp_tool_style.css
	@ $(log_l_h_cp)
	@ $(cp) $(cpflags) $(source) $(target)