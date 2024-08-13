#pragma once
#include "../acl_cpp_define.hpp"
#include "../stdlib/string.hpp"
#include "../stdlib/noncopyable.hpp"

#if !defined(ACL_MIME_DISABLE)

namespace acl {

/**
 * �� HTTP ͷ�е� Content-Type ��ص��ඨ�壬���Է����������ݣ�
 * Content-Type: application/x-www-form-urlencoded
 * Content-Type: multipart/form-data; boundary=xxx
 * Content-Type: application/octet-stream
 * Content-Type: text/html; charset=utf8
 * Content-Type: xxx/xxx; name=xxx
 * ...
 */
class ACL_CPP_API http_ctype : public noncopyable {
public:
	http_ctype();
	~http_ctype();

	/**
	 * ������ "=" ���������ж���ĸ���
	 * @param ctype {const http_ctype&} Դ����
	 * @return {http_ctype&}
	 */
	http_ctype& operator=(const http_ctype& ctype);

	/**
	 * ���� HTTP ͷ�� Content-Type �ֶ�ֵ
	 * @param cp {const char*} Content-Type �ֶ�ֵ���磺
	 * application/x-www-form-urlencoded
	 * multipart/form-data; boundary=xxx
	 * application/octet-stream
	 * @return {bool} ���������Ƿ�Ϸ�
	 */
	bool parse(const char* cp);

	/**
	 * ��� Content-Type �ֶ�ֵ text/html; charset=utf8 �е� text
	 * @return {const char*} ���� NULL ˵��û�и����ݣ�һ������Ϊ
	 *  parse ʧ�ܵ��µ�
	 */
	const char* get_ctype() const;

	/**
	 * ��� Content-Type �ֶ�ֵ text/html; charset=utf8 �е� html
	 * @return {const char*} ���� NULL ˵��û�и�����
	 */
	const char* get_stype() const;

	/**
	 * ��� Content-Type �ֶ�ֵ multipart/form-data; boundary=xxx
	 * �е� boundary ��ֵ xxx
	 * @return {const char*} ���� NULL ˵��û�и�����
	 */
	const char* get_bound() const;

	/**
	 * ��� Content-Type: xxx/xxx; name=name_xxx
	 * �е� name ��ֵ name_xxx
	 * @return {const char*} ���� NULL ˵��û�и�����
	 */
	const char* get_name() const;

	/**
	 * ��� Content-Type �ֶ�ֵ text/html; charset=utf8 �е� utf8
	 * @return {const char*} ���� NULL ˵��û�и�����
	 */
	const char* get_charset() const;

private:
	char* ctype_;
	char* stype_;
	char* name_;
	char* charset_;
	string* bound_;

	void reset();
};

} // namespace acl

#endif // !defined(ACL_MIME_DISABLE)