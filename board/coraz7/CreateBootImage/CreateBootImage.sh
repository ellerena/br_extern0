#!/bin/bash
source /opt/Xilinx/SDK/2019.1/settings64.sh
cp ~/git/buildroot/output/images/u-boot ~/git/buildroot/output/images/u-boot.elf
cd $(dirname "$BASH_SOURCE")
bootgen -image cora_br_fsbl.bif -arch zynq -o BOOT.bin -w
rm ~/git/buildroot/output/images/u-boot.elf
cd -

