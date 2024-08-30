#pragma once

class connect_manager : public acl::connect_manager
{
public:
	connect_manager(size_t min_conns);

	virtual ~connect_manager();

protected:
	// ���ി�麯����ʵ��
	acl::connect_pool* create_pool(const char* addr,
		size_t count, size_t idx);

private:
	size_t min_conns_;
};
