/* SPDX-License-Identifier: GPL-2.0+ */
/*
 * Copyright (C) 2022 Renesas Electronics Corporation
 */

#ifndef __RZT2H_DEV_H
#define __RZT2H_DEV_H

#include <asm/arch/renesas.h>

#define CONFIG_REMAKE_ELF

#ifdef CONFIG_SPL
#define CONFIG_SPL_TARGET	"spl/u-boot-spl.scif"
#endif

/* boot option */

#define CONFIG_CMDLINE_TAG
#define CONFIG_SETUP_MEMORY_TAGS
#define CONFIG_INITRD_TAG

/* Generic Interrupt Controller Definitions */
/* RZ/T2H use GIC-v3 */
#define CONFIG_GICV3
#define GICD_BASE	0x83000000
#define GICR_BASE	0x83040000

/* console */
#define CONFIG_SYS_CBSIZE		2048
#define CONFIG_SYS_BARGSIZE		CONFIG_SYS_CBSIZE
#define CONFIG_SYS_MAXARGS		64
#define CONFIG_SYS_BAUDRATE_TABLE	{ 115200, 38400 }

/* PHY needs a longer autoneg timeout */
#define PHY_ANEG_TIMEOUT		20000

/* MEMORY */
#define CONFIG_SYS_INIT_SP_ADDR		CONFIG_SYS_TEXT_BASE

/* SDHI clock freq */
#define CONFIG_SH_SDHI_FREQ		133000000

#define DRAM_RSV_SIZE			0x4000000
#define CONFIG_SYS_SDRAM_BASE		(0xC0000000 + DRAM_RSV_SIZE)
#define CONFIG_SYS_SDRAM_SIZE		(0x40000000 - DRAM_RSV_SIZE)

#define CONFIG_LOADADDR			CONFIG_SYS_LOAD_ADDR // Default load address for tfpt,bootp...
#define CONFIG_VERY_BIG_RAM
#define CONFIG_MAX_MEM_MAPPED		(0x40000000 - DRAM_RSV_SIZE)

#define CONFIG_SYS_MONITOR_BASE		0x00000000

#if defined(CONFIG_TARGET_RZT2H_DISTRO_BOOT)

#ifndef UC_EXTRA_BOOTARGS
#define UC_EXTRA_BOOTARGS ""
#endif

/* Ubuntu Core FIT Boot Environment */
#define UBUNTU_ENV_LOAD_BOOT_CONFIG \
	"load_uc=" \
		"setenv kernel_bootpart ${mmc_seed_part};" \
		"load ${devtype} ${mmcdev}:${kernel_bootpart} ${fitloadaddr} ${core_state};" \
		"env import -c ${fitloadaddr} ${filesize} ${recovery_vars};" \
		"if test \"${snapd_recovery_mode}\" = \"run\"; then " \
			"setenv bootargs \"console=${console} snapd_recovery_mode=${snapd_recovery_mode} ${snapd_standard_params} " UC_EXTRA_BOOTARGS "rw rootwait earlycon\";" \
			"setenv kernel_bootpart ${mmc_boot_part};" \
			"load ${devtype} ${mmcdev}:${kernel_bootpart} ${fitloadaddr} ${core_state};" \
			"env import -c ${fitloadaddr} ${filesize} ${kernel_vars};" \
			"setenv kernel_name ${snap_kernel};" \
			"if test -n \"${kernel_status}\"; then " \
				"if test \"${kernel_status}\" = \"try\"; then " \
					"if test -n \"${snap_try_kernel}\"; then " \
						"setenv kernel_status trying;" \
						"setenv kernel_name \"${snap_try_kernel}\";" \
					"fi;" \
				"elif test \"${kernel_status}\" = \"trying\"; then " \
					"setenv kernel_status \"\";" \
				"fi;" \
			"fi;" \
			"env export -c ${fitloadaddr} ${kernel_vars};" \
			"save ${devtype} ${mmcdev}:${kernel_bootpart} ${fitloadaddr} ${core_state} ${filesize};" \
			"setenv kernel_prefix \"/uboot/ubuntu/${kernel_name}/\";" \
		"else " \
			"setenv bootargs \"console=${console} snapd_recovery_mode=${snapd_recovery_mode} snapd_recovery_system=${snapd_recovery_system} ${snapd_standard_params} " UC_EXTRA_BOOTARGS "rw rootwait earlycon\";" \
			"setenv kernel_prefix \"/systems/${snapd_recovery_system}/kernel/\";" \
		"fi;" \
		"setenv platform_part 1;" \
		"setenv fit_config r9a09g077m44-rzt2h-evk.dtb;" \
		"run loadfiles; " \
		"bootm ${fitloadaddr}#${fit_config}\0"

#define UBUNTU_ENV_LOAD_FIT_BOOT_FILES \
	"loadfiles=load ${devtype} ${mmcdev}:${kernel_bootpart} ${fitloadaddr} ${kernel_prefix}/${kernel_filename}\0"

#define UBUNTU_ENV_DEFAULT \
	"kernel_filename=kernel.img\0" \
	"core_state=/uboot/ubuntu/boot.sel\0" \
	"kernel_vars=snap_kernel snap_try_kernel kernel_status\0" \
	"recovery_vars=snapd_recovery_mode snapd_recovery_system snapd_recovery_kernel\0" \
	"snapd_recovery_mode=install\0" \
	"snapd_standard_params=\0" \
	UBUNTU_ENV_LOAD_BOOT_CONFIG


