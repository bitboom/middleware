/*
 * Copyright (c) 2011 Samsung Electronics Co., Ltd All Rights Reserved
 *
 *    Licensed under the Apache License, Version 2.0 (the "License");
 *    you may not use this file except in compliance with the License.
 *    You may obtain a copy of the License at
 *
 *        http://www.apache.org/licenses/LICENSE-2.0
 *
 *    Unless required by applicable law or agreed to in writing, software
 *    distributed under the License is distributed on an "AS IS" BASIS,
 *    WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *    See the License for the specific language governing permissions and
 *    limitations under the License.
 */
/*
 * @file        colors.cpp
 * @author      Lukasz Wrzosek (l.wrzosek@samsung.com)
 * @version     1.0
 * @brief       Some constants with definition of colors for Console
 *              and html output
 */
#include <stddef.h>
#include <dpl/colors.h>

namespace AuthPasswd {
namespace Colors {
namespace Text {
const char *BOLD_GREEN_BEGIN = "\033[1;32m";
const char *BOLD_GREEN_END = "\033[m";
const char *RED_BEGIN = "\033[0;31m";
const char *RED_END = "\033[m";
const char *PURPLE_BEGIN = "\033[0;35m";
const char *PURPLE_END = "\033[m";
const char *GREEN_BEGIN = "\033[0;32m";
const char *GREEN_END = "\033[m";
const char *CYAN_BEGIN = "\033[0;36m";
const char *CYAN_END = "\033[m";
const char *BOLD_RED_BEGIN = "\033[1;31m";
const char *BOLD_RED_END = "\033[m";
const char *BOLD_YELLOW_BEGIN = "\033[1;33m";
const char *BOLD_YELLOW_END = "\033[m";
const char *BOLD_GOLD_BEGIN = "\033[0;33m";
const char *BOLD_GOLD_END = "\033[m";
const char *BOLD_WHITE_BEGIN = "\033[1;37m";
const char *BOLD_WHITE_END = "\033[m";
} //namespace Text

namespace Html {
const char *BOLD_GREEN_BEGIN = "<font color=\"green\"><b>";
const char *BOLD_GREEN_END = "</b></font>";
const char *PURPLE_BEGIN = "<font color=\"purple\"><b>";
const char *PURPLE_END = "</b></font>";
const char *RED_BEGIN = "<font color=\"red\"><b>";
const char *RED_END = "</b></font>";
const char *GREEN_BEGIN = "<font color=\"green\">";
const char *GREEN_END = "</font>";
const char *CYAN_BEGIN = "<font color=\"cyan\">";
const char *CYAN_END = "</font>";
const char *BOLD_RED_BEGIN = "<font color=\"red\"><b>";
const char *BOLD_RED_END = "</b></font>";
const char *BOLD_YELLOW_BEGIN = "<font color=\"yellow\"><b>";
const char *BOLD_YELLOW_END = "</b></font>";
const char *BOLD_GOLD_BEGIN = "<font color=\"gold\"><b>";
const char *BOLD_GOLD_END = "</b></font>";
const char *BOLD_WHITE_BEGIN = "<font color=\"white\"><b>";
const char *BOLD_WHITE_END = "</b></font>";
} //namespace Html
} //namespace Colors
} //namespace AuthPasswd
