/*
 *  Copyright (c) 2019 Samsung Electronics Co., Ltd All Rights Reserved
 *
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License
 */

#include <vist/client/query.hpp>
#include <vist/exception.hpp>

#include <gflags/gflags.h>

#include <cstdlib>
#include <iostream>
#include <string>

using namespace vist;

DEFINE_string(query, "", "Query statement to execute.");

int main(int argc, char *argv[]) try {
	gflags::SetUsageMessage("ViST default admin program.");
	gflags::ParseCommandLineFlags(&argc, &argv, true);

	if (!FLAGS_query.empty()) {
		auto rows = Query::Execute(FLAGS_query);

		std::cout << "Success to query." << std::endl;

		for (const auto& row : rows) {
			std::cout << "\tRow info >> ";
			for (const auto& col : row)
				std::cout << "Column[" << col.first << "] = " << col.second << ", ";
			std::cout << std::endl;
		}

		std::cout << "Total " << rows.size() << "-rows." << std::endl;
	}
	return EXIT_SUCCESS;
} catch(const Exception<ErrCode>& e) {
	std::cout << "Failed message: " << e.what() << std::endl;
	return EXIT_FAILURE;
} catch(const std::exception& e) {
	std::cout << "Failed message: " << e.what() << std::endl;
	return EXIT_FAILURE;
}
