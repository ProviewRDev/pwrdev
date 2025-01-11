include $(pwre_dir_symbols)

ifndef variables_mk
 include $(pwre_kroot)/tools/bld/src/variables.mk
endif

ifndef rules_mk
 include $(pwre_kroot)/tools/bld/src/rules.mk
endif

# include $(pwre_kroot)/tools/bld/src/$(type_name)_generic.mk


log_wbl 	= echo "I $(time)  build-$(pwre_bmodule), Generate wbl file $(notdir $@)" | tee -a $(logfile)
log_pal 	= echo "I $(time)  build-$(pwre_bmodule), Generate palette file $(notdir $@)" | tee -a $(logfile)
log_ge_com 	= echo "I $(time)  build-$(pwre_bmodule), Generate pwg files from ge_com files" | tee -a $(logfile)

all:   copy

init:

lib:

exe:

copy:  $(pwr_exe)/pwr_z2m_palette.cnf \
       $(pwr_inc)/pwr_zigbee2mqttclasses.h

gen:   $(pwre_sroot)/wbl/z2m/src/zigbee2mqtt1.wb_load \
       $(pwre_sroot)/wbl/z2m/src/zigbee2mqtt2.wb_load \
       $(pwre_sroot)/wbl/z2m/src/zigbee2mqtt3.wb_load \
       $(pwre_sroot)/wbl/z2m/src/zigbee2mqtt4.wb_load \
       $(pwre_sroot)/wbl/z2m/src/zigbee2mqtt5.wb_load \
       $(pwre_sroot)/wbl/z2m/src/zigbee2mqtt6.wb_load \
       $(pwre_sroot)/wbl/z2m/src/zigbee2mqtt7.wb_load \
       $(pwre_sroot)/wbl/z2m/src/zigbee2mqtt8.wb_load \
       $(pwre_sroot)/wbl/z2m/src/zigbee2mqtt9.wb_load \
       $(pwre_sroot)/exp/z2m/src/pwr_z2m_palette.cnf \
       $(pwre_sroot)/mmi/z2m/src/pwr_c_abl_rb56ac.pwg


clean: clean_z2m

$(pwr_inc)/pwr_zigbee2mqttclasses.h : $(pwre_sroot)/exp/z2m/src/pwr_zigbee2mqttclasses.h
	@ $(log_cp)
	@ $(cp) $(source) $(target)

$(pwre_sroot)/wbl/z2m/src/zigbee2mqtt1.wb_load : $(pwre_sroot)/exp/z2m/src/z2m_devices.dat
	@ $(log_wbl)
	@ tools_z2m_gen -n Zigbee2mqtt1 -v 0.0.250.10 -f a\*,b\*,c\* -d $(pwre_sroot)/wbl/z2m1/src -c $(pwre_sroot)/lib/z2m/src -g $(pwre_sroot)/mmi/z2m/src -i $(source)

$(pwre_sroot)/wbl/z2m/src/zigbee2mqtt2.wb_load : $(pwre_sroot)/exp/z2m/src/z2m_devices.dat
	@ $(log_wbl)
	@ tools_z2m_gen -n Zigbee2mqtt2 -v 0.0.250.11 -f d\*,e\*,f\* -d $(pwre_sroot)/wbl/z2m2/src -c $(pwre_sroot)/lib/z2m/src -g $(pwre_sroot)/mmi/z2m/src -i $(source)

$(pwre_sroot)/wbl/z2m/src/zigbee2mqtt3.wb_load : $(pwre_sroot)/exp/z2m/src/z2m_devices.dat
	@ $(log_wbl)
	@ tools_z2m_gen -n Zigbee2mqtt3 -v 0.0.250.12 -f g\*,h\*,i\* -d $(pwre_sroot)/wbl/z2m3/src -c $(pwre_sroot)/lib/z2m/src -g $(pwre_sroot)/mmi/z2m/src -i $(source)

$(pwre_sroot)/wbl/z2m/src/zigbee2mqtt4.wb_load : $(pwre_sroot)/exp/z2m/src/z2m_devices.dat
	@ $(log_wbl)
	@ tools_z2m_gen -n Zigbee2mqtt4 -v 0.0.250.13 -f j\*,k\*,l\* -d $(pwre_sroot)/wbl/z2m4/src -c $(pwre_sroot)/lib/z2m/src -g $(pwre_sroot)/mmi/z2m/src -i $(source)

