/* SPDX-License-Identifier: GPL-2.0+ */
/*
 * Copyright (C) 2022 Renesas Electronics Corporation
 */

#ifndef __RZV2H_DEV_H
#define __RZV2H_DEV_H

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
/* RZ/V2H use GIC-v3 */
#define CONFIG_GICV3
#define GICD_BASE	0x14900000
#define GICR_BASE	0x14940000

/* console */
#define CONFIG_SYS_CBSIZE		2048
#define CONFIG_SYS_BARGSIZE		CONFIG_SYS_CBSIZE
#define CONFIG_SYS_BAUDRATE_TABLE	{ 115200, 38400 }

/* PHY needs a longer autoneg timeout */
#define PHY_ANEG_TIMEOUT		20000

/* MEMORY */
#define CONFIG_SYS_INIT_SP_ADDR		CONFIG_SYS_TEXT_BASE
#define CONFIG_SYS_BOOTM_LEN		  (256 * 1024 * 1024)

#define	DRAM_RSV_SIZE			0x08000000
#define	CFG_MAX_MEM_MAPPED		(0x80000000u - DRAM_RSV_SIZE)

/* SDHI clock freq */
#define CONFIG_SH_SDHI_FREQ		133000000

/* The HF/QSPI layout permits up to 1 MiB large bootloader blob */
#define CONFIG_BOARD_SIZE_LIMIT		1048576

/* ENV setting */
#if defined(CONFIG_RZV2H_DISTRO_BOOT)

/* Ubuntu Core extra boot args (can be overridden by snapcraft build) */
#ifndef UC_EXTRA_BOOTARGS
#define UC_EXTRA_BOOTARGS ""
#endif

/* Renesas accelerator firmware */
#define RENESAS_ACCEL_ENV \
	"ocaaddr=0xA8000000\0" \
	"ocabin=OpenCV_Bin.bin\0" \
	"codaddr=0xAFD00000\0" \
	"codbin=Codec_Bin.bin\0" \
	"load_accel=" \
		"if load ${devtype} ${devnum}:${platform_part} ${ocaaddr} ${ocabin}; then " \
			"echo Loaded OpenCV accelerator binary to ${ocaaddr}; " \
		"else " \
			"echo WARNING: OpenCV accelerator binary not found on ${devtype} ${devnum}:${platform_part}, skipping; " \
		"fi;" \
		"if load ${devtype} ${devnum}:${platform_part} ${codaddr} ${codbin}; then " \
			"echo Loaded codec binary to ${codaddr}; " \
		"else " \
			"echo WARNING: Codec binary not found on ${devtype} ${devnum}:${platform_part}, skipping; " \
		"fi;\0"

/* Ubuntu Core FIT Boot Environment */
#define UBUNTU_ENV_LOAD_BOOT_CONFIG \
	"load_uc=" \
		"setenv kernel_bootpart ${mmc_seed_part};" \
		"load ${devtype} ${mmcdev}:${kernel_bootpart} ${fitloadaddr} ${core_state};" \
		"env import ${fitloadaddr} ${filesize} ${recovery_vars};" \
		"if test \"${snapd_recovery_mode}\" = \"run\"; then " \
			"setenv bootargs \"console=${console} snapd_recovery_mode=${snapd_recovery_mode} ${snapd_standard_params} " UC_EXTRA_BOOTARGS "rw rootwait earlycon\";" \
			"setenv kernel_bootpart ${mmc_boot_part};" \
			"load ${devtype} ${mmcdev}:${kernel_bootpart} ${fitloadaddr} ${core_state};" \
			"env import ${fitloadaddr} ${filesize} ${kernel_vars};" \
			"if test -n \"${snap_kernel}\"; then " \
				"env import -c ${fitloadaddr} ${filesize} ${kernel_vars};" \
			"fi;" \
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
		"setenv fit_config r9a09g057h44-rzv2h-evk.dtb;" \
		"run load_accel;" \
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
				"run load_accel;" \
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

/* Auto-detect Boot Mode (FIT or EFI) */
#define BOOT_DETECT_ENV \
	"detect_boot=" \
		"setenv devtype mmc; setenv devnum 1; setenv distro_bootpart 2; " \
		"if test -e ${devtype} ${devnum}:${distro_bootpart} ${boot_efi_binary}; then " \
			"echo Detected EFI boot from ${devtype} ${devnum}:${distro_bootpart}; " \
			"run boot_efi; " \
		"else " \
			"echo Detected Ubuntu Core FIT boot; " \
			"setenv mmcdev ${devnum}; " \
			"run load_uc; " \
			"if test -n \"${fitimage_addr}\"; then " \
				"echo Booting FIT image at ${fitimage_addr}; " \
				"bootm ${fitloadaddr}; " \
			"fi; " \
		"fi;\0"

