#pragma once
#include "../acl_cpp_define.hpp"
#include <stdarg.h>

#ifndef ACL_LOGGER_MACRO_OFF

# define logger_open	acl::log::open
# define logger_close	acl::log::close

# if defined(_WIN32) || defined(_WIN64)

#  if _MSC_VER >= 1500
#   define logger(fmt, ...)  \
        acl::log::msg4(__FILE__, __LINE__, __FUNCTION__, fmt, __VA_ARGS__)
#   define logger_warn(fmt, ...)  \
        acl::log::warn4(__FILE__, __LINE__, __FUNCTION__, fmt, __VA_ARGS__)
#   define logger_error(fmt, ...)  \
        acl::log::error4(__FILE__, __LINE__, __FUNCTION__, fmt, __VA_ARGS__)
#   define logger_fatal(fmt, ...)  \
        acl::log::fatal4(__FILE__, __LINE__, __FUNCTION__, fmt, __VA_ARGS__)
#   define logger_debug(section, level, fmt, ...)  \
        acl::log::msg6(section, level, __FILE__, __LINE__, __FUNCTION__, fmt, __VA_ARGS__)
#  else
#   define logger       acl::log::msg1
#   define logger_warn  acl::log::warn1
#   define logger_error acl::log::error1
#   define logger_fatal acl::log::fatal1
#   define logger_debug acl::log::msg3
#  endif

# else

