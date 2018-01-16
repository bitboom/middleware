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
	std::string select3 = admin.select(&Admin::uid, &Admin::key)
							   .where(expr(&Admin::id) > 3);
	std::string select4 = admin.selectAll().where(expr(&Admin::uid) > 3);
	std::string select5 = admin.selectAll().where(expr(&Admin::uid) > 3 &&
												  expr(&Admin::pkg) == "dpm");
	std::string select6 = admin.selectAll().where(expr(&Admin::uid) > 3 ||
												  expr(&Admin::pkg) == "dpm");

	std::cout << select1 << '\n'; // SELECT id pkg uid key FROM admin
	std::cout << select2 << '\n'; // SELECT id uid key FROM admin
	std::cout << select3 << '\n'; // SELECT uid key FROM admin WHERE id = ? 
	std::cout << select4 << '\n'; // SELECT * FROM admin WHERE uid = ?
	std::cout << select5 << '\n'; // SELECT * FROM admin WHERE uid = ? AND pkg = ?
	std::cout << select6 << '\n'; // SELECT * FROM admin WHERE uid = ? OR pkg = ?

	int uid = 0, id = 1;
	std::string update1 = admin.update(&Admin::id, &Admin::pkg, &Admin::uid, &Admin::key);
	std::string update2 = admin.update(&Admin::key).where(expr(&Admin::uid) == uid &&
														  expr(&Admin::id) == id);
	std::string update3 = admin.update(&Admin::key, &Admin::pkg)
							   .where(expr(&Admin::uid) == 0 && expr(&Admin::id) == 1);
	std::cout << update1 << '\n'; // UPDATE admin SET id = ? pkg = ? uid = ? key = ?
	std::cout << update2 << '\n'; // UPDATE admin SET key = ?  WHERE uid = ? AND id = ?
	std::cout << update3 << '\n'; // UPDATE admin SET key = ?  WHERE uid = ? AND id = ?

	std::string delete1 = admin.remove();
	std::string delete2 = admin.where(expr(&Admin::pkg) == "dpm" && expr(&Admin::uid) == 3);
	std::cout << delete1 << '\n'; // DELETE FROM admin
	std::cout << delete2 << '\n'; // DELETE FROM admin WHERE pkg = ? AND uid = ?

	std::string insert1 = admin.insert(&Admin::id, &Admin::pkg, &Admin::uid, &Admin::key);
	std::string insert2 = admin.insert(&Admin::id, &Admin::pkg, &Admin::key);
	std::cout << insert1 << '\n'; // INSERT INTO admin (id, pkg, uid, key) VALUES (?, ?, ?, ?)
	std::cout << insert2 << '\n'; // INSERT INTO admin (id, pkg, key) VALUES (?, ?, ?)

	return 0;
}
