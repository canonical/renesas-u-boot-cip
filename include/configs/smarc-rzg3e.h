/* SPDX-License-Identifier: GPL-2.0+ */
/*
 * Copyright (C) 2022 Renesas Electronics Corporation
 */

#ifndef __SMARC_RZG3E_H
#define __SMARC_RZG3E_H

#include <asm/arch/renesas.h>

#define CONFIG_REMAKE_ELF

#ifdef CONFIG_SPL
#define CONFIG_SPL_TARGET	"spl/u-boot-spl.scif"
#endif

/* Boot options */
#define CONFIG_CMDLINE_TAG
#define CONFIG_SETUP_MEMORY_TAGS
#define CONFIG_INITRD_TAG

/* Generic Interrupt Controller Definitions */
/* RZ/G3E use GIC-v3 */
#define CONFIG_GICV3
#define GICD_BASE	0x14900000
#define GICR_BASE	0x14940000

/* console */
#define CONFIG_SYS_CBSIZE		2048
#define CONFIG_SYS_BARGSIZE		CONFIG_SYS_CBSIZE
#define CONFIG_SYS_BAUDRATE_TABLE	{ 115200, 38400 }

/* PHY needs a longer autoneg timeout */
#define PHY_ANEG_TIMEOUT		20000

/* Memory */
#define CONFIG_SYS_INIT_SP_ADDR		CONFIG_SYS_TEXT_BASE
#define CONFIG_SH_SDHI_FREQ		133000000
#define DRAM_RSV_SIZE			0x08000000
#define CONFIG_SYS_SDRAM_BASE		(0x40000000 + DRAM_RSV_SIZE)
#define CONFIG_SYS_SDRAM_SIZE		(0x100000000u - DRAM_RSV_SIZE) //total 4GB
#define CONFIG_LOADADDR			CONFIG_SYS_LOAD_ADDR  // Default load address for tftp,bootp...
#define CONFIG_VERY_BIG_RAM
#define CONFIG_MAX_MEM_MAPPED		(0x80000000u - DRAM_RSV_SIZE)
#define CONFIG_SYS_MONITOR_BASE		0x00000000
#define CONFIG_SYS_MONITOR_LEN		(1 * 1024 * 1024)
#define CONFIG_SYS_MALLOC_LEN		(64 * 1024 * 1024)
#define CONFIG_SYS_BOOTM_LEN		(256 * 1024 * 1024)

/* The HF/QSPI layout permits up to 2 MiB large bootloader blob */
#define CONFIG_BOARD_SIZE_LIMIT		2097152

/* FIT Environment (from FIT header) */
#define UBUNTU_ENV_LOAD_BOOT_CONFIG \
    "load_uc=" \
      "setenv kernel_bootpart ${mmc_seed_part};" \
      "load ${devtype} ${mmcdev}:${kernel_bootpart} ${fitloadaddr} ${core_state};" \
      "env import ${fitloadaddr} ${filesize} ${recovery_vars};" \
      "if test \"${snapd_recovery_mode}\" = \"run\"; then " \
        "setenv bootargs \"snapd_recovery_mode=${snapd_recovery_mode} ${snapd_standard_params} rw rootwait earlycon modprobe.blacklist=adv7511\";" \
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
          "env export -c ${fitloadaddr} ${kernel_vars};" \
          "save ${devtype} ${mmcdev}:${kernel_bootpart} ${fitloadaddr} ${core_state} ${filesize};" \
        "fi;" \
        "setenv kernel_prefix \"/uboot/ubuntu/${kernel_name}/\";" \
      "else " \
        "setenv bootargs \"snapd_recovery_mode=${snapd_recovery_mode} snapd_recovery_system=${snapd_recovery_system} ${snapd_standard_params} rw rootwait earlycon modprobe.blacklist=adv7511\";" \
        "setenv kernel_prefix \"/systems/${snapd_recovery_system}/kernel/\";" \
      "fi;" \
      "setenv platform_part 1;"                                           \
      "load ${devtype} ${mmcdev}:${platform_part} ${fdt_addr_r} ${fdtfile};" \
      "run loadfiles\0"

