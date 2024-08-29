#include "stdafx.h"
#include "master_service.h"

int main(int argc, char *argv[])
{
	printf("ACL_VSTRING=%zd, acl::string=%zd, std::string=%zd\r\n",
		sizeof(ACL_VSTRING), sizeof(acl::string), sizeof(std::string));
	acl::acl_cpp_init();
	master_service& ms = acl::singleton2<master_service>::get_instance();

	ms.set_cfg_int(var_conf_int_tab);
	ms.set_cfg_int64(var_conf_int64_tab);
	ms.set_cfg_str(var_conf_str_tab);
	ms.set_cfg_bool(var_conf_bool_tab);

	if (argc >= 2 && strcasecmp(argv[1], "alone") == 0) {
		const char* addr = "80; 443";

		acl::log::stdout_open(true);
		acl::fiber::stdout_open(true);

		if (argc >= 4) {
			addr = argv[3];
		}
		printf("listen: %s\r\n", addr);

		ms.run_alone(addr, argc >= 3 ? argv[2] : NULL);
	} else {
		ms.run_daemon(argc, argv);
	}

	return 0;
}
