#include "acl_stdafx.hpp"

#ifdef ACL_HAS_FLOCK_LOCK
# include <sys/file.h>
#endif

#ifndef ACL_PREPARE_COMPILE
# include "acl_cpp/stdlib/log.hpp"
# include "acl_cpp/stdlib/util.hpp"
# include "acl_cpp/stream/fstream.hpp"
#endif

namespace acl {

fstream::fstream()
{
}

fstream::~fstream()
{
	close();
}

void fstream::open(ACL_FILE_HANDLE fh, unsigned int oflags,
	const char* path /* = NULL */)
{
	open_stream(true);  // ���û��෽���ȴ�����������

	acl_assert(ACL_VSTREAM_FILE(stream_) == ACL_FILE_INVALID);

	stream_->fread_fn   = acl_file_read;
	stream_->fwrite_fn  = acl_file_write;
	stream_->fwritev_fn = acl_file_writev;
	stream_->fclose_fn  = acl_file_close;

	stream_->fd.h_file  = fh;
	stream_->type       = ACL_VSTREAM_TYPE_FILE;
	stream_->oflags     = oflags;
	stream_->omode      = 0600;

	opened_ = true;
	eof_    = false;

	if (path && *path) {
		acl_vstream_set_path(stream_, path);
	}
}

bool fstream::open(const char* path, unsigned int oflags, int mode)
{
	if (path == NULL || *path == 0) {
		return false;
	}

	ACL_FILE_HANDLE fh;

	fh = acl_file_open(path, oflags, mode);
	if (fh == ACL_FILE_INVALID) {
		return false;
	}

	open_stream(true);  // ���û��෽���ȴ�����������

	stream_->fread_fn   = acl_file_read;
	stream_->fwrite_fn  = acl_file_write;
	stream_->fwritev_fn = acl_file_writev;
	stream_->fclose_fn  = acl_file_close;

	stream_->fd.h_file  = fh;
	stream_->type       = ACL_VSTREAM_TYPE_FILE;
	stream_->oflags     = oflags;
	stream_->omode      = mode;

	acl_vstream_set_path(stream_, path);
	opened_ = true;
	eof_    = false;
	return true;
}

bool fstream::remove()
{
	const char* filepath = file_path();
	if (filepath == NULL || *filepath == 0) {
		return false;
	}

#if defined(_WIN32) || defined(_WIN64)
	// WINDOWS �±����ȹر��ļ����
	close();
	return ::_unlink(filepath) == 0 ? true : false;
#else
	return ::unlink(filepath) == 0 ? true : false;
#endif
}

bool fstream::rename(const char* from_path, const char* to_path)
{
	if (from_path == NULL || *from_path == 0) {
		logger_error("from_path NULL");
		return false;
	}
	if (to_path == NULL || *to_path == 0) {
		logger_error("to_path NULL");
		return false;
	}

	bool need_reopen;
	unsigned int oflags, omode;

#if defined(_WIN32) || defined(_WIN64)
	if (opened() && stream_ != NULL) {
		oflags = stream_->oflags;
		omode  = stream_->omode;
		// WINDOWS �±����ȹر��ļ����
		close();
		need_reopen = true;
	} else {
		oflags = 0;
		omode  = 0;
		need_reopen = false;
	}
#else
	oflags = 0;
	omode  = 0;
	need_reopen = false;
#endif
	if (::rename(from_path, to_path) == -1) {
		logger_error("rename from %s to %s error %s",
			from_path, to_path, last_serror());
		return false;
	}

	if (!need_reopen) {
		return true;
	}

	// ��� windows ƽ̨����Ҫ���´򿪸��ļ����
	return open(to_path, oflags, omode);
}

const char* fstream::file_path() const
{
	return stream_ ? stream_->path : NULL;
}

bool fstream::open_trunc(const char* path)
{
	return open(path, O_RDWR | O_CREAT | O_TRUNC, 0600);
}

bool fstream::create(const char* path)
{
	return open(path, O_RDWR | O_CREAT, 0600);
}

acl_off_t fstream::fseek(acl_off_t offset, int whence)
{
	if (stream_ == NULL) {
		logger_error("stream_ null");
		return -1;
	}

	acl_off_t ret = acl_vstream_fseek(stream_, offset, whence);
	eof_ = ret >= 0 ? false : true;
	return ret;
}

acl_off_t fstream::ftell(void)
{
	if (stream_ == NULL) {
		logger_error("stream_ null");
		return -1;
	}
	acl_off_t ret = acl_vstream_ftell(stream_);
	eof_ = ret >= 0 ? false : true;
	return ret;
}

bool fstream::ftruncate(acl_off_t length)
{
	if (stream_ == NULL) {
		logger_error("stream_ null");
		return false;
	}

	// ��Ҫ�Ƚ��ļ�ָ���Ƶ���ʼλ��
	if (fseek(0, SEEK_SET) < 0) {
		return false;
	}
	return acl_file_ftruncate(stream_, length) == 0 ? true : false;
}

acl_int64 fstream::fsize() const
{
	if (stream_ == NULL) {
		logger_error("stream_ null");
		return -1;
	}
	return acl_vstream_fsize(stream_);
}

acl_int64 fstream::fsize(const char* path)
{
	return acl_file_size(path);
}

ACL_FILE_HANDLE fstream::file_handle() const
{
	return stream_ ? ACL_VSTREAM_FILE(stream_) : ACL_FILE_INVALID;
}

bool fstream::lock(bool exclude /* = true */)
{
	ACL_FILE_HANDLE fd = file_handle();
	if (fd == ACL_FILE_INVALID) {
		logger_error("invalid file handle");
#ifndef ACL_WINDOWS
		errno = EBADF;
#endif
		return false;
	}

#ifdef	ACL_HAS_FLOCK_LOCK
	// Use flock mode first when flock is supported by OS.
	return acl_myflock(fd, ACL_FLOCK_STYLE_FLOCK,
		exclude ?  LOCK_EX : LOCK_SH) == 0;
#else
	return acl_myflock(fd, ACL_FLOCK_STYLE_FCNTL,
		exclude ? ACL_FLOCK_OP_EXCLUSIVE : ACL_FLOCK_OP_SHARED) == 0;
#endif
}

bool fstream::try_lock(bool exclude /* = true */)
{
	ACL_FILE_HANDLE fd = file_handle();
	if (fd == ACL_FILE_INVALID) {
		logger_error("invalid file handle");
#ifndef ACL_WINDOWS
		errno = EBADF;
#endif
		return false;
	}

#ifdef	ACL_HAS_FLOCK_LOCK
	return acl_myflock(fd, ACL_FLOCK_STYLE_FLOCK,
		exclude ?  (LOCK_EX | LOCK_NB) : (LOCK_SH | LOCK_NB)) == 0;
#else
	return acl_myflock(fd, ACL_FLOCK_STYLE_FCNTL,
		exclude ? (ACL_FLOCK_OP_NOWAIT | ACL_FLOCK_OP_EXCLUSIVE)
			: (ACL_FLOCK_OP_NOWAIT | ACL_FLOCK_OP_SHARED)) == 0;
#endif
}

bool fstream::unlock()
{
	ACL_FILE_HANDLE fd = file_handle();
	if (fd == ACL_FILE_INVALID) {
		logger_error("invalid file handle");
#ifndef ACL_WINDOWS
		errno = EBADF;
#endif
		return false;
	}

#ifdef	ACL_HAS_FLOCK_LOCK
	return acl_myflock(fd, ACL_FLOCK_STYLE_FLOCK, ACL_FLOCK_OP_NONE) == 0;
#else
	return acl_myflock(fd, ACL_FLOCK_STYLE_FCNTL, ACL_FLOCK_OP_NONE) == 0;
#endif
}

} // namespace acl