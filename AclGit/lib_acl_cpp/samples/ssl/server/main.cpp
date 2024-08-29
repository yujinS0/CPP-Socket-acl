// main.cpp : �������̨Ӧ�ó������ڵ㡣
//
#include "stdafx.h"
#include <assert.h>
#include "lib_acl.h"
#include "acl_cpp/lib_acl.hpp"

//////////////////////////////////////////////////////////////////////////////

class ssl_sni_checker : public acl::ssl_sni_checker {
public:
	ssl_sni_checker() {}
	~ssl_sni_checker() {}

	// @override
	bool check(acl::sslbase_io* io, const char* sni, acl::string& host) {
		if (io) {
			io->set_has_sni(true);
			acl::sslbase_io* me = (acl::sslbase_io*) io->get_ctx();
			if (io != me) {
				printf("Invalid io=%p, me=%p\r\n", io, me);
				return false;
			}
			printf("ssl_sni_checker::check: sslbase_io=%p\r\n", io);
		} else {
			printf("ssl_sni_checker::check: sslbase_io=NULL\r\n");
		}


		if (sni == NULL || *sni == 0) {
			printf("Invalid SNI=%p\r\n", sni);
			return false;
		}

		host = sni;

		int pos;
		if ((pos = host.find(':')) > 0 || (pos = host.find('|')) > 0) {
			host.truncate((size_t) pos);
		}

		printf("Check sni successfully, sni=%s, %zd, host=%s, %zd\r\n",
			sni, strlen(sni), host.c_str(), host.size());
		return true;
	}
};

//////////////////////////////////////////////////////////////////////////////

class echo_thread : public acl::thread {
public:
	echo_thread(acl::sslbase_conf& ssl_conf, acl::socket_stream* conn,
		int times)
	: ssl_conf_(ssl_conf), conn_(conn), times_(times) {}

private:
	acl::sslbase_conf&  ssl_conf_;
	acl::socket_stream* conn_;
	int times_;

	~echo_thread(void) {
		printf("Delete connection from %s\r\n", conn_->get_peer(true));
		delete conn_;
	}

	// @override
	void* run(void) {
		// �� socket ��װ SSL IO ����
		if (!setup_ssl()) {
			delete this;
			return NULL;
		}

		do_echo();

		delete this;
		return NULL;
	}

	bool setup_ssl(void) {
		bool non_block = false;
		acl::sslbase_io* ssl = ssl_conf_.create(non_block);

		// ����˽�ж����� ssl_sni_checker::check() �м��
		ssl->set_ctx(ssl);

		// ����ʹ�� SSL ��ʽ����������Ҫ�� SSL IO ������ע��������
		// �����������У����� ssl io �滻 stream ��Ĭ�ϵĵײ� IO ����
		if (conn_->setup_hook(ssl) == ssl) {
			printf("setup ssl IO hook error, errno=%d, %s\r\n",
				acl::last_error(), acl::last_serror());
			ssl->destroy();
			return false;
		}

		printf("ssl handshake ok, sslbase_io=%p, has sni=%s\r\n",
			ssl, ssl->has_sni() ? "yes" : "no");
		return true;
	}

	void do_echo(void) {
		char buf[4096];

		int times = 0;
		while (true) {
			int ret = conn_->read(buf, sizeof(buf), false);
			if (ret == -1) {
				if (acl::last_error() != EAGAIN) {
					break;
				}

				printf("Times of timeout: %d\r\n", ++times);

				if (times >= times_) {
					break;
				}

				continue;
			}

			times = 0;

			if (conn_->write(buf, ret) == -1) {
				break;
			}
		}
	}
};

//////////////////////////////////////////////////////////////////////////////

static void start_server(const acl::string& addr, acl::sslbase_conf& ssl_conf,
	  int rw_timeout, int times) {

	acl::server_socket ss;
	if (!ss.open(addr)) {
		printf("listen %s error %s\r\n", addr.c_str(), acl::last_serror());
		return;
	}

	while (true) {
		acl::socket_stream* conn = ss.accept();
		if (conn == NULL) {
			printf("accept error %s\r\n", acl::last_serror());
			break;
		}

		conn->set_rw_timeout(rw_timeout);
		acl::thread* thr = new echo_thread(ssl_conf, conn, times);
		thr->set_detachable(true);
		thr->start();
	}
}

//////////////////////////////////////////////////////////////////////////////

static bool ssl_init(const acl::string& ssl_crt, const acl::string& ssl_key,
	acl::sslbase_conf& ssl_conf) {

	ssl_conf.enable_cache(true);

	// ���� SSL ֤�鼰֤��˽Կ
	if (!ssl_conf.add_cert(ssl_crt, ssl_key)) {
		printf("add ssl crt=%s error\r\n", ssl_crt.c_str());
		return false;
	}

	return true;
}

