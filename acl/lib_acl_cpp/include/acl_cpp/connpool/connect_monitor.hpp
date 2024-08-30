#pragma once
#include "../acl_cpp_define.hpp"
#include "../stdlib/thread.hpp"
#include "../stream/aio_handle.hpp"
#include <vector>

namespace acl {

class aio_handle;
class check_client;
class connect_manager;
class rpc_service;
class socket_stream;
class aio_socket_stream;
class thread_pool;

class ACL_CPP_API connect_monitor : public thread {
public:
	/**
	 * ���캯��
	 * @param manager {connect_manager&}
	 * @param check_server {bool} �Ƿ������˷�������Բ������õķ����ַ�����������
	 */
	connect_monitor(connect_manager& manager, bool check_server = false);

	virtual ~connect_monitor();

	/**
	 * ��ϣ����������ʽ�����������ʱ����Ҫ�ȵ��ñ�������
	 * acl::rpc_service �����ӿڴ����������ڳ�ʼ��ʱ�����ñ�������
	 * ����÷�������ʽ���� IO ���
	 * @param max_threads {int} rpc_service �����̳߳������е�����߳���
	 * @param addr {const char*} ϣ�� rpc_service ��������ı�����ַ������
	 *  Ϊ�����Ļص�ַ���� UNIX ƽ̨��ʹ�����׽ӿڵ�ַ
	 * @return {connect_monitor&}
	 */
	connect_monitor& open_rpc_service(int max_threads,
		const char* addr = NULL);

	/**
	 * ���ü�ⶨʱ��������ʱ����
	 * @param n {int} ʱ�������룩
	 * @return {connect_mointor&}
	 */
	connect_monitor& set_check_inter(int n);

	/**
	 * �������ӱ����������ĳ�ʱʱ��
	 * @param n {int} ��ʱʱ�䣨�룩
	 * @return {connect_monitor&}
	 */
	connect_monitor& set_conn_timeout(int n);

	/**
	 * �Ƿ������˷�������Բ������õķ����ַ�����������
	 * @return {bool}
	 */
	bool check_server_on() const {
		return check_server_;
	}

	/**
	 * �����Ƿ������Ӽ�����������Զ��رչ��ڵĿ�������
	 * @param check_idle {bool} �Ƿ��Զ��رչ��ڵĿ�������
	 * @param kick_dead {bool} �Ƿ����������ӵĴ��״̬���ر��쳣���ӣ����ò���
	 *  Ϊ true ʱ��connect_client ������������� alive() �鷽�������������Ƿ���
	 * @param keep_conns {bool} �Ƿ����ٳ�ÿ�����ӳ�����С������
	 * @param threads {thread_pool*} �̳߳طǿ�ʱ���ᱻʹ��������������������
	 * @param step {bool} ÿ�μ�����ӳظ���
	 * @return {connect_monitor&}
	 */
	connect_monitor& set_check_conns(bool check_idle, bool kick_dead,
		bool keep_conns, thread_pool* threads = NULL, size_t step = 0);

	/**
	 * �Ƿ��Զ���Ⲣ�رչ��ڿ�������
	 * @return {bool}
	 */
	bool check_idle_on() const {
		return check_idle_;
	}

	/**
	 * �Ƿ���Ҫ����쳣���Ӳ��ر�
	 * @return {bool}
	 */
	bool kick_dead_on() const {
		return kick_dead_;
	}

	/**
	 * �Ƿ���Ҫ��������ÿ�����ӳص���С������
	 * @return {bool}
	 */
	bool keep_conns_on() const {
		return keep_conns_;
	}

	/**
	 * �� check_idle_on() ���� true ʱ������ÿ�μ���������������
	 * @return {size_t}
	 */
	size_t get_check_step() const {
		return check_step_;
	}

	/**
	 * ���ǰ�������õ��̳߳ض���
	 * @return {thread_pool*}
	 */
	thread_pool* get_threads() const {
		return threads_;
	}

