#pragma once
#include "../acl_cpp_define.hpp"
#include "../stdlib/string.hpp"
#include "../stdlib/noncopyable.hpp"

struct ACL_AIO;
struct ACL_EVENT;

namespace acl {

// �¼���������
typedef enum {
	ENGINE_SELECT,  // select ģʽ(֧������ƽ̨)
	ENGINE_POLL,    // poll ģʽ(�� UNIX ƽ̨)
	ENGINE_KERNEL,  // kernel ģʽ(win32: iocp, Linux: epoll, FreeBsd: kqueue, Solaris: devpoll
	ENGINE_WINMSG   // win32 GUI ��Ϣģʽ
} aio_handle_type;

/**
 * ������IO���¼������࣬�����װ��ϵͳ�� select/poll/epoll/kqueue/devpoll/iocp,
 */

class aio_timer_callback;
class aio_delay_free;
class aio_timer_delay_free;

class ACL_CPP_API aio_handle : private noncopyable {
public:
	/**
	 * ���캯�������Զ�����IO�¼����棬���������������л��Զ��ͷ�
	 * @param engine_type {aio_handle_type} �����õ���������
	 *  ENGINE_SELECT: select ��ʽ��֧�� win32/unix ƽ̨
	 *  ENGINE_POLL: poll ��ʽ��֧�� unix ƽ̨
	 *  ENGINE_KERNEL: �Զ����ݸ���ϵͳƽ̨��֧�ֵĸ�Ч�ں������������
	 *  ENGINE_WINMSG: win32 ������Ϣ��ʽ��֧�� win32 ƽ̨
	 * @param nMsg {unsigned int} �� engine_type Ϊ ENGINE_WINMSG������ֵ
	 *  ���� 0 ʱ�����첽����������Ϣ�󶨣�������ȱʡ��Ϣ�󶨣�
	 *  �� engine_type Ϊ�� ENGINE_WINMSG ʱ����ֵ�������첽�����������
	 *  
	 */
	aio_handle(aio_handle_type engine_type = ENGINE_SELECT,
		unsigned int nMsg = 0);

	/**
	 * ���캯���������߽� ACL_AIO �������������������������в�����
	 * �Զ��ͷŸ� ACL_AIO ���
	 * @param handle {ACL_AIO*} ACL_AIO ���
	 */
	aio_handle(ACL_AIO* handle);

	virtual ~aio_handle();

	/**
	 * ����첽�����������Ƿ�������������������ᱻ���еĻ���
	 * ���첽���������첽�������̳У�һ�� aio_handle �������ȱʡ
	 * ���������������
	 * @param onoff {bool} �����Ƿ���������
	 */
	void keep_read(bool onoff);

	/**
	 * ����첽�������Ƿ������˳��������ݵĹ���
	 * @return {bool}
	 */
	bool keep_read() const;

	/**
	 * ���ö�ʱ��
	 * @param callback {aio_timer_callback*} ��ʱ���ص����������
	 * @param delay {int64} ��ʱ��ʱ����(΢��)
	 * @param id {unsigned int} ��ʱ��ĳ������� ID ��
	 * @return {int64} ��ʱ����Чʱ��(��1970.1.1������΢����)
	 */
#if defined(_WIN32) || defined(_WIN64)
	__int64 set_timer(aio_timer_callback* callback,
		__int64 delay, unsigned int id = 0);
#else
	long long int set_timer(aio_timer_callback* callback,
		long long int delay, unsigned int id = 0);
#endif

	/**
	 * ɾ����ʱ�������ж�ʱ�����¼�
	 * @param callback {aio_timer_callback*} ��ʱ���ص����������
	 * @return {time_t} ��ʱ����Чʱ��(��1970.1.1������΢����)
	 */
#if defined(_WIN32) || defined(_WIN64)
	__int64 del_timer(aio_timer_callback* callback);
#else
	long long int del_timer(aio_timer_callback* callback);
#endif

	/**
	 * ɾ����ʱ����ĳ��ָ�� ID �ŵĶ�ʱ����
	 * @param callback {aio_timer_callback*} ��ʱ���ص����������
	 * @param id {unsigned int} ��ʱ��ĳ������� ID ��
	 * @return {time_t} ��ʱ����Чʱ��(��1970.1.1������΢����)
	 */
#if defined(_WIN32) || defined(_WIN64)
	__int64 del_timer(aio_timer_callback* callback, unsigned int id);
#else
	long long del_timer(aio_timer_callback* callback, unsigned int id);
#endif