#define CFG_EXTRA_ENV_SETTINGS	\
	"usb_pgood_delay=2000\0" \
	"fitloadaddr=0x61000000\0" \
	"fdt_addr_r=0x48000000\0" \
	"fdtfile="CONFIG_DEFAULT_FDT_FILE"\0" \
	"kernel_addr_r=0x48080000\0" \
	"console=ttySC0,115200\0" \
	"mmc_seed_part=2\0" \
	"mmc_boot_part=3\0" \
	"platform_part=1\0" \
	RENESAS_ACCEL_ENV \
	UBUNTU_ENV_DEFAULT \
	UBUNTU_ENV_LOAD_FIT_BOOT_FILES \
	EFI_ENV_DEFAULT \
	BOOT_DETECT_ENV \
	"dfu_alt_info=" \
		"sf 0:0=fip.bin raw 0x60000 0x1F0000\0" \
	"dfu_bufsiz=" \
		"0x1F0000\0" \
	"ipaddr=" \
		"192.168.10.7\0" \
	"serverip=" \
		"192.168.10.3\0" \
	"bootcmd=run detect_boot\0"

#elif defined(CONFIG_TARGET_RZV2H_DEV)
#define CFG_EXTRA_ENV_SETTINGS	\
	"usb_pgood_delay=2000\0"	\
	"bootm_size=0x10000000\0"	\
	"prodsdbootargs=setenv bootargs rw rootwait earlycon root=/dev/mmcblk2p2 \0" \
	"prodemmcbootargs=setenv bootargs rw rootwait earlycon root=/dev/mmcblk0p2 \0" \
	"bootimage=booti 0x48080000 - 0x48000000 \0" \
	"emmcload=ext4load mmc 0:2 0x48080000 boot/Image;ext4load mmc 0:2 0x48000000 boot/r9a09g057h4-dev.dtb;run prodemmcbootargs \0" \
	"sd2load=ext4load mmc 2:2 0x48080000 boot/Image;ext4load mmc 2:2 0x48000000 boot/r9a09g057h4-dev.dtb;run prodsdbootargs \0" \
	"bootcmd_check=if mmc dev 2; then run sd2load; else run emmcload; fi \0"
#elif defined(CONFIG_TARGET_RZV2H_EVK_ALPHA)
#define CFG_EXTRA_ENV_SETTINGS	\
	"usb_pgood_delay=2000\0"	\
	"bootm_size=0x10000000\0"	\
	"prodsdbootargs=setenv bootargs rw rootwait earlycon root=/dev/mmcblk1p2 \0" \
	"prodemmcbootargs=setenv bootargs rw rootwait earlycon root=/dev/mmcblk0p2 \0" \
	"set_pmic=i2c dev 8; i2c mw 0x6a 0x22 0x0f; i2c mw 0x6a 0x24 0x00; i2c md 0x6a 0x00 0x30; i2c mw 0x12 0x8D 0x02; i2c md 0x12 0x20 0x80 \0" \
	"bootimage=run set_pmic; booti 0x48080000 - 0x48000000 \0" \
	"emmcload=ext4load mmc 0:2 0x48080000 boot/Image;ext4load mmc 0:2 0x48000000 boot/r9a09g057h4-evk-alpha.dtb;run prodemmcbootargs \0" \
	"sd1load=ext4load mmc 1:2 0x48080000 boot/Image;ext4load mmc 1:2 0x48000000 boot/r9a09g057h4-evk-alpha.dtb;run prodsdbootargs \0" \
	"bootcmd_check=if mmc dev 1; then run sd1load; else run emmcload; fi \0"
#else
#define CFG_EXTRA_ENV_SETTINGS       \
	"usb_pgood_delay=2000\0"        \
	"bootm_size=0x10000000\0"       \
	"prodsd0bootargs=setenv bootargs rw rootwait earlycon root=/dev/mmcblk0p2 \0" \
	"prodsd1bootargs=setenv bootargs rw rootwait earlycon root=/dev/mmcblk1p2 \0" \
	"bootimage=booti 0x48080000 - 0x48000000 \0" \
	"sd0load=ext4load mmc 0:2 0x48080000 boot/Image;ext4load mmc 0:2 0x48000000 boot/r9a09g057h44-rzv2h-evk.dtb;run prodsd0bootargs \0" \
	"sd1load=ext4load mmc 1:2 0x48080000 boot/Image;ext4load mmc 1:2 0x48000000 boot/r9a09g057h44-rzv2h-evk.dtb;run prodsd1bootargs \0" \
	"bootcmd_check=if mmc dev 1; then run sd1load; else run sd0load; fi \0"
#endif

#if defined(CONFIG_RZV2H_DISTRO_BOOT)
#define CONFIG_BOOTCOMMAND	"run detect_boot"
#else
#define CONFIG_BOOTCOMMAND	"env default -a;run bootcmd_check;run bootimage"
#endif

/* For board */
/* Ethernet RAVB */
#define CONFIG_BITBANGMII_MULTI

#endif /* __RZV2H_DEV_H */
