#ifndef ACL_VSTREAM_NET_INCLUDE_H
#define ACL_VSTREAM_NET_INCLUDE_H

#ifdef __cplusplus
extern "C" {
#endif

#include "../stdlib/acl_define.h"
#include "../stdlib/acl_vstream.h"

/**
 * ����ĳ����ַ������UNIX�������Լ������׽��֣�
 * @param addr {const char*} ������ַ,
 *  �磺127.0.0.1:80 �����׽���(UNIXƽ̨) �磺/tmp/test.sock���� Linux ƽ̨�£�
 *  ��� addr ������ĸΪ '@'������ astract unix domain path.
 * @param qlen {int} �������еĳ���
 * @param flag {unsigned} ������־λ���μ���ACL_INET_FLAG_XXX
 * @param io_bufsize {int} ���յ��µĿͻ����׽��ֵ�IO��������С
 * @param rw_timeout {int} ���յ��µĿͻ����׽��ֵ�IO��д��ʱʱ�䣬��λΪ��
 * @return {ACL_VSTREAM*} ������ָ��
 */
ACL_API ACL_VSTREAM *acl_vstream_listen_ex(const char *addr, int qlen,
		unsigned flag, int io_bufsize, int rw_timeout);

/**
 * ����ĳ����ַ������UNIX�������Լ������׽��֣�
 * @param addr {const char*} ������ַ
 *  �磺127.0.0.1:80, �����׽���, �磺/tmp/test.sock������ַΪ ip:0 ʱ�����
 *  �˿ں��ɲ���ϵͳ�Զ�����
 * @param qlen {int} �������еĳ���
 * @return {ACL_VSTREAM*} ������ָ��
 */
ACL_API ACL_VSTREAM *acl_vstream_listen(const char *addr, int qlen);

/**
 * �Ӽ������н���һ���ͻ���������
 * @param listen_stream {ACL_VSTREAM*} ������
 * @param client_stream {ACL_VSTREAM*} ���ظ����õ� ACL_VSTREAM �ṹ��
 *  ���Ϊ�����ڲ�����һ���µ� ACL_VSTREAM ���������øýṹ�ռ�
 * @param ipbuf {char*} �����Ϊ���������洢�ͻ��˵�IP��ַ
 * @param bsize {int} ��� ipbuf ��Ϊ�գ����ʾ ipbuf �Ŀռ��С
 * @return {ACL_VSTREAM*} �����Ϊ�����ʾ�½��յĿͻ�����
 */
ACL_API ACL_VSTREAM *acl_vstream_accept_ex(ACL_VSTREAM *listen_stream,
		ACL_VSTREAM *client_stream, char *ipbuf, int bsize);

/**
 * �Ӽ������н���һ���ͻ���������
 * @param listen_stream {ACL_VSTREAM*} ������
 * @param ipbuf {char*} �����Ϊ���������洢�ͻ��˵�IP��ַ
 * @param bsize {int} ��� ipbuf ��Ϊ�գ����ʾ ipbuf �Ŀռ��С
 * @return {ACL_VSTREAM*} �����Ϊ�����ʾ�½��յĿͻ�����
 */
ACL_API ACL_VSTREAM *acl_vstream_accept(ACL_VSTREAM *listen_stream,
		char *ipbuf, int bsize);

/**
 * Զ�����ӷ�����
 * @param addr {const char*} ��������ַ, �������һ�����׽ӿڷ�����(��UNIXƽ̨),
 *  ���׽ӵ�ַ��/tmp/test.sock������ Linux ƽ̨���������ĸΪ '@' ���ʾ���ӱ�
 *  �صĳ���UNIX���׽ӿ�(Linux abstract unix domain socket)��
 *  �������һ��TCP�����������ַ��ʽΪ:
 *  remote_addr[@local_ip]|[#interface], ��:
 *  1. www.sina.com|80@60.28.250.199, ��˼�ǰ󶨱�������ַΪ:
 *     60.28.250.199, Զ������ www.sina.com �� 80 �˿�;
 *  2.  211.150.111.12|80@192.168.1.1����ʾ���󶨱��ص�ַ��
 *  3. 211.150.111.12|80#eth0����ʾ�󶨱���ָ������;
 *  4. �����OS �Զ��󶨱��� IP ��ַ�������дΪ��www.sina.com|80;
 * @param block_mode {int} �������ӻ��Ƿ��������ӣ�ACL_BLOCKING, ACL_NON_BLOCKING
 * @param conn_timeout {int} ���ӳ�ʱʱ��(��)
 * @param rw_timeout {int} �������ɹ���Ķ�д��ʱʱ�䣬��λΪ��
 * @param bufsize {int} �������ɹ���Ļ�������С
 * @param flags {unsigned*} �ǿ�ʱ�����������ʧ�ܵĴ�����Ϣ�����ӵ��������λ
 * @return {ACL_VSTREAM*} �����Ϊ�գ����ʾ���ӳɹ����������
 */
ACL_API ACL_VSTREAM *acl_vstream_connect2(const char *addr, int block_mode,
	int conn_timeout, int rw_timeout, int bufsize, unsigned *flags);
#define acl_vstream_connect_ex	acl_vstream_connect2

/**
 * Զ�����ӷ�����
 * @param addr {const char*} ��������ַ������ͬ��
 * @param block_mode {int} �������ӻ��Ƿ��������ӣ�ACL_BLOCKING, ACL_NON_BLOCKING
 * @param connect_timeout {int} ���ӳ�ʱʱ��(��)
 * @param rw_timeout {int} �������ɹ���Ķ�д��ʱʱ�䣬��λΪ��
 * @param rw_bufsize {int} �������ɹ���Ļ�������С
 * @return {ACL_VSTREAM*} �����Ϊ�գ����ʾ���ӳɹ����������
 */
ACL_API ACL_VSTREAM *acl_vstream_connect(const char *addr, int block_mode,
	int connect_timeout, int rw_timeout, int rw_bufsize);

/**
 * Զ�����ӷ�����
 * @param addr {const char*} ��������ַ������ͬ��
 * @param block_mode {int} �������ӻ��Ƿ��������ӣ�ACL_BLOCKING, ACL_NON_BLOCKING
 * @param connect_timeout {int} ���ӳ�ʱʱ��(����)
 * @param rw_timeout {int} �������ɹ���Ķ�д��ʱʱ�䣬��λΪ����
 * @param rw_bufsize {int} �������ɹ���Ļ�������С
 * @param flags {unsigned*} �ǿ�ʱ�����������ʧ�ܵĴ�����Ϣ�����ӵ��������λ
 * @return {ACL_VSTREAM*} �����Ϊ�գ����ʾ���ӳɹ����������
 */
ACL_API ACL_VSTREAM *acl_vstream_timed_connect(const char *addr, int block_mode,
	int connect_timeout, int rw_timeout, int rw_bufsize, unsigned *flags);

/**
 * ��� UDP ͨ�ţ��ú��������󶨱��� UDP ��ַ������󶨳ɹ����򴴽�
 * ACL_VSTREAM ����, �û���������� ACL_VSTREAM ����Ķ�д�ӿ�
 * @param addr {const char*} ���� UDP ��ַ����ʽ��ip:port�����������ַ ip:0
 *  ���ò���ϵͳ�Զ����䱾�ض˿ںţ����⻹֧���� UNIX ƽ̨�°� UNIX ���׽ӿڣ�
 *  UNIX ���׽ӿڵĵ�ַ��ʽΪ��{path}@udp������ {path} Ϊ���׽ӿ�·����@udp Ϊ
 *  UDP ��׺
 * @param rw_timeout {int} ��д��ʱʱ��(��)
 * @param flag {unsigned} ��־λ
 * @return {ACL_VSTREAM*} ���� NULL ��ʾ��ʧ��
 */
ACL_API ACL_VSTREAM *acl_vstream_bind(const char *addr, int rw_timeout, unsigned flag);

/**
 * ����������������Ϊ UDP IO ģʽ
 * @param stream {ACL_VSTREAM*}
 */
ACL_API void acl_vstream_set_udp_io(ACL_VSTREAM *stream);

/**
 * �����鲥����
 * @param addr {const char*} �鲥IP��ַ
 * @param iface {const char*} �����շ����ݰ��� IP ��ַ
 * @param port {int} �����շ����ݰ��� Port
 * @param timeout {int} IO ��ʱʱ�䣨�룩
 * @param flag {unsigned} ��־λ
 * @return {ACL_VSTREAM*} ���� NULL ��ʾ��ʧ��
 */
ACL_API ACL_VSTREAM *acl_vstream_bind_multicast(const char *addr,
	const char *iface, int port, int timeout, unsigned flag);

/**
 * �����鲥�� TTL ѡ�� (1--255)
 * @param sock {ACL_SOCKET}
 * @param ttl {int}
 * @return {int} ���� -1 ��ʾ����0 ��ʾ�ɹ�
 */
ACL_API int acl_multicast_set_ttl(ACL_SOCKET sock, int ttl);

/**
 * �����鲥�󶨵ı��ص�ַ
 * @param sock {ACL_SOCKET}
 * @param addr {const char*}
 * @return {int} ���� 0 ��ʾ�ɹ���-1 ��ʾʧ��
 */
ACL_API int acl_multicast_set_if(ACL_SOCKET sock, const char *addr);

/**
 * �뿪�鲥�����ô� API �ɹ��󽫲��ٽ����鲥���ݰ�
 * @param sock {ACL_SOCKET}
 * @param addr {const char*} �鲥 IP
 * @param iface {const char*} �����󶨵Ľ��� IP
 * @return {int} ���� 0 ��ʾ�ɹ���-1 ��ʾʧ��
 */
ACL_API int acl_multicast_drop(ACL_SOCKET sock, const char *addr, const char *iface);

#ifdef __cplusplus
}
#endif

#endif