	/**
	 * ����ʱ����������״̬ʱ���û���Ϊ�޷��ͷŸö�ʱ��������ڴ�й¶��
	 * ͨ���˺��������Խ���������״̬�Ķ�ʱ��������δ����״̬ʱ���¼�
	 * ���������ͷ�(���� aio_delay_free::destroy())���Ӷ����Ա���
	 * �ڴ�й¶����
	 * @param callback {aio_delay_free*}
	 */
	void delay_free(aio_delay_free* callback);

	/**
	 * ��� ACL_AIO ���
	 * @return {ACL_AIO*}
	 */
	ACL_AIO* get_handle() const;

	/**
	 * ����첽���������
	 * @return {aio_handle_type}
	 */
	aio_handle_type get_engine_type() const;

	/**
	 * ��õ�ǰ���ڼ�ص��첽��������
	 * @return {int}
	 */
	int length() const;

	/**
	 * ��������첽����״̬��������׼�����첽���Ĵ�������
	 * @return {bool} �Ƿ�Ӧ��ֹ�첽����
	 */
	bool check();

	/**
	 * ��ñ����¼�ѭ�����������¼�����
	 * @return {int}
	 */
	int last_nready() const;

	/**
	 * ֪ͨ�첽��������ֹ
	 */
	void stop();

	/**
	 * �����첽������ڲ�״̬
	 */
	void reset();

	/**
	 * ���� DNS ��������ַ�б�����ʽ��ip1:port1;ip2:port2...
	 * @param addrs {const char*} DNS ��������ַ�б����磺8.8.8.8:53;1.1.1.1:53
	 * @param timeout {int} DNS ��ѯ��ʱʱ�䣨�룩
	 *  ע��set_dns �� dns_add ִ����ͬ�Ĺ���
	 */
	void set_dns(const char* addrs, int timeout);
	void dns_add(const char* addrs, int timeout);

	/**
	 * ɾ��ָ���� DNS ��������ַ�б�����ʽ��ip1:port1;ip2:port2...
	 * @param addrs {const char*} DNS ��������ַ�б�
	 */
	void dns_del(const char* addrs);

	/**
	 * ����������õ����� DNS �������б�
	 */
	void dns_clear();

	/**
	 * DNS �������б�����
	 * @return {size_t}
	 */
	size_t dns_size() const;

	/**
	 * �ж� DNS �������б��Ƿ�Ϊ��
	 * @return {bool}
	 */
	bool dns_empty() const;
	
	/**
	 * ��� DNS ��������ַ�б�
	 * @param out {std::vector<std::pair<acl::string, unsigned short> >&}
	 */
	void dns_list(std::vector<std::pair<string, unsigned short> >& out);

public:
	/**
	 * �����첽����ѭ���ĵȴ�ʱ���е��뼶����
	 * @param n {int} ������ select/poll/epoll/kqueue/devpoll
	 *  ʱ���뼶�ȴ�ʱ��
	 */
	void set_delay_sec(int n);

	/**
	 * �����첽����ѭ���ĵȴ�ʱ���е�΢�뼶����
	 * @param n {int} ������ select/poll/epoll/kqueue/devpoll
	 *  ʱ��΢�뼶�ȴ�ʱ��
	 */
	void set_delay_usec(int n);

	/**
	 * �����¼�ѭ�������ж�ʱ�������������״̬��ʱ������
	 * �ڲ�ȱʡֵ 100 ms
	 */
	void set_check_inter(int n);

	/**
	 * �����첽���Ķ���������С
	 * @param n {int} ����������С
	 */
	void set_rbuf_size(int n);

protected:
	friend class aio_stream;

	/**
	 * �첽�������� 1
	 */
	void increase();

	/**
	 * ���첽�������� 1 ʱ�Ļص��麯��
	 */
	virtual void on_increase() {}

	/**
	 * �첽�������� 1
	 */
	void decrease();

	/**
	 * ���첽�������� 1 ʱ�Ļص��麯��
	 */
	virtual void on_decrease() {}

private:
	ACL_AIO* aio_;
	bool inner_alloc_;
	bool stop_;
	int  nstream_;
	aio_handle_type engine_type_;
	aio_timer_delay_free* delay_free_timer_;

	void destroy_timer(aio_timer_callback* callback);
	static void on_timer_callback(int event_type, ACL_EVENT*,
		aio_timer_callback *callback);
};

} // namespace acl