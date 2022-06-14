# iwl20x_ax_enable
This kernel livepatch enables 802.11ax mode for Intel AX201/AX200 WLAN adapter in Ukraine and/or Russia.

The module should be compiled and loaded as usual livepatch. `iwl20x_ax_en` should be loaded before `iwlwifi` (see `iwlwifi.conf`).

The module `iwl20x_ax_en.ko` should be signed with your MOK key if the system boots in EFI Secure Boot mode. (See RHEL documentation on Signing kernel modules for secure boot)


## Installation  

Generate and enroll (if not yet) an X.509 key pair [as described.](https://docs.fedoraproject.org/en-US/fedora/latest/system-administrators-guide/kernel-module-driver-configuration/Working_with_Kernel_Modules/) (If the system boots in EFI Secure Boot mode.)

`# make`  
`# /usr/src/kernels/$(uname -r)/scripts/sign-file sha256 <your_mok_key>.priv <your_mok_key>.der iwl20x_ax_en.ko` (skip this for Secure Boot disabled system)  
`# cp ./iwl20x_ax_en.ko /lib/modules/$(uname -r)/`  
`# cat ./iwlwifi.conf >> /etc/modprobe.d/iwlwifi.conf`  
`# depmod -a`  
`# reboot`
