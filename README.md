# iwl20x_ax_enable
This kernel livepatch enables 802.11ax mode for Intel AX201/AX200 WLAN adapter in Ukraine and/or Russia.

The module should be compiled and loaded as usual livepatch. `iwl20x_ax_en` should be loaded before `iwlwifi` (see `iwlwifi.conf`).

The module `iwl20x_ax_en.ko` should be signed with your MOK key if the system boots in EFI Secure Boot mode. (See RHEL documentation on Signing kernel modules for secure boot)


## Installation
\# make  
\# cp ./iwl20x_ax_en.ko /lib/modules/$(uname -r)/  
\# cat ./iwlwifi.conf >> /etc/modprobe.d/iwlfifi.conf  
\# depmod -a  
\# reboot
