#pragma once
#include <vector>

typedef enum {
	REDIS_OBJECT_UNKOWN,
	REDIS_OBJECT_NIL,
	REDIS_OBJECT_ERROR,
	REDIS_OBJECT_STATUS,
	REDIS_OBJECT_INTEGER,
	REDIS_OBJECT_STRING,
	REDIS_OBJECT_ARRAY,
} redis_object_t;

class redis_client;

/**
 * �� redis-server ���ؽ�������࣬�� redis-server ���ص����ݽ��з����󴴽�
 * redis_object �����
 * the redis result for redis-server's reply
 */
class redis_object : public acl::noncopyable {
public:
	redis_object(acl::dbuf_pool* dbuf);

	/**
	 * ������ new/delete ���������� new �¶���ʱ��ʹ�ڴ�ķ�����
	 * �ڴ�ؽ��з���
	 * override new/delete operator, when the new object was created,
	 * memory was alloc in acl::dbuf_pool, which is a memroy pool allocator
	 */
	void *operator new(size_t size, acl::dbuf_pool* pool);
	void operator delete(void* ptr, acl::dbuf_pool* pool);

	/**
	 * ��õ�ǰ���������������
	 * get the data type of the reply from redis-server
	 * @return {redis_object_t}
	 *  defined above REDIS_OBJECT_
	 */
	redis_object_t get_type(void) const {
		return object_type_;
	}

	/**
	 * ��õ�ǰ������洢�Ķ���ĸ���
	 * get the number of objects from redis-server
	 * @return {size_t} ����ֵ��洢���͵Ķ�Ӧ��ϵ���£�
	 *  the relation between returned value and result type show below:
	 *  REDIS_OBJECT_ERROR: 1
	 *  REDIS_OBJECT_STATUS: 1
	 *  REDIS_OBJECT_INTEGER: 1
	 *  REDIS_OBJECT_STRING: > 0 ʱ��ʾ���ַ������ݱ��зֳɷ������ڴ��ĸ���
	 *  REDIS_OBJECT_ARRAY: children_->size()
	 */
	size_t get_size(void) const;

	/**
	 * ������ֵΪ REDIS_OBJECT_INTEGER ����ʱ�����������ض�Ӧ�� 32 λ����ֵ
	 * get the 32 bits integer for REDIS_OBJECT_INTEGER result
	 * @param success {bool*} ��ָ��� NULL ʱ��¼���������Ƿ�ɹ�
	 *  when not NULL, storing the status of success
	 * @return {int}
	 */
	int get_integer(bool* success = NULL) const;

	/**
	 * ������ֵΪ REDIS_OBJECT_INTEGER ����ʱ�����������ض�Ӧ�� 64 λ����ֵ
	 * get the 64 bits integer for REDIS_OBJECT_INTEGER result
	 * @param success {bool*} ��ָ��� NULL ʱ��¼���������Ƿ�ɹ�
	 *  when not NULL, storing the status of success
	 * @return {long long int}
	 */
	long long int get_integer64(bool* success = NULL) const;

	/**
	 * ������ֵΪ REDIS_OBJECT_STRING ����ʱ�����������ض�Ӧ�� double ����ֵ
	 * get the double value for REDIS_OBJECT_STRING result
	 * @param success {bool*} ��ָ��� NULL ʱ��¼���������Ƿ�ɹ�
	 *  when not NULL, storing the status of success
	 * @return {double}
	 */
	double get_double(bool* success = NULL) const;

	/**
	 * ������ֵΪ REDIS_OBJECT_STATUS ����ʱ������������״̬��Ϣ
	 * get operation status for REDIS_OBJECT_STATUS result
	 * @return {const char*} ���� "" ��ʾ����
	 *  error if empty string returned
	 */
	const char* get_status() const;

	/**
	 * ������ʱ����ֵΪ REDIS_OBJECT_ERROR ���ͣ����������س�����Ϣ
	 * when some error happened, this can get the error information
	 * @return {const char*} ���ؿմ� "" ��ʾû�г�����Ϣ
	 *  there was no error information if empty string returned
	 */
	const char* get_error(void) const;

