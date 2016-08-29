#
# Copyright (c) 2015 Samsung Electronics Co., Ltd All Rights Reserved
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

SET(POLICY      ${POLICY}
				bluetooth.cpp
				location.cpp
				password.cpp
				krate.cpp
				password-manager.cpp
)

SET(DEPENDENCY  ${DEPENDENCY}
				capi-location-manager
				capi-network-bluetooth
				auth-fw-admin
				bluetooth-api
				krate
)
