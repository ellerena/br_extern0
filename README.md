- Xilinx uses gLibc (buildroot default is ucLibc) so change BR to gLibc

br_extern0/
├── board
│   ├── arty
│   │   ├── artylnx_cnf_defconfig
│   │   ├── artylnx_cnf_defconfig_nosvc
│   │   ├── arty_lnx_pmod.dts
│   │   ├── busybox.config
│   │   └── busybox_ovly
│   │       ├── etc
│   │       │   └── init.d
│   │       │       └── S99httpd
│   │       └── var
│   │           └── www
│   │               ├── cgi-bin
│   │               │   └── <multiple tools>
│   │               ├── dev_cora.elf
│   │               ├── dev.elf
│   │               ├── index.html
│   │               ├── sys_cora.elf
│   │               └── sys.elf
│   ├── bbgw
│   │   └── uEnv.txt
│   ├── common
│   │   └── busybox_ovly
│   │       ├── etc
│   │       │   └── init.d
│   │       └── var
│   │           └── www
│   │               ├── cgi-bin        // multiple scripts used for play/test
│   │               │   ├── eddie.py
│   │               │   ├── index.cgi
│   │               │   ├── info.lua
│   │               │   ├── info.php
│   │               │   └── testperl
│   │               └── kotester_nosvc // <<GENERATE>> basic generic tool to test kernel modules
│   ├── coraz7
│   │   ├── busybox_ovly
│   │   │   ├── etc
│   │   │   │   └── init.d
│   │   │   │       └── S99httpd
│   │   │   └── var
│   │   │       └── www
│   │   │           ├── cgi-bin
│   │   │           └── index.html  // default landing page, show board name
│   │   ├── CreateBootImage         // out-of-tree tools to generate SD uboot
│   │   │   ├── cora_br_fsbl.bif
│   │   │   ├── CreateBootImage.sh  // generate BOOT.bin containing FSBL and u-boot only
│   │   │   └── uEnv.txt            // custom configuration for u-boot
│   │   ├── uboot_zynq_coraz7_defconfig  // u-boot main configuration file
│   │   └── zynq-coraz7.dts              // custom dts generated with SDK + preprocessing
│   └── sama5d3xek
│       ├── busybox_ovly
│       │   ├── etc
│       │   │   ├── init.d
│       │   │   │   └── S99httpd
│       │   │   └── network
│       │   │       └── interrfaces
│       │   └── var
│       │       └── www
│       │           ├── cgi-bin
│       │           │   └── <multiple tools>
│       │           └── index.html
│       └── sama5d31_busybox.config
├── Config.in
├── configs
│   ├── artylnx_busybox_defconfig
│   ├── _atmel_sama5d3xek_defconfig
│   └── coraz7lnx_defconfig
├── external.desc
├── external.mk
├── package
│   ├── arty_pmod
│   │   ├── arty_pmod.mk
│   │   ├── Config.in
│   │   └── src
│   │       ├── Makefile
│   │       ├── pmoddev.c
│   │       └── pmodsys.c
│   └── cora_br
│       ├── Config.in
│       ├── cora_br.mk
│       └── src
│           ├── infodev.c
│           ├── infosys.c
│           └── Makefile