	/**
	 * ���ض�Ӧ�±������(���������ͷ� REDIS_OBJECT_ARRAY ʱ��
	 * get the string data of associated subscript(just for the type
	 * of no REDIS_OBJECT_ARRAY)
	 * @param i {size_t} �����±�
	 *  the array's subscript
	 * @param len {size_t*} ��Ϊ�� NULL ָ��ʱ�洢���������ݵĳ���
	 *  when not NULL, the parameter will store the length of the result
	 * @return {const char*} ���� NULL ��ʾ�±�Խ��
	 *  NULL if nothing exists or the subscript is out of bounds
	 */
	const char* get(size_t i, size_t* len = NULL) const;

	/**
	 * �������е���������(���������ͷ� REDIS_OBJECT_ARRAY ʱ����ַ
	 * return all data's array if the type isn't REDIS_OBJECT_ARRAY
	 * @return {const char**}
	 */
	const char** get_argv(void) const {
		return (const char**) argv_;
	}

	/**
	 * �������е����ݳ�������(���������ͷ� REDIS_OBJECT_ARRAY ʱ����ַ
	 * return all length's array if the type isn't REDIS_OBJECT_ARRAY
	 * @return {const size_t*}
	 */
	const size_t* get_lens(void) const {
		return lens_;
	}

	/**
	 * �����������ݵ��ܳ���(���������ͷ� REDIS_OBJECT_ARRAY ʱ��
	 * return the total length of all data for no REDIS_OBJECT_ARRAY
	 * @return {size_t}
	 */
	size_t get_length(void) const;

	/**
	 * ����������Ϊ REDIS_OBJECT_STRING ����ʱ���ú��������ڴ���ŵ�����
	 * �洢�������ڴ��У�����Ҫע���ֹ�ڴ����
	 * compose a continus data for the slicing chunk data internal
	 * @param buf {acl::string&} �洢������ݣ��ڲ����ȵ��� buf.clear()
	 *  store the result
	 * @return {int} ���ݵ��ܳ��ȣ�����ֵ 0 ��ʾ�ڲ�����Ϊ��
	 *  return the total length of data, 0 if data array has no elements
	 */
	int argv_to_string(acl::string& buf) const;
	int argv_to_string(char* buf, size_t size) const;

	/**
	 * ����������Ϊ REDIS_OBJECT_ARRAY ����ʱ���ú����������е��������
	 * return the objects array when result type is REDIS_OBJECT_ARRAY
	 * @param size {size_t*} ����������ǿ�ʱ����õ�ַ������鳤��
	 *  store the array's length if size isn't NULL
	 * @return {const const redis_object*}
	 */
	const redis_object** get_children(size_t* size) const;

	/**
	 * ����������Ϊ REDIS_OBJECT_ARRAY ����ʱ���ú������ض�Ӧ�±�Ľ������
	 * get one object of the given subscript from objects array
	 * @param i {size_t} �±�ֵ
	 *  the given subscript
	 * @return {const redis_object*} ���±�ֵԽ�����������ʱ���򷵻� NULL
	 *  NULL if subscript is out of bounds or object not exist
	 */
	const redis_object* get_child(size_t i) const;

	/**
	 * ���ع��캯��������ڴ�ض���
	 * get the memory pool object set in constructor
	 * @return {acl::dbuf_pool*}
	 */
	acl::dbuf_pool* get_dbuf(void) {
		return dbuf_;
	}

	/**
	 * ����������ת�����ַ���
	 * @param out {acl::string&} �洢���(��׷�ӷ�ʽ���)
	 * @return {const acl::string&}
	 */
	const acl::string& to_string(acl::string& out) const;

private:
	~redis_object(void);

	friend class redis_client;
	void clear(void);

	redis_object& set_type(redis_object_t type);
	redis_object& set_size(size_t size);
	redis_object& put(const char* buf, size_t len);
	redis_object& put(const redis_object* rr, size_t idx);

private:
	redis_object_t object_type_;
	acl::dbuf_pool* dbuf_;

	size_t  size_;
	size_t  idx_;
	const char** argv_;
	size_t* lens_;

	//std::vector<const redis_object*>* children_;
	const redis_object** children_;
	size_t  children_size_;
	size_t  children_idx_;
};
