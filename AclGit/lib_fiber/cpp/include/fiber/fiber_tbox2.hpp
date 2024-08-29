#pragma once
#include "fiber_cpp_define.hpp"
#include <list>
#include <stdlib.h>
#include "fiber_mutex.hpp"
#include "fiber_cond.hpp"

namespace acl {

/**
 * ����Э��֮�䣬�߳�֮���Լ�Э�����߳�֮�����Ϣͨ�ţ�ͨ��Э����������
 * ��Э���¼���ʵ��
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
 * acl::fiber_tbox2<myobj> tbox;
 *
 * void thread_producer(void) {
 *     myobj o;
 *     tbox.push(o);
 * }
 *
 * void thread_consumer(void) {
 *     myobj o;

 *     if (tbox.pop(o)) {
 *         o.test();
 *     }
 * }
 */

// The fiber_tbox2 has an object copying process in push/pop which is suitable
// for transfering the object managed by std::shared_ptr.

template<typename T>
class fiber_tbox2 {
public:
	/**
	 * ���췽��
	 */
	fiber_tbox2(void) : size_(0) {}

	~fiber_tbox2(void) {}

	/**
	 * ������Ϣ������δ�����ѵ���Ϣ����
	 */
	void clear(void) {
		tbox_.clear();
	}

	/**
	 * ������Ϣ����
	 * @param t {T} ��Ϣ����
	 * @param notify_first {bool} ���������Ϊ true�����ڲ��������Ϣ��
	 *  ������֪ͨ�������ʽ����������Ƚ�����֪ͨ��ʽ���� fiber_tbox2 ����
	 *  ���������ڱȽϳ�ʱ���ò�����Ϊ false ��Ч�ʸ��ߣ���� fiber_tbox2
	 *  ������������ڽ϶�(�磺�ȴ��ߵ��� pop ��ֱ������ fiber_tbox2 ����),
	 *  �򱾲���Ӧ����Ϊ true���Ա��� push �߻�û����ȫ����ǰ�� fiber_tbox2
	 *  ������ǰ���ٶ�����ڴ�Ƿ�����
	 * @return {bool}
	 */
	bool push(T t, bool notify_first = true) {
		// �ȼ���
		if (mutex_.lock() == false) {
			abort();
		}

		// ������������Ϣ����
		tbox_.push_back(t);
		size_++;

		if (notify_first) {
			if (cond_.notify() == false) {
				abort();
			}
			if (mutex_.unlock() == false) {
				abort();
			}
			return true;
		} else {
			if (mutex_.unlock() == false) {
				abort();
			}
			if (cond_.notify() == false) {
				abort();
			}
			return true;
		}
	}

	/**
	 * ������Ϣ����
	 * @param t {T&} ������ ���� true ʱ��Ž������
	 * @param wait_ms {int} >= 0 ʱ���õȴ���ʱʱ��(���뼶��)��
	 *  ������Զ�ȴ�ֱ��������Ϣ��������
	 * @return {bool} �Ƿ�����Ϣ����
	 */
	bool pop(T& t, int wait_ms = -1) {
		if (mutex_.lock() == false) {
			abort();
		}
		while (true) {
			if (peek_obj(t)) {
				if (mutex_.unlock() == false) {
					abort();
				}
				return true;
			}

			if (!cond_.wait(mutex_, wait_ms) && wait_ms >= 0) {
				if (mutex_.unlock() == false) {
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
	size_t size(void) const {
		return size_;
	}

public:
	void lock(void) {
		if (mutex_.lock() == false) {
			abort();
		}
	}

	void unlock(void) {
		if (mutex_.unlock() == false) {
			abort();
		}
	}

private:
	fiber_tbox2(const fiber_tbox2&) {}
	const fiber_tbox2& operator=(const fiber_tbox2&);

private:
	std::list<T>  tbox_;
	size_t        size_;
	fiber_mutex   mutex_;
	fiber_cond    cond_;

	bool peek_obj(T& t) {
		typename std::list<T>::iterator it = tbox_.begin();
		if (it == tbox_.end()) {
			return false;
		}
		size_--;
		t = *it;
		tbox_.erase(it);
		return true;
	}
};

} // namespace acl