static acl::sslbase_conf* load_mbedtls(acl::string& ssl_libs)
{
	if (ssl_libs.empty()) {
#if defined(__APPLE__)
		ssl_libs = "../libmbedtls.dylib";
#elif defined(__linux__)
		ssl_libs = "../libmbedtls.so";
#elif defined(_WIN32) || defined(_WIN64)
		ssl_libs = "../mbedtls.dll";
#else
# error "unknown OS type"
#endif
	}

	// ���� MbedTLS ��̬��·��
	const std::vector<acl::string>& libs = ssl_libs.split2(",; \t");
	if (libs.size() == 1) {
		acl::mbedtls_conf::set_libpath(libs[0]);
	} else if (libs.size() == 3) {
		// libcrypto, libx509, libssl);
		acl::mbedtls_conf::set_libpath(libs[0], libs[1], libs[2]);
	} else {
		printf("invalid ssl_lib=%s\r\n", ssl_libs.c_str());
		return NULL;
	}

	// ���� MbedTLS ��̬��
	if (!acl::mbedtls_conf::load()) {
		printf("load %s error\r\n", ssl_libs.c_str());
		return NULL;
	}

	// ��ʼ�������ģʽ�µ�ȫ�� SSL ���ö���
	bool server_side = true;

	// SSL ֤��У�鼶��
	acl::mbedtls_verify_t verify_mode = acl::MBEDTLS_VERIFY_NONE;

	return new acl::mbedtls_conf(server_side, verify_mode);
}

static acl::sslbase_conf* load_openssl(acl::string& ssl_libs)
{
#if defined(__APPLE__)
	acl::string libcrypto = "/usr/local/lib/libcrypto.dylib";
	acl::string libssl    = "/usr/local/lib/libssl.dylib";
#elif defined(__linux__)
	acl::string libcrypto = "/usr/local/lib64/libcrypto.so";
	acl::string libssl    = "/usr/local/lib64/libssl.so";
#else
# error "Unsupport OS!"
#endif
	if (!ssl_libs.empty()) {
		const std::vector<acl::string>& libs = ssl_libs.split2(",; \t");
		if (libs.size() >= 2) {
			libcrypto = libs[0];
			libssl = libs[1];
		} else {
			libssl = libs[0];
		}
	}

	// ���� OpenSSL ��̬��ļ���·��
	acl::openssl_conf::set_libpath(libcrypto, libssl);

	// ��̬���� OpenSSL ��̬��
	if (!acl::openssl_conf::load()) {
		printf("load ssl error=%s, crypto=%s, ssl=%s\r\n",
			acl::last_serror(), libcrypto.c_str(), libssl.c_str());
		return NULL;
	}

	bool server_side = true;

	acl::sslbase_conf* conf =  new acl::openssl_conf(server_side);
	conf->set_sni_checker(new ssl_sni_checker());
	return conf;
}

//////////////////////////////////////////////////////////////////////////////

static void usage(const char* procname) {
	printf("usage: %s -h [help]\r\n"
		" -s listen_addr[default: 0.0.0.0|1443\r\n"
		" -t ssl_type[openssl|mbedtls, default: openssl]\r\n"
		" -L ssl_libs_path\r\n"
		" -c ssl_crt[default: ../ssl_crt.pem]\r\n"
		" -k ssl_key[default: ../ssl_key.pem\r\n"
		" -r rw_timeout[default: 5]\r\n"
		" -o count_of_timeout[default: 1]\r\n"
		, procname);
}

int main(int argc, char* argv[]) {
	acl::string addr     = "0.0.0.0|1443";
	acl::string ssl_crt  = "../ssl_crt.pem", ssl_key = "../ssl_key.pem";
	acl::string ssl_type = "openssl";
	acl::string ssl_libs;
	int rw_timeout = 5, times = 1;

	int ch;
	while ((ch = getopt(argc, argv, "hs:t:L:c:k:r:o:o:")) > 0) {
		switch (ch) {
		case 'h':
			usage(argv[0]);
			return 0;
		case 's':
			addr = optarg;
			break;
		case 't':
			ssl_type = optarg;
			break;
		case 'L':
			ssl_libs = optarg;
			break;
		case 'c':
			ssl_crt = optarg;
			break;
		case 'k':
			ssl_key = optarg;
			break;
		case 'r':
			rw_timeout = atoi(optarg);
			break;
		case 'o':
			times = atoi(optarg);
			break;
		default:
			break;
		}
	}

	acl::acl_cpp_init();
	acl::log::stdout_open(true);

	acl::sslbase_conf* ssl_conf;

	if (ssl_type == "mbedtls") {
		ssl_conf = load_mbedtls(ssl_libs);
	} else if (ssl_type == "openssl") {
		ssl_conf = load_openssl(ssl_libs);
	} else {
		printf("Not support ssl type=%s\r\n", ssl_type.c_str());
		return 1;
	}

	if (!ssl_conf) {
		printf("Load ssl error, libs=%s\r\n", ssl_libs.c_str());
		return 1;
	}

	if (!ssl_init(ssl_crt, ssl_key, *ssl_conf)) {
		printf("ssl_init failed\r\n");
		return 1;
	}

	start_server(addr, *ssl_conf, rw_timeout, times);

	acl::ssl_sni_checker* checker = ssl_conf->get_sni_checker();
	delete checker;
	delete ssl_conf;
	return 0;
}
