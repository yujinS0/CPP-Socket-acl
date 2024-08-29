#pragma once
#include "../acl_cpp_define.hpp"
#include <list>
#include "../stdlib/locker.hpp"
#include "../stdlib/noncopyable.hpp"

namespace acl {

class connect_manager;
class connect_client;
class thread_pool;

/**
 * �����Ͷ����˵����� connect_pool::put() ʱoper����Ϊ
 */
typedef enum {
	cpool_put_oper_none  = 0,		// �����κβ���
	cpool_put_check_idle = 1,		// ��Ⲣ�رճ�ʱ��������
	cpool_put_check_dead = (1 << 1),	// ��Ⲣ�ر��쳣����
	cpool_put_keep_conns = (1 << 2),	// �����ٳ���С������
} cpool_put_oper_t;

/**
 * �ͻ������ӳ��࣬ʵ�ֶ����ӳصĶ�̬��������Ϊ�����࣬��Ҫ����ʵ��
 * ���麯�� create_connect ���ڴ��������˵�һ�����ӣ����������
 * ��������ͨ�� set_delay_destroy() �����ӳ�����ʱ�����������ʵ��
 * �����Ƕ�̬����
 */
class ACL_CPP_API connect_pool : public noncopyable {
public:
	/**
	 * ���캯��
	 * @param addr {const char*} ������������ַ����ʽ��ip:port(domain:port)
	 * @param max {size_t} ���ӳ�������Ӹ������ƣ������ֵ��Ϊ 0��������
	 *  ���ӳص���������
	 * @param idx {size_t} �����ӳض����ڼ����е��±�λ��(�� 0 ��ʼ)
	 */
	connect_pool(const char* addr, size_t max, size_t idx = 0);

	/**
	 * ���൱������������ʱ����ʵ��ӦΪ��̬����
	 */
	virtual ~connect_pool();

	/**
	 * �˽ӿ��������ó�ʱʱ��
	 * @param conn_timeout {int} �������ӳ�ʱʱ��(��)
	 * @param rw_timeout {int} ���� IO ��ʱʱ��(��)
	 * @param sockopt_timo {bool} �Ƿ�ʹ�� setsockopt ���ö�д��ʱ
	 */
	connect_pool& set_timeout(int conn_timeout, int rw_timeout,
		bool sockopt_timo = false);

	/**
	 * �������ӳ���С����������check_idle/check_dead���������������С������
	 * @param min {size_t} > 0 ʱ���Զ�����������С������
	 * @return {connect_pool&}
	 */
	connect_pool& set_conns_min(size_t min);

	/**
	 * �������ӳ��쳣��������Ϊ����״̬��ʱ����
	 * @param retry_inter {int} �����ӶϿ��������ٴδ����ӵ�ʱ����(��)��
	 *  ����ֵ <= 0 ʱ��ʾ�������ӶϿ����������������������볬����ʱ����
	 *  �������Ͽ�������δ���ñ�����ʱ���ڲ�ȱʡֵΪ 1 ��
	 * @return {connect_pool&}
	 */
	connect_pool& set_retry_inter(int retry_inter);

	/**
	 * �������ӳ��п������ӵĿ�����������
	 * @param ttl {time_t} ���������������ڣ�����ֵ < 0 ��ʾ�������Ӳ����ڣ�
	 *  == 0 ʱ��ʾ���̹��ڣ�> 0 ��ʾ���и�ʱ��κ󽫱��ͷ�
	 * @return {connect_pool&}
	 */
	connect_pool& set_idle_ttl(time_t ttl);

	/**
	 * �����Զ����������ӵ�ʱ������ֻӰ��ÿ�ε��� put ����ʱ�ļ��ʱ����
	 * @param n {int} ʱ������ȱʡֵΪ 30 �룻�����ص��� put ʱ�Ŀ������Ӽ�⣬
	 *  ����Խ�������Ϊ -1�������� 0 ��ÿ�ζ������������
	 * @return {connect_pool&}
	 */
	connect_pool& set_check_inter(int n);

