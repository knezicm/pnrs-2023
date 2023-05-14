
echo "Run embedded_command_shell.sh first"

export QUARTUS_ROOTDIR=/home/nikola/intelFPGA_lite/20.1/quartus/
export PATH=/home/nikola/intelFPGA_lite/20.1/quartus/sopc_builder/bin/:$PATH

echo "CONVERTING .sof TO .rbf"
quartus_cpf -c output_files/de1_soc_top.sof needed_files/socfpga.rbf

echo "CONVERTING SOPC to DTB"
sopc2dts --input soc_system.sopcinfo --output needed_files/socfpga.dtb --type dtb --board soc_system_board_info.xml --board hps_common_board_info.xml --bridge-removal all --clocks

echo "CREATING HEADER FILES"
sopc-create-header-files soc_system.sopcinfo --single needed_files/hps_soc_system.h --module hps_0