//#include "acl_cpp/lib_acl.hpp"
//#include "pch.h"
//#include <iostream>
//#include <mysql.h>
//// mysql.cpp : �������̨Ӧ�ó������ڵ㡣
////
//
//#include "acl_cpp/lib_acl.hpp"
//
//const char* CREATE_TBL =
//"create table group_tbl\r\n"
//"(\r\n"
//"group_name varchar(128) not null,\r\n"
//"uvip_tbl varchar(32) not null default 'uvip_tbl',\r\n"
//"access_tbl varchar(32) not null default 'access_tbl',\r\n"
//"access_week_tbl varchar(32) not null default 'access_week_tbl',\r\n"
//"access_month_tbl varchar(32) not null default 'access_month_tbl',\r\n"
//"update_date date not null default '1970-1-1',\r\n"
//"disable integer not null default 0,\r\n"
//"add_by_hand integer not null default 0,\r\n"
//"class_level integer not null default 0,\r\n"
//"primary key(group_name, class_level)\r\n"
//")";
//
//static bool db_create(const char* dbaddr, const char* dbname,
//	const char* dbuser, const char* dbpass)
//{
//	acl::db_mysql db(dbaddr, "mysql", dbuser, dbpass);
//	if (db.open() == false)
//	{
//		printf("open %s@mysql error, dbuser: %s, dbpass: %s\r\n",
//			dbaddr, dbuser, dbpass);
//		return false;
//	}
//
//	acl::string sql;
//	sql.format("use mysql");
//	if (db.sql_update(sql.c_str()) == false)
//	{
//		printf("'%s' error: %s\r\n", sql.c_str(), db.get_error());
//		return false;
//	}
//	db.free_result();
//
//	sql.format("create database %s character set utf8;\r\n", dbname);
//	if (db.sql_update(sql.c_str()) == false)
//	{
//		printf("'%s' error: %s\r\n", sql.c_str(), db.get_error());
//		return false;
//	}
//	db.free_result();
//
//	sql.format("grant CREATE,DROP,INSERT,DELETE,UPDATE,SELECT on %s.* to %s",
//		dbname, dbuser);
//	if (db.sql_update(sql.c_str()) == false)
//	{
//		printf("'%s' error: %s\r\n", sql.c_str(), db.get_error());
//		return false;
//	}
//	db.free_result();
//
//	sql = "flush privileges";
//	if (db.sql_update(sql.c_str()) == false)
//	{
//		printf("'%s' error: %s\r\n", sql.c_str(), db.get_error());
//		return false;
//	}
//	db.free_result();
//
//	printf("create db(%s) ok\r\n", dbname);
//	return true;
//}
//
//static bool tbl_create(const char* dbaddr, const char* dbname,
//	const char* dbuser, const char* dbpass)
//{
//	acl::db_mysql db(dbaddr, dbname, dbuser, dbpass, 0, false, 60, 60, "utf8");
//	if (db.open() == false)
//	{
//		printf("open %s@mysql error, dbuser: %s, dbpass: %s\r\n",
//			dbaddr, dbuser, dbpass);
//		return false;
//	}
//
//	if (db.tbl_exists("group_tbl"))
//	{
//		printf("table exist\r\n");
//		return (true);
//	}
//	else if (db.sql_update(CREATE_TBL) == false)
//	{
//		printf("sql error\r\n");
//		return (false);
//	}
//	else
//	{
//		printf("create table ok\r\n");
//		return (true);
//	}
//}
//
//// ��ӱ�����
//static bool tbl_insert(acl::db_handle& db, int n)
//{
//	if (db.begin_transaction() == false)
//	{
//		printf("begin transaction false: %s\r\n", db.get_error());
//		return false;
//	}
//
//	acl::query query;
//	query.create_sql("insert into group_tbl(group_name, uvip_tbl,"
//		" update_date) values(:group, :test, :date)")
//		.set_format("group", "group:%d", n)
//		.set_parameter("test", "test")
//		.set_date("date", time(NULL), "%Y-%m-%d");
//
//	if (db.exec_update(query) == false)
//		return (false);
//
//	if (db.commit() == false)
//	{
//		printf("commit error: %s\r\n", db.get_error());
//		return false;
//	}
//
//	const acl::db_rows* result = db.get_result();
//	if (result)
//	{
//		const std::vector<acl::db_row*>& rows = result->get_rows();
//		for (size_t i = 0; i < rows.size(); i++)
//		{
//			const acl::db_row* row = rows[i];
//			for (size_t j = 0; j < row->length(); j++)
//				printf("%s, ", (*row)[j]);
//			printf("\r\n");
//		}
//	}
//
//	db.free_result();
//	return (true);
//}
//
//// ��ѯ������
//static int tbl_select(acl::db_handle& db, int n)
//{
//	acl::query query;
//	query.create_sql("select * from group_tbl where group_name=:group"
//		" and uvip_tbl=:test")
//		.set_format("group", "group:%d", n)
//		.set_format("test", "test");
//
//	if (db.exec_select(query) == false)
//	{
//		printf("select sql error\r\n");
//		return (-1);
//	}
//
//	printf("\r\n---------------------------------------------------\r\n");
//
//	// �г���ѯ�������һ
//	const acl::db_rows* result = db.get_result();
//	if (result)
//	{
//		const std::vector<acl::db_row*>& rows = result->get_rows();
//		for (size_t i = 0; i < rows.size(); i++)
//		{
//			if (n > 100)
//				continue;
//			const acl::db_row* row = rows[i];
//			for (size_t j = 0; j < row->length(); j++)
//				printf("%s, ", (*row)[j]);
//			printf("\r\n");
//		}
//	}
//
//	// �г���ѯ���������
//	for (size_t i = 0; i < db.length(); i++)
//	{
//		if (n > 100)
//			continue;
//		const acl::db_row* row = db[i];
//
//		// ȡ�����м�¼��ĳ���ֶε�ֵ
//		const char* ptr = (*row)["group_name"];
//		if (ptr == NULL)
//		{
//			printf("error, no group name\r\n");
//			continue;
//		}
//		printf("group_name=%s: ", ptr);
//		for (size_t j = 0; j < row->length(); j++)
//			printf("%s, ", (*row)[j]);
//		printf("\r\n");
//	}
//
//	// �г���ѯ���������
//	const std::vector<acl::db_row*>* rows = db.get_rows();
//	if (rows)
//	{
//		std::vector<acl::db_row*>::const_iterator cit = rows->begin();
//		for (; cit != rows->end(); cit++)
//		{
//			if (n > 100)
//				continue;
//			const acl::db_row* row = *cit;
//			for (size_t j = 0; j < row->length(); j++)
//				printf("%s, ", (*row)[j]);
//			printf("\r\n");
//		}
//
//	}
//	int  ret = (int)db.length();
//
//	// �ͷŲ�ѯ���
//	db.free_result();
//	return (ret);
//}
//
//// ɾ��������
//static bool tbl_delete(acl::db_handle& db, int n)
//{
//	acl::query query;
//	query.create_sql("delete from group_tbl where group_name=:group")
//		.set_format("group", "group-%d", n);
//
//	if (db.exec_update(query) == false)
//	{
//		printf("delete sql error\r\n");
//		return (false);
//	}
//
//	for (size_t i = 0; i < db.length(); i++)
//	{
//		const acl::db_row* row = db[i];
//		for (size_t j = 0; j < row->length(); j++)
//			printf("%s, ", (*row)[j]);
//		printf("\r\n");
//	}
//	// �ͷŲ�ѯ���
//	db.free_result();
//
//	return (true);
//}
//
//int main(void)
//{
//	// WIN32 ����Ҫ���ô˺��������й� SOCKET �ĳ�ʼ��
//	acl::acl_cpp_init();
//
//	// ����������־�������Ļ
//	acl::log::stdout_open(true);
//
//	acl::string line;
//	acl::stdin_stream in;
//	acl::stdout_stream out;
//
//#if	defined(_WIN32) || defined(_WIN64)
//	const char* libname = "libmysql.dll";
//#else
//	const char* libname = "libmysqlclient_r.so";
//#endif
//
//	acl::string path;
//
//	// ��Ϊ���ö�̬���صķ�ʽ��������ҪӦ�ø��� mysql �ͻ��˿����ڵ�·��
//	out.format("Enter %s load path: ", libname);
//	if (in.gets(line) && !line.empty())
//#if	defined(_WIN32) || defined(_WIN64)
//		//path.format("%s\\%s", line.c_str(), libname);
//		path.format("C:\\Program Files\\MySQL\\MySQL Server 8.0\\lib\\%s", libname);
//
//#else
//		path.format("%s/%s", line.c_str(), libname);
//#endif
//	else
//		path = libname;
//
//	out.format("%s path: %s\r\n", libname, path.c_str());
//	// ���ö�̬����ص�ȫ·��
//	acl::db_handle::set_loadpath(path);
//
//	acl::string dbaddr("127.0.0.1:3306");
//	acl::string dbname("acl_test_db"), dbuser("root"), dbpass;
//
//	out.format("Enter dbaddr [default: %s]: ", dbaddr.c_str());
//	if (in.gets(line) && !line.empty())
//		dbaddr = line;
//
//	out.format("Enter dbname [default: %s]: ", dbname.c_str());
//	if (in.gets(line) && !line.empty())
//		dbname = line;
//
//	out.format("Enter dbuser [default: %s]: ", dbuser.c_str());
//	if (in.gets(line) && !line.empty())
//		dbuser = line;
//
//	out.format("Enter dbpass [default: %s]: ", dbpass.c_str());
//	if (in.gets(line) && !line.empty())
//		dbpass = line;
//
//	out.format("dbname: %s, dbuser: %s, dbpass: %s\r\n",
//		dbname.c_str(), dbuser.c_str(), dbpass.c_str());
//
//	// �����Ҫ�������ݿ⣬����Ҫ�� root ��ݽ��д���
//	out.format("Do you want to create %s? yes|no: ", dbname.c_str());
//	if (in.gets(line) && (line == "yes" || line == "y"))
//	{
//		if (dbuser != "root")
//		{
//			printf("dbuser must be root for create db\r\n");
//			dbuser = "root";
//		}
//
//		// �������ݿ�
//		if (db_create(dbaddr, dbname, dbuser, dbpass) == false)
//		{
//			printf("create db failed, enter any key to exit.\r\n");
//			getchar();
//			return 1;
//		}
//		printf("create db %s ok, enter any key to continue\r\n",
//			dbname.c_str());
//		(void)in.gets(line);
//	}
//
//	// �����ݱ�����ʱ������
//	//if (tbl_create(dbaddr, dbname, dbuser, dbpass) == false)
//	//{
//	//	printf("create table error\r\n");
//	//	getchar();
//	//	return 1;
//	//}
//
//	//////////////////////////////////////////////////////////////////////
//
//	acl::db_mysql db(dbaddr, dbname, dbuser, dbpass, 0, false);
//	int   max = 100;
//
//	// �ȴ����ݿ�����
//	if (db.open() == false)
//	{
//		printf("open db(%s@%s) error\r\n",
//			dbaddr.c_str(), dbname.c_str());
//		getchar();
//		return 1;
//	}
//
//	printf("open db %s ok\r\n", dbname.c_str());
//	out.puts("Enter any key to continue ...");
//	(void)in.gets(line);
//
//	// �����������
//	for (int i = 0; i < max; i++)
//	{
//		bool ret = tbl_insert(db, i);
//		if (ret)
//			printf(">>insert ok: i=%d, affected: %d\r",
//				i, db.affect_count());
//		else
//			printf(">>insert error: i = %d\r\n", i);
//	}
//	printf("\r\n");
//	int  n = 0;
//
//	// ������ѯ����
//	for (int i = 0; i < max; i++)
//	{
//		int  ret = tbl_select(db, i);
//		if (ret >= 0)
//		{
//			n += ret;
//			printf(">>select ok: i=%d, ret=%d\r", i, ret);
//		}
//		else
//			printf(">>select error: i = %d\r\n", i);
//	}
//	printf("\r\n");
//	printf(">>select total: %d\r\n", n);
//
//	// ����ɾ������
//	for (int i = 0; i < max; i++)
//	{
//		bool ret = tbl_delete(db, i);
//		if (ret)
//			printf(">>delete ok: %d, affected: %d\r",
//				i, (int)db.affect_count());
//		else
//			printf(">>delete error: i = %d\r\n", i);
//	}
//	printf("\r\n");
//
//	//#ifndef WIN32
//	//	mysql_server_end();
//	//	mysql_thread_end();
//	//#endif
//
//	printf("mysqlclient lib's version: %ld, info: %s\r\n",
//		db.mysql_libversion(), db.mysql_client_info());
//
//	printf("Enter any key to exit.\r\n");
//	getchar();
//	return 0;
//}
//
//
///////////////////// ���� �ڵ� 
////
////
////const char* CREATE_TBL =
////"CREATE TABLE IF NOT EXISTS test_tbl ("
////"id INT AUTO_INCREMENT PRIMARY KEY,"
////"name VARCHAR(100) NOT NULL"
////")";
////
////bool create_table(acl::db_mysql& db) {
////    if (db.sql_update(CREATE_TBL) == false) {
////        std::cerr << "Failed to create table: " << db.get_error() << std::endl;
////        return false;
////    }
////    std::cout << "Table created successfully!" << std::endl;
////    return true;
////}
////
////bool insert_data(acl::db_mysql& db, const char* name) {
////    acl::query query;
////    query.create_sql("INSERT INTO test_tbl (name) VALUES (:name)")
////        .set_parameter("name", name);
////
////    if (db.exec_update(query) == false) {
////        std::cerr << "Insert error: " << db.get_error() << std::endl;
////        return false;
////    }
////    std::cout << "Data inserted successfully!" << std::endl;
////    return true;
////}
////
////bool select_data(acl::db_mysql& db) {
////    acl::query query;
////    query.create_sql("SELECT id, name FROM test_tbl");
////
////    if (db.exec_select(query) == false) {
////        std::cerr << "Select error: " << db.get_error() << std::endl;
////        return false;
////    }
////
////    const acl::db_rows* result = db.get_result();
////    if (result) {
////        const std::vector<acl::db_row*>& rows = result->get_rows();
////        for (size_t i = 0; i < rows.size(); i++) {
////            const acl::db_row* row = rows[i];
////            std::cout << "ID: " << (*row)["id"] << ", Name: " << (*row)["name"] << std::endl;
////        }
////    }
////
////    db.free_result();
////    return true;
////}
////
////
////#include <mysql.h>
////#include <iostream>
////
////int main() {
////    SetConsoleOutputCP(CP_UTF8);
////
////    const char* dbaddr = "127.0.0.1";
////    const char* dbname = "testdb";
////    const char* dbuser = "yujin";
////    const char* dbpass = "000930";
////    unsigned int port = 3306;
////
////    MYSQL* conn = mysql_init(nullptr);
////    if (!conn) {
////        std::cerr << "mysql_init() failed\n";
////        return 1;
////    }
////
////    //// SSL ���� ��� ����
////    //mysql_options(conn, MYSQL_OPT_SSL_CA, "C:/certs/ca-cert.pem");
////    //mysql_options(conn, MYSQL_OPT_SSL_CERT, "C:/certs/client-cert.pem");
////    //mysql_options(conn, MYSQL_OPT_SSL_KEY, "C:/certs/client-key.pem");
////
////    // SSL ���� ��� ����
////    //mysql_ssl_set(conn, "C:/certs/client-key.pem", "C:/certs/client-cert.pem", "C:/certs/ca-cert.pem", nullptr, nullptr);
////
////    // MySQL ������ SSL�� ����
////    if (mysql_real_connect(conn, dbaddr, dbuser, dbpass, dbname, port, nullptr, CLIENT_SSL) == nullptr) {
////        std::cerr << "Failed to connect to MySQL: " << mysql_error(conn) << std::endl;
////        mysql_close(conn);
////        return 1;
////    }
////
////    //// MySQL ������ ����
////    //if (mysql_real_connect(conn, dbaddr, dbuser, dbpass, dbname, port, nullptr, CLIENT_SSL) == nullptr) {
////    //    std::cerr << "Failed to connect to MySQL: " << mysql_error(conn) << std::endl;
////    //    mysql_close(conn);
////    //    return 1;
////    //}
////
////    std::cout << "Connected to MySQL successfully using SSL!" << std::endl;
////
////    // ���� ���� ����
////    if (mysql_query(conn, "CREATE TABLE IF NOT EXISTS test_tbl (id INT AUTO_INCREMENT PRIMARY KEY, name VARCHAR(100))")) {
////        std::cerr << "Create table failed: " << mysql_error(conn) << std::endl;
////        mysql_close(conn);
////        return 1;
////    }
////
////    std::cout << "Table created successfully!" << std::endl;
////
////    // MySQL ���� ����
////    mysql_close(conn);
////    return 0;
////}
////
//
///////////////////////////////////����
////int main() {
////    SetConsoleOutputCP(CP_UTF8);
////    acl::acl_cpp_init();  // Initialize the ACL library
////
////    const char* dbaddr = "127.0.0.1:3306";
////    const char* dbname = "testdb";
////    const char* dbuser = "yujin";
////    const char* dbpass = "000930";
////
////    // Initialize database connection
////    acl::db_mysql db(dbaddr, dbname, dbuser, dbpass, 0, false, 60, 60, "utf8");
////
////    MYSQL* mysql_handle = (MYSQL*)db.get_conn();
////
////    if (mysql_handle) {
////        // SSL ���� ��� ����
////        mysql_options(mysql_handle, MYSQL_OPT_SSL_CA, "C:/certs/ca-cert.pem");
////        mysql_options(mysql_handle, MYSQL_OPT_SSL_CERT, "C:/certs/client-cert.pem");
////        mysql_options(mysql_handle, MYSQL_OPT_SSL_KEY, "C:/certs/client-key.pem");
////    }
////    else {
////        std::cerr << "Failed to get MySQL handle!" << std::endl;
////        return 1;
////    }
////
////    // MySQL ������ ����
////    if (db.open() == false) {
////        std::cerr << "Failed to connect to MySQL server: " << db.get_error() << std::endl;
////        return 1;
////    }
////
////
////    if (db.dbopen() == false) {
////        std::cerr << "Failed to connect to MySQL server: " << db.get_error() << std::endl;
////        return 1;
////    }
////    std::cout << "Connected to MySQL successfully!" << std::endl;
////
////    // Create table
////    if (!create_table(db)) {
////        return 1;
////    }
////
////    // Insert some data
////    insert_data(db, "John Doe");
////    insert_data(db, "Jane Smith");
////
////    // Select and display data
////    select_data(db);
////
////    db.close();
////    return 0;
////}