	/**
	 * �����ӳ��г����Ի�ȡһ�����ӣ��������������á����ϴη���������쳣
	 * ʱ����δ���ڻ����ӳ����Ӹ����ﵽ���������򽫷��� NULL��������һ��
	 * �µ��������������ʱʧ�ܣ�������ӳػᱻ��Ϊ������״̬
	 * @param on {bool} �ò������������ӳ�û�п�������ʱ�Ƿ񴴽��µ����ӣ�
	 *  ���Ϊ false���򲻻ᴴ��������
	 * @param tc {double*} �ǿ�ʱ�洢�ܺ�ʱʱ�䣬��λ��ms
	 * @param old {bool*} �ǿ�ʱָ����ȡ�õ������Ƿ������ӳ��еľ����ӣ�
	 *  ��� *old Ϊtrue��ʾ�����˾����ӣ�����Ϊ�½�����
	 * @return {connect_client*} ���Ϊ�����ʾ�÷��������ӳض��󲻿���
	 */
	connect_client* peek(bool on = true, double* tc = NULL, bool* old = NULL);

	/**
	 * ��һ�������ڵ�ǰ���ӳص����Ӷ����뵱ǰ���ӳض���󶨣�ʹ֮�����ڵ�ǰ
	 * ���ӳض���
	 * @param conn {redis_client*}
	 */
	void bind_one(connect_client* conn);

	/**
	 * �ͷ�һ�����������ӳ��У��������ӳض�Ӧ�ķ����������û������ϣ���ر�
	 * ������ʱ��������ӽ��ᱻֱ���ͷ�
	 * @param conn {redis_client*}
	 * @param keep {bool} �Ƿ���Ը����ӱ��ֳ�����
	 * @param oper {cpool_put_oper_t} �Զ��������ӳر�־λ������μ�����
	 *  cpool_put_oper_t ���Ͷ���
	 */
	void put(connect_client* conn, bool keep = true,
		 cpool_put_oper_t oper = cpool_put_check_idle);

	/**
	 * ������ӳ��п��е����ӣ��ͷŹ�������
	 * @param ttl {time_t} ��ֵ >= 0 ʱ������ʱ����ڴ�ֵ�����ӽ����ر�
	 * @param exclusive {bool} �ڲ��Ƿ���Ҫ����
	 * @return {size_t} ���ر��ͷſ������Ӹ���
	 */
	size_t check_idle(time_t ttl, bool exclusive = true);
	size_t check_idle(bool exclusive = true);

	/**
	 * �������״̬�����رնϿ����ӣ��ڲ��Զ���������
	 * @param threads {thread_pool*} �ǿ�ʱ��ʹ�ø��̳߳ؼ������״̬
	 * @return {size_t} ���رյ����Ӹ���
	 */
	size_t check_dead(thread_pool* threads = NULL);

	/**
	 * ���������� set_conns_min() ���õ���С������
	 * @param threads {thread_pool*} �ǿ�ʱ��ʹ�ø��̳߳ش���������
	 */
	void keep_conns(thread_pool* threads = NULL);

	/**
	 * �������ӳصĴ��״̬
	 * @param ok {bool} ���ø������Ƿ�����
	 */
	void set_alive(bool ok /* true | false */);

	/**
	 * ������ӳ��Ƿ������������ӳ�������ʱ���ú�������������ӳ��Ƿ�Ӧ��
	 * �Զ��ָ����������ָ����򽫸����ӳ�����Ϊ����״̬
	 * @return {bool} ���� true ��ʾ��ǰ���ӳش�������״̬�������ʾ��ǰ
	 *  ���ӳز�����
	 */
	bool aliving();

	/**
	 * ��ȡ���ӳصķ�������ַ
	 * @return {const char*} ���طǿյ�ַ
	 */
	const char* get_addr() const {
		return addr_;
	}

	/**
	 * ��ȡ���ӳ�������������ƣ��������ֵΪ 0 ���ʾû���������������
	 * @return {size_t}
	 */
	size_t get_max() const {
		return max_;
	}

