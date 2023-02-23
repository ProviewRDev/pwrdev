include $(pwre_dir_symbols)

-include $(pwre_kroot)/tools/bld/src/$(type_name)_generic.mk

exe :  $(doc_dir)/sv_se/orm/pwr_dataqclasses_h.html \
       $(doc_dir)/en_us/orm/pwr_dataqclasses_h.html \
       $(doc_dir)/sv_se/orm/pwr_dataqclasses_hpp.html \
       $(doc_dir)/en_us/orm/pwr_dataqclasses_hpp.html \
       $(doc_dir)/en_us/orm/dataq_plc_c.html \
       $(doc_dir)/sv_se/orm/dataq_plc_c.html \
       $(doc_dir)/en_us/orm/dataq_plc_macro_h.html \
       $(doc_dir)/sv_se/orm/dataq_plc_macro_h.html

$(doc_dir)/sv_se/orm/pwr_dataqclasses_h.html : $(inc_dir)/pwr_dataqclasses.h
	@ $(log_html)
	@ $(co_convert) -c -d $(doc_dir)/sv_se/orm "$(source)"

$(doc_dir)/en_us/orm/pwr_dataqclasses_h.html : $(inc_dir)/pwr_dataqclasses.h
	@ $(log_html)
	@ $(co_convert) -c -d $(doc_dir)/en_us/orm "$(source)"

$(doc_dir)/sv_se/orm/pwr_dataqclasses_hpp.html : $(inc_dir)/pwr_dataqclasses.hpp
	@ $(log_html)
	@ $(co_convert) -c -d $(doc_dir)/sv_se/orm "$(source)"

$(doc_dir)/en_us/orm/pwr_dataqclasses_hpp.html : $(inc_dir)/pwr_dataqclasses.hpp
	@ $(log_html)
	@ $(co_convert) -c -d $(doc_dir)/en_us/orm "$(source)"

$(doc_dir)/en_us/orm/dataq_plc_c.html : $(pwre_sroot)/lib/dataq/src/dataq_plc.c
	@ $(log_html)
	@ $(co_convert) -c -d $(doc_dir)/en_us/orm "$(source)"

$(doc_dir)/sv_se/orm/dataq_plc_c.html : $(pwre_sroot)/lib/dataq/src/dataq_plc.c
	@ $(log_html)
	@ $(co_convert) -c -d $(doc_dir)/sv_se/orm "$(source)"

$(doc_dir)/en_us/orm/dataq_plc_macro_h.html : $(pwre_sroot)/lib/dataq/src/dataq_plc_macro.h
	@ $(log_html)
	@ $(co_convert) -c -d $(doc_dir)/en_us/orm "$(source)"

$(doc_dir)/sv_se/orm/dataq_plc_macro_h.html : $(pwre_sroot)/lib/dataq/src/dataq_plc_macro.h
	@ $(log_html)
	@ $(co_convert) -c -d $(doc_dir)/sv_se/orm "$(source)"