$(pwre_sroot)/wbl/z2m/src/zigbee2mqtt5.wb_load : $(pwre_sroot)/exp/z2m/src/z2m_devices.dat
	@ $(log_wbl)
	@ tools_z2m_gen -n Zigbee2mqtt5 -v 0.0.250.14 -f m\*,n\*,o\* -d $(pwre_sroot)/wbl/z2m5/src -c $(pwre_sroot)/lib/z2m/src -g $(pwre_sroot)/mmi/z2m/src -i $(source)

$(pwre_sroot)/wbl/z2m/src/zigbee2mqtt6.wb_load : $(pwre_sroot)/exp/z2m/src/z2m_devices.dat
	@ $(log_wbl)
	@ tools_z2m_gen -n Zigbee2mqtt6 -v 0.0.250.15 -f p\*,q\* -d $(pwre_sroot)/wbl/z2m6/src -c $(pwre_sroot)/lib/z2m/src -g $(pwre_sroot)/mmi/z2m/src -i $(source)

$(pwre_sroot)/wbl/z2m/src/zigbee2mqtt7.wb_load : $(pwre_sroot)/exp/z2m/src/z2m_devices.dat
	@ $(log_wbl)
	@ tools_z2m_gen -n Zigbee2mqtt7 -v 0.0.250.16 -f r\*,s\* -d $(pwre_sroot)/wbl/z2m7/src -c $(pwre_sroot)/lib/z2m/src -g $(pwre_sroot)/mmi/z2m/src -i $(source)

$(pwre_sroot)/wbl/z2m/src/zigbee2mqtt8.wb_load : $(pwre_sroot)/exp/z2m/src/z2m_devices.dat
	@ $(log_wbl)
	@ tools_z2m_gen -n Zigbee2mqtt8 -v 0.0.250.17 -f t\*,u\* -d $(pwre_sroot)/wbl/z2m8/src -c $(pwre_sroot)/lib/z2m/src -g $(pwre_sroot)/mmi/z2m/src -i $(source)

$(pwre_sroot)/wbl/z2m/src/zigbee2mqtt9.wb_load : $(pwre_sroot)/exp/z2m/src/z2m_devices.dat
	@ $(log_wbl)
	@ tools_z2m_gen -n Zigbee2mqtt9 -v 0.0.250.18 -f v\*,w\*,x\*,y\*,z\* -d $(pwre_sroot)/wbl/z2m9/src -c $(pwre_sroot)/lib/z2m/src -g $(pwre_sroot)/mmi/z2m/src -i $(source)

$(pwre_sroot)/exp/z2m/src/pwr_z2m_palette.cnf : $(pwre_sroot)/wbl/z2m1/src/zigbee2mqtt1.txt
	@ $(log_pal)
	@ tools_z2m_palgen

$(pwr_exe)/pwr_z2m_palette.cnf : $(pwre_sroot)/exp/z2m/src/pwr_z2m_palette.cnf
	@ $(log_cp)
	@ $(cp) $(source) $(target)

$(pwre_sroot)/mmi/z2m/src/pwr_c_abl_rb56ac.pwg : $(pwre_sroot)/mmi/z2m/src/pwr_c_abl_rb56ac.ge_com
	@ $(log_ge_com)
	@ export pwrp_pop=$(pwre_sroot)/mmi/z2m/src; export pwr_load=$(pwr_eload); export pwr_exe=$(pwr_eexe); export pwrp_tmp=/tmp; wb_ge @$(pwre_sroot)/mmi/z2m/src/process

