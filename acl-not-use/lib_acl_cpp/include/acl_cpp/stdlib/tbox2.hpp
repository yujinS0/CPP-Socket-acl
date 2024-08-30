#pragma once
#include "../acl_cpp_define.hpp"
#include <list>
#include <stdlib.h>
#include "thread_mutex.hpp"
#include "thread_cond.hpp"

namespace acl {

/**
 * �����߳�֮�����Ϣͨ�ţ�ͨ���߳������������߳���ʵ��
 *
 * ʾ����
 *
 * class myobj {
 * public:
 *     myobj(void) {}
 *     ~myobj(void) {}
 *
 *     void test(void) { printf("hello world\r\n"); }
 * };
 *
 * acl::tbox2<myobj> tbox;
 *
 * void thread_producer(void) {
 *     myobj o;
 *     tbox.push(o);
 * }
 *
 * void thread_consumer(void) {
 *     myobj o;
 *     if (tbox.pop(o)) {
 *         o->test();
 *     }
 * }
 */

template<typename T>
class tbox2 {
public:
	/**
	 * ���췽��
	 */
	tbox2() : size_(0), cond_(&lock_) {}

	~tbox2() {}

	/**
	 * ������Ϣ������δ�����ѵ���Ϣ����
	 */
	void clear() {
		box_.clear();
	}

	/**
	 * ������Ϣ����
	 * @param t {T} ��Ϣ����
	 * @param notify_first {bool} ���Ϊ true������֪ͨ������������Ƚ���
	 *  ��֪ͨ��ע����ߵ�����
	 * @return {bool}
	 * @override
	 */
	bool push(T t, bool notify_first = true) {
		if (lock_.lock() == false) {
			abort();
		}

		box_.push_back(t);
		size_++;

		if (notify_first) {
			if (cond_.notify() == false) {
				abort();
			}
			if (lock_.unlock() == false) {
				abort();
			}
		} else {
			if (lock_.unlock() == false) {
				abort();
			}
			if (cond_.notify() == false) {
				abort();
			}
		}

		return true;
	}

	/**
	 * ������Ϣ����
	 * @param t {T&} ������ ���� true ʱ��Ž������
	 * @param wait_ms {int} >= 0 ʱ���õȴ���ʱʱ��(���뼶��)��
	 *  ������Զ�ȴ�ֱ��������Ϣ��������
	 * @return {bool} �Ƿ�����Ϣ����
	 * @override
	 */
	bool pop(T& t, int wait_ms = -1) {
		long long n = ((long long) wait_ms) * 1000;
		if (lock_.lock() == false) {
			abort();
		}
		while (true) {
			if (peek(t)) {
				if (lock_.unlock() == false) {
					abort();
				}
				return true;
			}

			// ע�����˳�򣬱����ȵ��� wait ���ж� wait_ms
			if (!cond_.wait(n, true) && wait_ms >= 0) {
				if (lock_.unlock() == false) {
					abort();
				}
				return false;
			}
		}
	}

	/**
	 * ���ص�ǰ��������Ϣ�����е���Ϣ����
	 * @return {size_t}
	 */
	size_t size() const {
		return size_;
	}

public:
	void lock() {
		if (lock_.lock() == false) {
			abort();
		}
	}

	void unlock() {
		if (lock_.unlock() == false) {
			abort();
		}
	}

private:
	tbox2(const tbox2&) {}
	const tbox2& operator=(const tbox2&);
private:
	std::list<T> box_;
	size_t       size_;
	thread_mutex lock_;
	thread_cond  cond_;

	bool peek(T& t) {
		typename std::list<T>::iterator it = box_.begin();
		if (it == box_.end()) {
			return false;
		}
		size_--;
		t = *it;
		box_.erase(it);
		return true;
	}
};

} // namespace acl
