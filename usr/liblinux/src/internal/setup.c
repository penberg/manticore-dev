#include <manticore/config_abi.h>
#include <manticore/syscalls.h>
#include <linux/if_ether.h>
#include <assert.h>

void __liblinux_setup(void)
{
	int eth_desc = acquire("/dev/eth", 0);
	if (eth_desc < 0) {
		assert(0);
	}
	char mac_addr[ETH_ALEN];
	get_config(eth_desc, CONFIG_ETHERNET_MAC_ADDRESS, mac_addr, ETH_ALEN);
	fprintf(stderr, "MAC address = %02x:%02x:%02x:%02x:%02x:%02x\n", mac_addr[0], mac_addr[1], mac_addr[2], mac_addr[3], mac_addr[4], mac_addr[5]);
}