#  ifdef ACL_CPP_LOG_SKIP_FILE
#   define logger(fmt, args...)  \
        acl::log::msg4("none", __LINE__, __FUNCTION__, fmt, ##args)
#   define logger_warn(fmt, args...)  \
        acl::log::warn4("none", __LINE__, __FUNCTION__, fmt, ##args)
#   define logger_error(fmt, args...)  \
        acl::log::error4("none", __LINE__, __FUNCTION__, fmt, ##args)
#   define logger_fatal(fmt, args...)  \
        acl::log::fatal4("none", __LINE__, __FUNCTION__, fmt, ##args)
#   define logger_debug(section, level, fmt, args...)  \
        acl::log::msg6(section, level, __FILE__, __LINE__, __FUNCTION__, fmt, ##args)
#  else
#   define logger(fmt, args...)  \
        acl::log::msg4(__FILE__, __LINE__, __FUNCTION__, fmt, ##args)
#   define logger_warn(fmt, args...)  \
        acl::log::warn4(__FILE__, __LINE__, __FUNCTION__, fmt, ##args)
#   define logger_error(fmt, args...)  \
        acl::log::error4(__FILE__, __LINE__, __FUNCTION__, fmt, ##args)
#   define logger_fatal(fmt, args...)  \
        acl::log::fatal4(__FILE__, __LINE__, __FUNCTION__, fmt, ##args)
#   define logger_debug(section, level, fmt, args...)  \
        acl::log::msg6(section, level, __FILE__, __LINE__, __FUNCTION__, fmt, ##args)
#  endif

# endif
#endif // ACL_LOG_MACRO_OFF

namespace acl {

class string;

class ACL_CPP_API log {
public:
	/**
	 * 댔역휑羚匡숭, 瞳넋埼놓迦뺏쟁딧痰굶변鑒寧늴
	 * @param recipients {const char*} 휑羚쌈澗포죗깊，譚 "|" 롸몰，쌈澗포
	 *  옵鹿角굶뒈匡숭샀陶넋敬쌈왯，흔:
	 *  /tmp/test.log|UDP:127.0.0.1:12345|TCP:127.0.0.1:12345|UNIX:/tmp/test.sock
	 *  맡토零狼헹쉥杰唐휑羚谿珂랙못 /tmp/test.log, UDP:127.0.0.1:12345,
	 *  TCP:127.0.0.1:12345 뵨 UNIX:/tmp/test.sock 愷몸휑羚쌈澗포뚤蹶
	 * @param procname 넋埼츰, 흔: test
	 * @param cfg 딧桿휑羚토零, 목駕槨: {section}:{level}; {section}:{level}; ...
	 *  흔: 100:2; 101:3; 102: 4, 깊刻怜션쩌깃街槨 100/섬깎 < 2,
	 *  鹿섟깃街槨 101/섬깎 < 3, 鹿섟깃街槨 102/섬깎 < 4 돨휑羚淃
	 */
	static void open(const char* recipients, const char* procname = "unknown",
		const char* cfg = NULL);

	/**
	 * 넋埼藁놔품딧痰늪변鑒밑균휑羚
	 */
	static void close(void);

	/**
	 * 놓迦뺏휑羚딧桿딧痰쌈왯
	 * @param cfg {const char*} 딧桿깃푯섟섬깎俚륜눔, 목駕흔苟:
	 *  {section}:{level}; {section}:{level}; ...
	 *  흔: 1:1, 2:10, 3:8...  or 1:1; 2:10; 3:8... or all:1
	 */
	static void debug_init(const char* cfg);

	/**
	 * 뎠灌繫법 open 댔역휑羚직랍딧痰션휑羚된宮밑변鑒珂角뤠矜狼쉥斤口
	 * 渴놔逞깃硫渴놔
	 * @param onoff {bool}
	 */
	static void stdout_open(bool onoff);

	/**
	 * 휑羚션쩌변鑒
	 */

	static void ACL_CPP_PRINTF(1, 2) msg1(const char* fmt, ...);
	static void ACL_CPP_PRINTF(4, 5) msg4(const char* fname,
		int line, const char* func, const char* fmt, ...);
	static void ACL_CPP_PRINTF(3, 4) msg3(size_t section,
		size_t level, const char* fmt, ...);
	static void ACL_CPP_PRINTF(6, 7) msg6(size_t section, size_t level,
		const char* fname, int line, const char* func, const char* fmt, ...);

	static void ACL_CPP_PRINTF(1, 2) warn1(const char* fmt, ...);
	static void ACL_CPP_PRINTF(4, 5) warn4(const char* fname, int line,
		const char* func, const char* fmt, ...);

	static void ACL_CPP_PRINTF(1, 2) error1(const char* fmt, ...);
	static void ACL_CPP_PRINTF(4, 5) error4(const char* fname, int line,
		const char* func, const char* fmt, ...);

	static void ACL_CPP_PRINTF(1, 2) fatal1(const char* fmt, ...);
	static void ACL_CPP_PRINTF(4, 5) fatal4(const char* fname, int line,
		const char* func, const char* fmt, ...);

	static void vmsg2(const char* fmt, va_list ap);
	static void vmsg5(const char* fname, int line, const char* func,
		const char* fmt, va_list ap);
	static void vmsg4(size_t section, size_t level, const char* fmt, va_list ap);
	static void vmsg7(size_t section, size_t level, const char* fname,
		int line, const char* func, const char* fmt, va_list ap);

	static void vwarn2(const char* fmt, va_list ap);
	static void vwarn5(const char* fname, int line, const char* func,
		const char* fmt, va_list ap);

	static void verror2(const char* fmt, va_list ap);
	static void verror5(const char* fname, int line, const char* func,
		const char* fmt, va_list ap);

	static void vfatal2(const char* fmt, va_list ap);
	static void vfatal5(const char* fname, int line, const char* func,
		const char* fmt, va_list ap);

	/************************************************************************/
	/*                        刻절                                          */
	/************************************************************************/

#ifndef ACL_LOGGER_MACRO_OFF

	static void logger_test1(void) {
# define DEBUG_TEST_BASE	100
# define DEBUG_TEST1		(DEBUG_TEST_BASE + 1)
# define DEBUG_TEST2		(DEBUG_TEST_BASE + 2)
# define DEBUG_TEST3		(DEBUG_TEST_BASE + 3)

		const char* logfile = "test.log", *procname = "test";
		const char* cfg = "101:2; 102:3; 103:2";

		// 瞳넋埼놓迦뺏珂댔역휑羚
		logger_open(logfile, procname, cfg);

# if defined(VC2003) || defined(VC2002) || defined(VC6)

		// 삔畇휑羚

		logger("%s(%d), %s: %s", __FILE__, __LINE__, __FUNCTION__, "zsx");

		logger_debug(DEBUG_TEST1, 1, "%s(%d), %s: hello world11(%s)!",
			__FILE__, __LINE__, __FUNCTION__, "zsx");
		logger_debug(DEBUG_TEST2, 3, "%s(%d), %s: hello world12(%s)!",
			__FILE__, __LINE__, __FUNCTION__, "zsx");
		logger_debug(DEBUG_TEST3, 2, "%s(%d), %s: hello world13(%s)!",
			__FILE__, __LINE__, __FUNCTION__, "zsx");

		// 꼇삔畇휑羚

		logger_debug(DEBUG_TEST1, 3, "%s(%d), %s: hello world21(%s)!",
			__FILE__, __LINE__, __FUNCTION__, "zsx");

# else	// VC2005, VC2008, VC2010

		// 삔畇휑羚

		logger("error(%s)!", "zsx");

		logger_debug(DEBUG_TEST1, 1, "hello world11(%s)!", "zsx");
		logger_debug(DEBUG_TEST2, 3, "hello world12(%s)!", "zsx");
		logger_debug(DEBUG_TEST3, 2, "hello world13(%s)!", "zsx");

		// 꼇삔畇휑羚

		logger_debug(DEBUG_TEST1, 3, "hello world21(%s)!", "zsx");

# endif

		// 넋埼써監품밑균휑羚
		logger_close();
	}

	static void logger_test2(void) {
		logger("logger ok!");
		logger_warn("logger_warn ok!");
		logger_error("logger_error ok!");
		logger_fatal("logger_fatal ok!");
	}

#endif // ACL_LOGGER_MACRO_OFF
};

} // namespace acl