	/**
	 * ��ȡ���ӳص�ǰ����������
	 * @return {size_t}
	 */
	size_t get_count() const {
		return count_;
	}

	/**
	 * ��ø����ӳض��������ӳؼ����е��±�λ��
	 * @return {size_t}
	 */
	size_t get_idx() const {
		return idx_;
	}

	/**
	 * ����ͳ�Ƽ�����
	 * @param inter {int} ͳ�Ƶ�ʱ����
	 */
	void reset_statistics(int inter);

	/**
	 * ��ȡ�����ӳ��ܹ���ʹ�õĴ���
	 */
	unsigned long long get_total_used() const {
		return total_used_;
	}

	/**
	 * ��ȡ�����ӳص�ǰ��ʹ�ô���
	 * @return {unsigned long long}
	 */
	unsigned long long get_current_used() const {
		return current_used_;
	}

public:
	void set_key(const char* key);
	const char* get_key(void) const {
		return key_;
	}

	// ���ӱ��������ü���
	void refer();

	// ���ٱ��������ü���
	void unrefer();

protected:
	/**
	 * ���麯������Ҫ����ʵ��
	 * @return {connect_client*}
	 */
	virtual connect_client* create_connect() = 0;

	friend class connect_manager;

	/**
	 * ���ø����ӳض���Ϊ�ӳ������٣����ڲ��������ü���Ϊ 0 ʱ����������
	 */
	void set_delay_destroy();

protected:
	bool  alive_;				// �Ƿ�������
	ssize_t refers_;			// ��ǰ���ӳض�������ü���
	bool  delay_destroy_;			// �Ƿ��������ӳ�������
	// ������ķ������Ŀ������Ե�ʱ���������������ӳض����ٴα����õ�ʱ����
	int   retry_inter_;
	time_t last_dead_;			// �����ӳض����ϴβ�����ʱ��ʱ���

	char  key_[256];			// ������ӳ���ص� key
	char  addr_[256];			// ���ӳض�Ӧ�ķ�������ַ��IP:PORT
	int   conn_timeout_;			// �������ӳ�ʱʱ��(��)
	int   rw_timeout_;			// ���� IO ��ʱʱ��(��)
	bool  sockopt_timo_;			// �Ƿ�ʹ��s setsockopt ���ó�ʱ
	size_t idx_;				// �����ӳض����ڼ����е��±�λ��
	size_t max_;				// ���������
	size_t min_;				// ��С������
	size_t count_;				// ��ǰ��������
	time_t idle_ttl_;			// �������ӵ���������
	time_t last_check_;			// �ϴμ��������ӵ�ʱ���
	int   check_inter_;			// ���������ӵ�ʱ����

	locker lock_;				// ���� pool_ ʱ�Ļ�����
	unsigned long long total_used_;		// �����ӳص����з�����
	unsigned long long current_used_;	// ĳʱ����ڵķ�����
	time_t last_;				// �ϴμ�¼��ʱ���
	std::list<connect_client*> pool_;	// ���ӳؼ���

	size_t check_dead(size_t count);
	size_t check_dead(size_t count, thread_pool& threads);
	void keep_conns(size_t min);
	void keep_conns(size_t min, thread_pool& threads);

	size_t kick_idle_conns(time_t ttl);	// �رչ��ڵ�����
	connect_client* peek_back();		// ��β�� Peek ����
	void put_front(connect_client* conn);	// ��ͷ�� Put ����

	void count_inc(bool exclusive);		// �������������������ü���
	void count_dec(bool exclusive);		// �������������������ü���
};

class ACL_CPP_API connect_guard : public noncopyable {
public:
	connect_guard(connect_pool& pool)
	: keep_(true), pool_(pool), conn_(NULL)
	{
	}

	virtual ~connect_guard(void) {
		if (conn_) {
			pool_.put(conn_, keep_);
		}
	}

	void set_keep(bool keep) {
		keep_ = keep;
	}

	connect_client* peek(void) {
		conn_ = pool_.peek();
		return conn_;
	}

protected:
	bool keep_;
	connect_pool& pool_;
	connect_client* conn_;
};

} // namespace acl
