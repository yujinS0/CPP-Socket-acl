#include "stdafx.h"
#include "connect_client.h"
#include "connect_pool.h"

connect_pool::connect_pool(const char* addr, size_t count, size_t idx)
: acl::connect_pool(addr, count, idx)
, addr_(addr)
, conn_timeout_(30)
, rw_timeout_(30)
{
	this->set_conns_min(count);
}

connect_pool::~connect_pool()
{
}

void connect_pool::set_timeout(int conn_timeout, int rw_timeout)
{
	conn_timeout_ = conn_timeout;
	rw_timeout_ = rw_timeout;
}

acl::connect_client* connect_pool::create_connect()
{
	printf(">>>%s: create client for addr=%s\r\n", __FUNCTION__, addr_.c_str());
	return new connect_client(addr_.c_str(), conn_timeout_, conn_timeout_);
}
