#include "acl_stdafx.hpp"
#ifndef ACL_PREPARE_COMPILE
#include "acl_cpp/stdlib/util.hpp"
#include "acl_cpp/stdlib/log.hpp"
#include "acl_cpp/stdlib/string.hpp"
#include "acl_cpp/stdlib/dbuf_pool.hpp"
#include "acl_cpp/stream/stream_hook.hpp"
#include "acl_cpp/stream/stream.hpp"
#endif

namespace acl {

stream::stream()
: hook_(NULL)
, stream_(NULL)
, buf_(NULL)
, dbuf_(NULL)
, default_ctx_(NULL)
, ctx_table_(NULL)
, eof_(true)
, opened_(false)
{
}

stream::~stream()
{
	if (hook_) {
		hook_->destroy();
	}
	if (stream_) {
		acl_vstream_free(stream_);
	}
	delete buf_;
	delete dbuf_;
	delete ctx_table_;
}

string& stream::get_buf()
{
	if (buf_ == NULL) {
		buf_ = NEW string;
	}
	return *buf_;
}

dbuf_pool& stream::get_dbuf()
{
	if (dbuf_ == NULL) {
		dbuf_ = new dbuf_pool;
	}
	return *dbuf_;
}

bool stream::eof() const
{
	return eof_;
}

void stream::clear_eof()
{
	eof_ = false;
}

bool stream::opened() const
{
	return opened_;
}

ACL_VSTREAM* stream::get_vstream() const
{
	return stream_;
}

static bool set_sockopt_timeo(ACL_SOCKET fd, int opt, int timeout)
{
	if (timeout <= 0) {
		return true;
	}

# if defined(_WIN32) || defined(_WIN64)
	timeout *= 1000; // From seconds to millisecond.
	if (setsockopt(fd, SOL_SOCKET, opt, (const char*) &timeout, sizeof(timeout)) < 0) {
		logger_error("setsockopt error=%s, timeout=%d, opt=%d, fd=%d",
			last_serror(), timeout, opt, (int) fd);
		return false;
	}
# else   // Must be Linux or __APPLE__.
	struct timeval tm;
	tm.tv_sec  = timeout;
	tm.tv_usec = 0;

	if (setsockopt(fd, SOL_SOCKET, opt, &tm, sizeof(tm)) < 0) {
		logger_error("setsockopt error=%s, timeout=%d, opt=%d, fd=%d",
			last_serror(), timeout, opt, (int) fd);
		return false;
	}
# endif

	return true;
}

bool stream::set_rw_timeout(int n, bool use_sockopt /* false */)
{
	if (!stream_ || n <= 0) {
		return false;
	}

	if (!use_sockopt) {
		stream_->rw_timeout = n;
		return true;
	}

	stream_->rw_timeout = -1;

	if (!set_sockopt_timeo(ACL_VSTREAM_SOCK(stream_), SO_RCVTIMEO, n)) {
		return false;
	}

	return set_sockopt_timeo(ACL_VSTREAM_SOCK(stream_), SO_SNDTIMEO, n);
}

void stream::set_time_unit(time_unit_t unit) {
	if (stream_) {
		switch (unit) {
		case time_unit_ms:
			ACL_VSTREAM_SET_MS(stream_);
			break;
		case time_unit_us:
			ACL_VSTREAM_SET_US(stream_);
			break;
		case time_unit_ns:
			ACL_VSTREAM_SET_NS(stream_);
			break;
		case time_unit_s:
		default:
			ACL_VSTREAM_CLR_MS(stream_);
			ACL_VSTREAM_CLR_US(stream_);
			ACL_VSTREAM_CLR_NS(stream_);
			break;
		}
	}
}

int stream::get_rw_timeout(bool use_sockopt /* false */) const
{
	if (stream_ == NULL) {
		return -1;
	}

	if (!use_sockopt) {
		return stream_->rw_timeout;
	}

	ACL_SOCKET fd = ACL_VSTREAM_SOCK(stream_);

# if defined(_WIN32) || defined(_WIN64)
	int timeout = 0, len = sizeof(timeout);
	if (getsockopt(fd, SOL_SOCKET, SO_RCVTIMEO, (char*) &timeout, &len) < 0) {
		logger_error("getsockopt SO_RCVTIMEO error=%s, fd=%d",
			last_serror(), (int) fd);
		return -1;
	}
	return timeout / 1000;
# else
	struct timeval tm;
	memset(&tm, 0, sizeof(tm));
	socklen_t len = sizeof(tm);

	if (getsockopt(fd, SOL_SOCKET, SO_RCVTIMEO, &tm, &len) < 0) {
		logger_error("getsockopt SO_RCVTIMEO error=%s, fd=%d",
			last_serror(), (int) fd);
		return -1;
	}

	return tm.tv_sec;
# endif
}

ACL_VSTREAM* stream::unbind()
{
	eof_    = true;
	opened_ = false;
	ACL_VSTREAM* vstream = stream_;
	stream_ = NULL;
	return vstream;
}

bool stream::close()
{
	if (!opened_) {
		return false;
	}
	if (stream_ == NULL) {
		return true;
	}

	eof_    = true;
	opened_ = false;

#define	DEAD	(ACL_VSTREAM_FLAG_ERR | ACL_VSTREAM_FLAG_EOF)

	if (hook_) {
		bool alive = stream_->flag & DEAD ? false : true;
		(void) hook_->on_close(alive);
	}

	int ret = acl_vstream_close(stream_);
	stream_ = NULL;

	return ret == 0;
}

void stream::open_stream(bool is_file /* = false */)
{
	if (stream_ != NULL) {
		return;
	}
	stream_ = acl_vstream_fdopen(ACL_SOCKET_INVALID, O_RDWR, 8192, -1,
		is_file ? ACL_VSTREAM_TYPE_FILE : ACL_VSTREAM_TYPE_SOCK);
}

void stream::reopen_stream(bool is_file /* = false */)
{
	if (stream_) {
		acl_vstream_free(stream_);
	}
	open_stream(is_file);
}

bool stream::set_ctx(void* ctx, const char* key /* = NULL */,
	bool replace /* = true */)
{
	if (key == NULL || *key == 0) {
		if (!replace && default_ctx_ != NULL) {
			return false;
		}
		default_ctx_ = ctx;
		return true;
	}

	if (ctx_table_ == NULL) {
		ctx_table_ = NEW std::map<string, void*>;
	}

	if (replace) {
		(*ctx_table_)[key] = ctx;
		return true;
	}

	std::map<string, void*>::const_iterator cit = ctx_table_->find(key);
	if (cit != ctx_table_->end()) {
		return false;
	}
	(*ctx_table_)[key] = ctx;
	return true;
}

void* stream::get_ctx(const char* key /* = NULL */) const
{
	if (key == NULL || *key == 0) {
		return default_ctx_;
	}
	if (ctx_table_ == NULL) {
		return NULL;
	}
	std::map<string, void*>::const_iterator it = ctx_table_->find(key);
	if (it != ctx_table_->end()) {
		return it->second;
	} else {
		return NULL;
	}
}

void* stream::del_ctx(const char* key /* = NULL */)
{
	if (key == NULL || *key == 0) {
		if (default_ctx_ == NULL) {
			return NULL;
		}
		void* ctx    = default_ctx_;
		default_ctx_ = NULL;
		return ctx;
	}

	if (ctx_table_ == NULL) {
		return NULL;
	}

	std::map<string, void*>::iterator it = ctx_table_->find(key);
	if (it == ctx_table_->end()) {
		return NULL;
	}
	void *ctx = it->second;
	ctx_table_->erase(it);
	return ctx;
}

stream_hook* stream::get_hook() const
{
	return hook_;
}

stream_hook* stream::remove_hook()
{
	stream_hook* hook = hook_;
	hook_ = NULL;

	if (stream_->type == ACL_VSTREAM_TYPE_FILE) {
		stream_->fread_fn   = acl_file_read;
		stream_->fwrite_fn  = acl_file_write;
		stream_->fwritev_fn = acl_file_writev;
		stream_->fclose_fn  = acl_file_close;
	} else {
		stream_->read_fn    = acl_socket_read;
		stream_->write_fn   = acl_socket_write;
		stream_->writev_fn  = acl_socket_writev;
		stream_->close_fn   = acl_socket_close;
	}

	return hook;
}

#define	HOOK_KEY	"stream::setup_hook"

stream_hook* stream::setup_hook(stream_hook* hook)
{
	if (stream_ == NULL) {
		logger_error("stream_ null");
		return NULL;
	}

	stream_hook* old_hook = hook_;

	if (stream_->type == ACL_VSTREAM_TYPE_FILE) {
		ACL_FSTREAM_RD_FN read_fn  = stream_->fread_fn;
		ACL_FSTREAM_WR_FN write_fn = stream_->fwrite_fn;

		stream_->fread_fn  = fread_hook;
		stream_->fwrite_fn = fsend_hook;
		acl_vstream_add_object(stream_, HOOK_KEY, this);

		if (!hook->open(stream_)) {
			// �����ʧ�ܣ���ָ�

			stream_->fread_fn  = read_fn;
			stream_->fwrite_fn = write_fn;
			acl_vstream_del_object(stream_, HOOK_KEY);
			return hook;
		}
	} else {
		ACL_VSTREAM_RD_FN read_fn  = stream_->read_fn;
		ACL_VSTREAM_WR_FN write_fn = stream_->write_fn;

		stream_->read_fn  = read_hook;
		stream_->write_fn = send_hook;
		acl_vstream_add_object(stream_, HOOK_KEY, this);

		acl_tcp_set_nodelay(ACL_VSTREAM_SOCK(stream_));

		if (!hook->open(stream_)) {
			// �����ʧ�ܣ���ָ�

			stream_->read_fn  = read_fn;
			stream_->write_fn = write_fn;
			acl_vstream_del_object(stream_, HOOK_KEY);
			return hook;
		}
	}

	hook_ = hook;
	return old_hook;
}

int stream::read_hook(ACL_SOCKET, void *buf, size_t len, int,
	ACL_VSTREAM* vs, void *)
{
	stream* s = (stream*) acl_vstream_get_object(vs, HOOK_KEY);
	acl_assert(s);

	if (s->hook_ == NULL) {
		logger_error("hook_ null");
		return -1;
	}
	return s->hook_->read(buf, len);
}

int stream::send_hook(ACL_SOCKET, const void *buf, size_t len, int,
	ACL_VSTREAM* vs, void *)
{
	stream* s = (stream*) acl_vstream_get_object(vs, HOOK_KEY);
	acl_assert(s);

	if (s->hook_ == NULL) {
		logger_error("hook_ null");
		return -1;
	}
	return s->hook_->send(buf, len);
}

int stream::fread_hook(ACL_FILE_HANDLE, void *buf, size_t len, int,
	ACL_VSTREAM* vs, void *)
{
	stream* s = (stream*) acl_vstream_get_object(vs, HOOK_KEY);
	acl_assert(s);

	if (s->hook_ == NULL) {
		logger_error("hook_ null");
		return -1;
	}
	return s->hook_->read(buf, len);
}

int stream::fsend_hook(ACL_FILE_HANDLE, const void *buf, size_t len, int,
	ACL_VSTREAM* vs, void *)
{
	stream* s = (stream*) acl_vstream_get_object(vs, HOOK_KEY);
	acl_assert(s);

	if (s->hook_ == NULL) {
		logger_error("hook_ null");
		return -1;
	}
	return s->hook_->send(buf, len);
}

} // namespace acl
