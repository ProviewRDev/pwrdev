my : demo_all

include $(pwr_exe)/pwrp_rules.mk

demo_modules = $(pwrp_exe)/ra_status



demo_all : $(demo_modules)

$(pwrp_exe)/ra_status : $(pwrp_appl)/ra_status.cpp
	@ echo "Building ra_status"
	@ g++ -g -o $(target) $(source) -I$(pwr_inc) -I/usr/include/tirpc -L$(pwr_lib) -lpwr_rt -lpwr_co -lpwr_msg_dummy -ltirpc -lpthread -lm -lrt
