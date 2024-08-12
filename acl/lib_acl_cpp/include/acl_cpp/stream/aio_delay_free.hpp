#pragma once
#include "../acl_cpp_define.hpp"
#include "../stdlib/noncopyable.hpp"

namespace acl {

/**
 * ��Ҫ���ӳ��ͷŵ���̳д���󣬿��Ե��� aio_handle:delay_free ������
 * �ӳ����ٵ�Ŀ�ģ��������ڵݹ�����б������ͷ�ʱ�Ķ�����ǰ�ͷŵ�����
 */
class ACL_CPP_API aio_delay_free : public noncopyable {
public:
	aio_delay_free();
	virtual ~aio_delay_free();

	/**
	 * �ж���ʱ���Ƿ�����������״̬����������״̬�Ķ�ʱ����
	 * ���ܱ�ɾ���ģ����������ڴ����ش���
	 * @return {bool} �Ƿ�������״̬����������״̬�Ķ�����
	 *  ���������ٵ�
	 */
	bool locked() const;

	/**
	 * ���������������������������������ڶ�ʱ����������оͲ���
	 * �Զ����������������ٹ���
	 */
	void set_locked();

	/**
	 * ��������ȡ������������״̬
	 */
	void unset_locked();

	/**
	 * ���ٺ��������ڲ��� aio_timer_delay_free �����ж���Ҫ���ӳ��ͷ�
	 * �����������
	 */
	virtual void destroy() {}

private:
	bool locked_;
	bool locked_fixed_;
};

} // namespace acl