	/**
	 * ֹͣ����߳�
	 * @param graceful {bool} �Ƿ������عرռ����̣����Ϊ true
	 *  �������еļ�����ӹرպ����̲߳ŷ��أ�������ֱ�Ӽ���߳�
	 *  ֱ�ӷ��أ����ܻ����һЩ���ڼ�������δ���ͷš�������ˣ����
	 *  ���ӳؼ�Ⱥ��������ǽ�����ȫ�ֵģ����Խ��˲�����Ϊ false�����
	 *  ���ӳؼ�Ⱥ������������й�������Ҫ����δ������ͷţ���Ӧ����Ϊ true
	 */
	void stop(bool graceful);

	/**
	 * ��� connect_manager ���ö���
	 * @return {connect_manager&}
	 */
	connect_manager& get_manager() const {
		return manager_;
	}

	/**
	 * �麯��������������ر�����������һ���жϸ������Ƿ��Ǵ��ģ��ûص�
	 * ���������пռ�Ϊ��ǰ����������̵߳����пռ䣬����ڸûص������в�
	 * �����������̣����򽫻�������������������߳�
	 * @param checker {check_client&} ��������ӵļ����󣬿���ͨ��
	 *  check_client ���еķ������£�
	 *  1) get_conn ��÷��������Ӿ��
	 *  2) get_addr ��÷���˵�ַ
	 *  3) set_alive ���������Ƿ���
	 *  4) close �ر�����
	 */
	virtual void nio_check(check_client& checker, aio_socket_stream& conn);

	/**
	 * ͬ�� IO ����麯�����ú������̳߳ص�ĳ�����߳̿ռ������У��������
	 * ���ر������Լ��ʵ��Ӧ�õ��������Ӵ��״̬�������ڱ�������������
	 * IO ����
	 * @param checker {check_client&} ��������ӵļ�����
	 *  check_client ����������õķ������£�
	 *  1) get_addr ��÷���˵�ַ
	 *  2) set_alive ���������Ƿ���
	 *  check_client ���н�ֹ���õķ������£�
	 *  1) get_conn ��÷��������Ӿ��
	 *  2) close �ر�����
	 */
	virtual void sio_check(check_client& checker, socket_stream& conn);

	/**
	 * �����ӳɹ�ʱ�Ļص��������������ʵ�ֱ�����
	 * @param cost {double} �ӷ����������󵽳�ʱ��ʱ�������룩
	 */
	virtual void on_connected(const check_client&, double cost) {
		(void) cost;
	}

	/**
	 * �����ӳ�ʱʱ�Ļص��������������ʵ�ֱ�����
	 * @param addr {const char*} �����ķ�������ַ����ʽ: ip:port
	 * @param cost {double} �ӷ����������󵽳�ʱ��ʱ�������룩
	 */
	virtual void on_timeout(const char* addr, double cost) {
		(void) addr;
		(void) cost;
	}

	/**
	 * �����ӷ�����ʱ���ܾ�ʱ�Ļص������������ʵ�ֱ�����
	 * @param addr {const char*} �����ķ�������ַ����ʽ: ip:port
	 * @param cost {double} �ӷ����������󵽱��Ͽ���ʱ�������룩
	 */
	virtual void on_refused(const char* addr, double cost) {
		(void) addr;
		(void) cost;
	}

public:
	// ��Ȼ����ĺ����� public �ģ���ֻ���ڲ�ʹ��
	/**
	 * �������˽������Ӻ���ô˺���
	 * @param checker {check_client&}
	 */
	void on_open(check_client& checker);

protected:
	// ���ി�麯��
	virtual void* run();

private:
	bool stop_;
	bool stop_graceful_;
	aio_handle handle_;			// ��̨����̵߳ķ��������
	connect_manager& manager_;		// ���ӳؼ��Ϲ������
	bool check_server_;			// �Ƿ������˿�����
	int   check_inter_;			// ������ӳ�״̬��ʱ����(��)
	int   conn_timeout_;			// ���ӷ������ĳ�ʱʱ��
	bool  check_idle_;			// �Ƿ��Ⲣ�رչ��ڿ�������
	bool  kick_dead_;			// �Ƿ�ɾ���쳣����
	bool  keep_conns_;			// �Ƿ񱣳�ÿ�����ӳ���С������
	size_t check_step_;			// ÿ�μ�����ӳظ�������
	thread_pool* threads_;			// �̳߳طǿս��Ტ��ִ������
	rpc_service* rpc_service_;		// �첽 RPC ͨ�ŷ�����
};

} // namespace acl