clean_z2m:
	@ echo "Cleaning z2m"
	@ if [ -e $(pwre_sroot)/wbl/z2m1/src/zigbee2mqtt1.wb_load ]; then rm $(pwre_sroot)/wbl/z2m1/src/zigbee2mqtt1.wb_load; fi
	@ if [ -e $(pwre_sroot)/wbl/z2m2/src/zigbee2mqtt2.wb_load ]; then rm $(pwre_sroot)/wbl/z2m2/src/zigbee2mqtt2.wb_load; fi
	@ if [ -e $(pwre_sroot)/wbl/z2m3/src/zigbee2mqtt3.wb_load ]; then rm $(pwre_sroot)/wbl/z2m3/src/zigbee2mqtt3.wb_load; fi
	@ if [ -e $(pwre_sroot)/wbl/z2m4/src/zigbee2mqtt4.wb_load ]; then rm $(pwre_sroot)/wbl/z2m4/src/zigbee2mqtt4.wb_load; fi
	@ if [ -e $(pwre_sroot)/wbl/z2m5/src/zigbee2mqtt5.wb_load ]; then rm $(pwre_sroot)/wbl/z2m5/src/zigbee2mqtt5.wb_load; fi
	@ if [ -e $(pwre_sroot)/wbl/z2m6/src/zigbee2mqtt6.wb_load ]; then rm $(pwre_sroot)/wbl/z2m6/src/zigbee2mqtt6.wb_load; fi
	@ if [ -e $(pwre_sroot)/wbl/z2m7/src/zigbee2mqtt7.wb_load ]; then rm $(pwre_sroot)/wbl/z2m7/src/zigbee2mqtt7.wb_load; fi
	@ if [ -e $(pwre_sroot)/wbl/z2m8/src/zigbee2mqtt8.wb_load ]; then rm $(pwre_sroot)/wbl/z2m8/src/zigbee2mqtt8.wb_load; fi
	@ if [ -e $(pwre_sroot)/wbl/z2m9/src/zigbee2mqtt9.wb_load ]; then rm $(pwre_sroot)/wbl/z2m9/src/zigbee2mqtt9.wb_load; fi
	@ if [ -e $(pwre_sroot)/lib/z2m/src/rt_plc_zigbee2mqtt1.c ]; then rm $(pwre_sroot)/lib/z2m/src/rt_plc_zigbee2mqtt1.c; fi
	@ if [ -e $(pwre_sroot)/lib/z2m/src/rt_plc_zigbee2mqtt1.h ]; then rm $(pwre_sroot)/lib/z2m/src/rt_plc_zigbee2mqtt1.h; fi
	@ if [ -e $(pwre_sroot)/lib/z2m/src/rt_plc_zigbee2mqtt2.c ]; then rm $(pwre_sroot)/lib/z2m/src/rt_plc_zigbee2mqtt2.c; fi
	@ if [ -e $(pwre_sroot)/lib/z2m/src/rt_plc_zigbee2mqtt2.h ]; then rm $(pwre_sroot)/lib/z2m/src/rt_plc_zigbee2mqtt2.h; fi
	@ if [ -e $(pwre_sroot)/lib/z2m/src/rt_plc_zigbee2mqtt3.c ]; then rm $(pwre_sroot)/lib/z2m/src/rt_plc_zigbee2mqtt3.c; fi
	@ if [ -e $(pwre_sroot)/lib/z2m/src/rt_plc_zigbee2mqtt3.h ]; then rm $(pwre_sroot)/lib/z2m/src/rt_plc_zigbee2mqtt3.h; fi
	@ if [ -e $(pwre_sroot)/lib/z2m/src/rt_plc_zigbee2mqtt4.c ]; then rm $(pwre_sroot)/lib/z2m/src/rt_plc_zigbee2mqtt4.c; fi
	@ if [ -e $(pwre_sroot)/lib/z2m/src/rt_plc_zigbee2mqtt4.h ]; then rm $(pwre_sroot)/lib/z2m/src/rt_plc_zigbee2mqtt4.h; fi
	@ if [ -e $(pwre_sroot)/lib/z2m/src/rt_plc_zigbee2mqtt5.c ]; then rm $(pwre_sroot)/lib/z2m/src/rt_plc_zigbee2mqtt5.c; fi
	@ if [ -e $(pwre_sroot)/lib/z2m/src/rt_plc_zigbee2mqtt5.h ]; then rm $(pwre_sroot)/lib/z2m/src/rt_plc_zigbee2mqtt5.h; fi
	@ if [ -e $(pwre_sroot)/lib/z2m/src/rt_plc_zigbee2mqtt6.c ]; then rm $(pwre_sroot)/lib/z2m/src/rt_plc_zigbee2mqtt6.c; fi
	@ if [ -e $(pwre_sroot)/lib/z2m/src/rt_plc_zigbee2mqtt6.h ]; then rm $(pwre_sroot)/lib/z2m/src/rt_plc_zigbee2mqtt6.h; fi
	@ if [ -e $(pwre_sroot)/lib/z2m/src/rt_plc_zigbee2mqtt7.c ]; then rm $(pwre_sroot)/lib/z2m/src/rt_plc_zigbee2mqtt7.c; fi
	@ if [ -e $(pwre_sroot)/lib/z2m/src/rt_plc_zigbee2mqtt7.h ]; then rm $(pwre_sroot)/lib/z2m/src/rt_plc_zigbee2mqtt7.h; fi
	@ if [ -e $(pwre_sroot)/lib/z2m/src/rt_plc_zigbee2mqtt8.c ]; then rm $(pwre_sroot)/lib/z2m/src/rt_plc_zigbee2mqtt8.c; fi
	@ if [ -e $(pwre_sroot)/lib/z2m/src/rt_plc_zigbee2mqtt8.h ]; then rm $(pwre_sroot)/lib/z2m/src/rt_plc_zigbee2mqtt8.h; fi
	@ if [ -e $(pwre_sroot)/lib/z2m/src/rt_plc_zigbee2mqtt9.c ]; then rm $(pwre_sroot)/lib/z2m/src/rt_plc_zigbee2mqtt9.c; fi
	@ if [ -e $(pwre_sroot)/lib/z2m/src/rt_plc_zigbee2mqtt9.h ]; then rm $(pwre_sroot)/lib/z2m/src/rt_plc_zigbee2mqtt9.h; fi
	@ for file in $(pwre_sroot)/mmi/z2m/src/pwr_c_*.ge_com; do \
	    if [ -e $$file ]; then rm $$file; fi \
	  done
	@ for file in $(pwre_sroot)/mmi/z2m/src/pwr_c_*.pwg; do \
	    if [ -e $$file ]; then rm $$file; fi \
	  done
	@ if [ -e $(pwr_load)/zigbee2mqtt1.dbs ]; then rm $(pwr_load)/zigbee2mqtt1.dbs; fi
	@ if [ -e $(pwr_load)/zigbee2mqtt2.dbs ]; then rm $(pwr_load)/zigbee2mqtt2.dbs; fi
	@ if [ -e $(pwr_load)/zigbee2mqtt3.dbs ]; then rm $(pwr_load)/zigbee2mqtt3.dbs; fi
	@ if [ -e $(pwr_load)/zigbee2mqtt4.dbs ]; then rm $(pwr_load)/zigbee2mqtt4.dbs; fi
	@ if [ -e $(pwr_load)/zigbee2mqtt5.dbs ]; then rm $(pwr_load)/zigbee2mqtt5.dbs; fi
	@ if [ -e $(pwr_load)/zigbee2mqtt6.dbs ]; then rm $(pwr_load)/zigbee2mqtt6.dbs; fi
	@ if [ -e $(pwr_load)/zigbee2mqtt7.dbs ]; then rm $(pwr_load)/zigbee2mqtt7.dbs; fi
	@ if [ -e $(pwr_load)/zigbee2mqtt8.dbs ]; then rm $(pwr_load)/zigbee2mqtt8.dbs; fi
	@ if [ -e $(pwr_load)/zigbee2mqtt9.dbs ]; then rm $(pwr_load)/zigbee2mqtt9.dbs; fi
	@ if [ -e $(pwr_eload)/zigbee2mqtt1.dbs ]; then rm $(pwr_eload)/zigbee2mqtt1.dbs; fi
	@ if [ -e $(pwr_eload)/zigbee2mqtt2.dbs ]; then rm $(pwr_eload)/zigbee2mqtt2.dbs; fi
	@ if [ -e $(pwr_eload)/zigbee2mqtt3.dbs ]; then rm $(pwr_eload)/zigbee2mqtt3.dbs; fi
	@ if [ -e $(pwr_eload)/zigbee2mqtt4.dbs ]; then rm $(pwr_eload)/zigbee2mqtt4.dbs; fi
	@ if [ -e $(pwr_eload)/zigbee2mqtt5.dbs ]; then rm $(pwr_eload)/zigbee2mqtt5.dbs; fi
	@ if [ -e $(pwr_eload)/zigbee2mqtt6.dbs ]; then rm $(pwr_eload)/zigbee2mqtt6.dbs; fi
	@ if [ -e $(pwr_eload)/zigbee2mqtt7.dbs ]; then rm $(pwr_eload)/zigbee2mqtt7.dbs; fi
	@ if [ -e $(pwr_eload)/zigbee2mqtt8.dbs ]; then rm $(pwr_eload)/zigbee2mqtt8.dbs; fi
	@ if [ -e $(pwr_eload)/zigbee2mqtt9.dbs ]; then rm $(pwr_eload)/zigbee2mqtt9.dbs; fi
	@ if [ -e $(pwre_sroot)/exp/z2m/src/pwr_z2m_palette.cnf ]; then rm $(pwre_sroot)/exp/z2m/src/pwr_z2m_palette.cnf; fi

