/*
 *  Copyright (c) 2000 - 2015 Samsung Electronics Co., Ltd All Rights Reserved
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
/*
 * @file       main.cpp
 * @author     Janusz Kozerski (j.kozerski@samsung.com)
 * @version    1.0
 */

#include <iostream>
#include <boost/test/unit_test.hpp>
#include <boost/test/unit_test_log.hpp>
#include <boost/test/results_reporter.hpp>

#include <colour_log_formatter.h>

#include "common/log.h"

struct TestConfig {
	TestConfig()
	{
		boost::unit_test::unit_test_log.set_threshold_level(boost::unit_test::log_test_units);
		boost::unit_test::results_reporter::set_level(boost::unit_test::SHORT_REPORT);
		boost::unit_test::unit_test_log.set_formatter(new CCHECKER::colour_log_formatter);
	}
};

BOOST_GLOBAL_FIXTURE(TestConfig);

