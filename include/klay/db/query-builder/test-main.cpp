#include "table.hxx"
#include "column.hxx"
#include "expression.hxx"

#include <iostream>

using namespace qxx;

struct Admin {
	int id;
	std::string pkg;
	int uid;
	std::string key;
	int removable;
};

int main() {
	auto admin = make_table("admin", make_column("id", &Admin::id),
									 make_column("pkg", &Admin::pkg),
									 make_column("uid", &Admin::uid),
									 make_column("key", &Admin::key),
									 make_column("removable", &Admin::removable));

	std::string select1 = admin.select(&Admin::id, &Admin::pkg, &Admin::uid, &Admin::key);
	std::string select2 = admin.select(&Admin::id, &Admin::uid, &Admin::key);
	std::string select3 = admin.select(&Admin::uid, &Admin::key).where(expr(&Admin::id) > 3);
	std::string select4 = admin.selectAll().where(expr(&Admin::uid) > 3);
	std::string select5 = admin.selectAll().where(expr(&Admin::uid) > 3 && expr(&Admin::pkg) == "dpm");
	std::string select6 = admin.selectAll().where(expr(&Admin::uid) > 3 || expr(&Admin::pkg) == "dpm");

	std::cout << select1 << '\n'; // SELECT id pkg uid key FROM admin
	std::cout << select2 << '\n'; // SELECT id uid key FROM admin
	std::cout << select3 << '\n'; // SELECT uid key FROM admin WHERE id = ? 
	std::cout << select4 << '\n'; // SELECT * FROM admin WHERE uid = ?
	std::cout << select5 << '\n'; // SELECT * FROM admin WHERE uid = ? AND pkg = ?
	std::cout << select6 << '\n'; // SELECT * FROM admin WHERE uid = ? OR pkg = ?

	return 0;
}