/* EFI Boot Environment */
#define EFI_ENV_DEFAULT \
	"boot_efi_binary=efi/boot/bootaa64.efi\0" \
	"scan_for_usb_dev=" \
		"usb start; " \
		"if test ! -e usb ${devnum}:1 /; then usb reset; fi;\0" \
	"scan_boot_efi=" \
		"part list ${devtype} ${devnum} devplist; " \
		"env exists devplist || setenv devplist 1; " \
		"for distro_bootpart in ${devplist}; do " \
			"if test -e ${devtype} ${devnum}:${distro_bootpart} ${boot_efi_binary}; then " \
				"load ${devtype} ${devnum}:${distro_bootpart} " \
				"${kernel_addr_r} ${boot_efi_binary};" \
				"load ${devtype} ${devnum}:${platform_part} " \
				"${fdt_addr_r} ${fdtfile};" \
				"echo BootEFI from <${devtype}> [${devnum}:${distro_bootpart}] " \
				"dtb from <${devtype}> [${devnum}:${platform_part}] ${fdtfile};" \
				"bootefi ${kernel_addr_r} ${fdt_addr_r};" \
			"fi;" \
		"done;\0" \
	"mmc0_efi=" \
		"setenv devnum 0;" \
		"setenv devtype mmc;" \
		"run scan_boot_efi;\0" \
	"mmc1_efi=" \
		"setenv devnum 1;" \
		"setenv devtype mmc;" \
		"run scan_boot_efi;\0" \
	"usb0_efi=" \
		"setenv devnum 0;" \
		"setenv devtype usb;" \
		"run scan_for_usb_dev;" \
		"run scan_boot_efi;\0" \
	"usb1_efi=" \
		"setenv devnum 1;" \
		"setenv devtype usb;" \
		"run scan_for_usb_dev;" \
		"run scan_boot_efi;\0" \
	"efi_targets=usb0_efi usb1_efi mmc0_efi mmc1_efi\0" \
	"boot_efi=" \
		"for target in ${efi_targets}; do " \
			"run ${target}; " \
		"done;\0"

/* Detection Logic */
#define BOOT_DETECT_ENV \
    "detect_boot=" \
        "setenv devtype mmc; setenv devnum 1; setenv distro_bootpart 2; " \
        "echo Checking ${devtype} ${devnum}:${distro_bootpart} for FIT structure; " \
        "if load ${devtype} ${devnum}:${distro_bootpart} ${fitloadaddr} uboot/ubuntu/boot.sel; then " \
            "echo FIT structure detected (uboot/ubuntu/boot.sel found); " \
            "run boot_uc; " \
        "else " \
            "echo No FIT structure found on mmc ${devnum}:${distro_bootpart}, trying EFI; " \
            "run boot_efi; " \
        "fi;\0"


/* ENV setting */
#define CFG_EXTRA_ENV_SETTINGS	\
	"fdt_addr_r=0xC5F00000\0" \
	"fitloadaddr=0xE8000000\0" \
	"fdtfile="CONFIG_DEFAULT_FDT_FILE"\0" \
	"kernel_addr_r=0xc4200000\0" \
	"usb_pgood_delay=2000\0" \
	"console=ttySC0,115200\0" \
	"devtype=mmc\0" \
	/* 0: eMMC and 1: SD*/ \
	"mmcdev=1\0" \
    "mmc_seed_part=2\0" \
    "mmc_boot_part=3\0" \
    UBUNTU_ENV_DEFAULT \
    UBUNTU_ENV_LOAD_FIT_BOOT_FILES \
    EFI_ENV_DEFAULT \
	BOOT_DETECT_ENV \
	"dfu_alt_info=sf 0:0=fip.bin raw 0x60000 0x1F0000 \0" \
	"dfu_bufsiz=0x1F0000\0" \
	"ipaddr=192.168.10.7\0" \
    "serverip=192.168.10.1\0" \
	"boot_uc=run load_uc;bootm ${fitloadaddr}#${fdtfile}\0" \
    "bootcmd=run detect_boot;\0"

#else
#define CONFIG_BOOTCOMMAND     "env default -a;run bootcmd_check;run bootimage"

/* ENV setting */
#define CFG_EXTRA_ENV_SETTINGS	\
	"usb_pgood_delay=2000\0" \
	"bootm_size=0x10000000\0" \
	"prodsdbootargs=setenv bootargs rw rootwait earlycon root=/dev/mmcblk1p2 \0" \
	"prodemmcbootargs=setenv bootargs rw rootwait earlycon root=/dev/mmcblk0p2 \0" \
	"bootimage=booti 0xc4200000 - 0xC5F00000\0" \
	"emmcload=ext4load mmc 0:2 0xc4200000 boot/Image;ext4load mmc 0:2 0xC5F00000 boot/r9a09g077m44-dev.dtb;run prodemmcbootargs \0" \
	"sd1load=ext4load mmc 1:2 0xc4200000 boot/Image;ext4load mmc 1:2 0xC5F00000 boot/r9a09g077m44-dev.dtb;run prodsdbootargs \0" \
	"bootcmd_check=if mmc dev 1; then run sd1load; else run emmcload; fi \0" \
	"dfu_alt_info=sf 0:0=fip.bin raw 0x60000 0x1F0000 \0" \
	"dfu_bufsiz=0x1F0000\0"

#endif /* CONFIG_TARGET_RZT2H_DISTRO_BOOT */
/* For board */
/* Ethernet RAVB */
#define CONFIG_BITBANGMII_MULTI

#endif /* __RZT2H_DEV_H */
