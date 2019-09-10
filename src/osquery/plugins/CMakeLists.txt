# Copyright (c) 2014-present, Facebook, Inc.
# All rights reserved.
#
# This source code is licensed in accordance with the terms specified in
# the LICENSE file found in the root directory of this source tree.

ADD_OSQUERY_LIBRARY(osquery_pluing_config config/parsers/decorators.cpp
										  config/filesystem_config.cpp)

ADD_OSQUERY_LIBRARY(osquery_pluing_logger logger/filesystem_logger.cpp)

ADD_OSQUERY_LIBRARY(osquery_plugin_db database/ephemeral.cpp
									  database/sqlite.cpp)
