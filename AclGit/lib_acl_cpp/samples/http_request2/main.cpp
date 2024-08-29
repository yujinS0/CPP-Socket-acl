// http_servlet.cpp : �������̨Ӧ�ó������ڵ㡣
//
#include <assert.h>
#include <getopt.h>
#include "acl_cpp/lib_acl.hpp"

using namespace acl;

//////////////////////////////////////////////////////////////////////////

class http_request_test
{
public:
	http_request_test(const char* server_addr, const char* domain,
		const char* url, const char* stype, const char* charset)
	{
		server_addr_= server_addr;
		domain_ = domain;
		url_ = url;
		stype_ = stype;
		charset_ = charset;
		to_charset_ = "gb2312";
	}

	~http_request_test() {}

	bool run(void)
	{
		http_request req(server_addr_);

		// ��� HTTP ����ͷ�ֶ�

		string ctype("text/");
		ctype << stype_ << "; charset=" << charset_;

		http_header& hdr = req.request_header();  // ����ͷ���������
		hdr.set_url(url_);
		hdr.set_content_type(ctype);
		hdr.set_host(domain_);

		// ���� HTTP ��������
		if (req.request(NULL, 0) == false)
		{
			logger_error("send http request to %s error",
				server_addr_.c_str());
			return false;
		}

		acl::string line;

		while (true)
		{
			if (req.body_gets(line) == false)
				break;
			printf(">>>[%s], len: %d\r\n", line.c_str(),
				(int) line.size());
			line.clear();
		}

		if (req.body_gets(line))
			printf(">>last data: %s\r\n", line.c_str());

		return true;
	}

private:
	string server_addr_;	// web ��������ַ
	string domain_;		// Host�ֶ�
	string url_;		// URL
	string stype_;		// �������ݵ�����������
	string charset_;	// �������������ļ����ַ���
	string to_charset_;	// ����������Ӧ����תΪ�����ַ���
};

//////////////////////////////////////////////////////////////////////////

static void usage(const char* procname)
{
	printf("usage: %s -h[help] -s server_addr -H host -U url\r\n", procname);
}

int main(int argc, char* argv[])
{
	int   ch;
	string server_addr("127.0.0.1:8888");
	string domain("test.com");
	string url("/");
	string stype("html"), charset("gb2312");

	while ((ch = getopt(argc, argv, "hs:H:U:")) > 0)
	{
		switch (ch)
		{
		case 'h':
			usage(argv[0]);
			return 0;
		case 's':
			server_addr = optarg;
			break;
		case 'H':
			domain = optarg;
			break;
		case 'U':
			url = optarg;
			break;
		default:
			usage(argv[0]);
			return 0;
		}
	}

	// ��ʼ����
	log::stdout_open(true);
	http_request_test test(server_addr, domain, url, stype, charset);
	test.run();

	return 0;
}

