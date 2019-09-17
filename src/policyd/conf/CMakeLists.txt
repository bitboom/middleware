# Copyright (c) 2019 Samsung Electronics Co., Ltd All Rights Reserved
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#     http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
#

CONFIGURE_FILE(systemd/device-policy-manager.service.in device-policy-manager.service)
CONFIGURE_FILE(systemd/device-policy-syspopup.service.in device-policy-syspopup.service)
INSTALL(FILES device-policy-manager.service DESTINATION ${SYSTEMD_UNIT_INSTALL_DIR})
INSTALL(FILES device-policy-syspopup.service DESTINATION ${SYSTEMD_UNIT_INSTALL_DIR})
INSTALL(FILES systemd/device-policy-manager.socket DESTINATION ${SYSTEMD_UNIT_INSTALL_DIR})

INSTALL(FILES dbus/org.tizen.dpm.syspopup.conf DESTINATION ${DBUS_CONFIGURE_DIR})
INSTALL(FILES dbus/org.tizen.dpm.syspopup.service DESTINATION ${DBUS_SERVICE_DIR})

CONFIGURE_FILE(pkgconfig/dpm-pil.pc.in dpm-pil.pc @ONLY)
INSTALL(FILES dpm-pil.pc DESTINATION ${LIB_INSTALL_DIR}/pkgconfig)