#define UBUNTU_ENV_LOAD_FIT_BOOT_FILES \
    "loadfiles=load ${devtype} ${mmcdev}:${kernel_bootpart} ${fitloadaddr} ${kernel_prefix}/${kernel_filename}\0"

#define UBUNTU_ENV_DEFAULT \
    "kernel_filename=kernel.img\0" \
    "bootargs=modprobe.blacklist=adv7511\0" \
    "core_state=/uboot/ubuntu/boot.sel\0" \
    "kernel_vars=snap_kernel snap_try_kernel kernel_status\0" \
    "recovery_vars=snapd_recovery_mode snapd_recovery_system snapd_recovery_kernel\0" \
    "snapd_recovery_mode=install\0" \
    "snapd_standard_params=\0" \
    UBUNTU_ENV_LOAD_BOOT_CONFIG

/* EFI Environment (from EFI header) */


/* Traditional Linux Boot */
#define BOOT_LINUX_ENV \
    "boot_linux=" \
        "load ${devtype} ${devnum}:${distro_bootpart} ${kernel_addr_r} /boot/Image; " \
        "load ${devtype} ${devnum}:${distro_bootpart} ${fdt_addr_r} /boot/${fdtfile}; " \
        "setenv bootargs console=${console} root=/dev/mmcblk1p3 rootwait rw; " \
        "booti ${kernel_addr_r} - ${fdt_addr_r};\0"
/* Detection Logic */
#define BOOT_DETECT_ENV \
    "detect_boot=" \
        "setenv devtype mmc; setenv devnum 1; setenv distro_bootpart 2; " \
        "echo Checking ${devtype} ${devnum}:${distro_bootpart} for FIT structure; " \
        "if load ${devtype} ${devnum}:${distro_bootpart} ${fitloadaddr} uboot/ubuntu/boot.sel; then " \
            "echo FIT structure detected (uboot/ubuntu/boot.sel found); " \
            "run boot_uc; sleep 3; reset; " \
        "else " \
            "echo No FIT structure found, trying traditional Linux boot; " \
            "run boot_linux; " \
            "echo Traditional boot failed, trying EFI; " \
            "run boot_efi; " \
        "fi;\0"



/* Simplified EFI boot for Ubuntu Classic - directly boot from MMC 1:2 */
#define SIMPLE_EFI_BOOT \
	"boot_efi_binary=EFI/ubuntu/grubaa64.efi\0" \
	"boot_efi=" \
		"load mmc 1:1 ${fdt_addr_r} ${fdtfile}; " \
		"load mmc 1:2 ${kernel_addr_r} ${boot_efi_binary}; " \
		"bootefi ${kernel_addr_r} ${fdt_addr_r};\0"

/* Combined Environment */
#define CFG_EXTRA_ENV_SETTINGS \
    "console=ttySC0\0" \
    "usb_pgood_delay=2000\0" \
    "fdt_addr_r=0x48000000\0" \
    "fdtfile=" CONFIG_DEFAULT_FDT_FILE "\0" \
    "kernel_addr_r=0x48080000\0" \
    "fitloadaddr=0x61000000\0"   \
    UBUNTU_ENV_DEFAULT SIMPLE_EFI_BOOT \
    UBUNTU_ENV_LOAD_FIT_BOOT_FILES \
    BOOT_LINUX_ENV \
    BOOT_DETECT_ENV \
    "devtype=mmc\0" \
    "mmcdev=1\0" \
    "mmc_seed_part=2\0" \
    "mmc_boot_part=3\0" \
    "dfu_alt_info=sf 0:0=fip-smarc-rzg3e.bin raw 0x20000 0x1F0000\0" \
    "dfu_bufsiz=0x1F0000\0" \
    "ipaddr=192.168.10.7\0" \
    "serverip=192.168.10.1\0" \
    "boot_uc=run load_uc;bootm 0x61000000#r9a09g047e57-smarc.dtb 0x61000000#r9a09g047e57-smarc.dtb ${fdt_addr_r}\0" \
    "bootmode=auto\0" /* Default to auto-detection */ \
    "bootcmd=run detect_boot;\0"

/* Ethernet RAVB */
#define CONFIG_BITBANGMII_MULTI

#endif /* __SMARC_RZG3E_H */
