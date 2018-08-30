/*
 *  Copyright (c) 2015 Samsung Electronics Co., Ltd All Rights Reserved
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

#ifndef __DPM_DOC_H__
#define __DPM_DOC_H__


 /**
  * @ingroup CAPI_SECURITY_FRAMEWORK
  * @defgroup CAPI_SECURITY_DPM_MODULE Device Policy Manager
  * @brief The Device Policy Management framework provides APIs to create
  * security-aware applications that are useful in enterprise settings.
  * <br>
  * @section CAPI_SECURITY_DPM_MODULE_OVERVIEW Overview
  * The primary purpose of the DPM(Device Policy Management) framework is
  * to support enterprise applications, which require integration with
  * policies enforced on the device to restrict their operations on
  * some circumstance, such as Exchange ActiveSync or corporate apps.<br>
  * The device policy API supports the policies listed in the below table, and
  * any application can subscribe those policies by registering policy changed callback to
  * the device policy manager.<br>
  * @subsection CAPI_SECURITY_DPM_MODULE_POLICY Policy List
  *<table>
  *  <tr>
  *   <td>Policy Name</td>
  *   <td>State</td>
  *   <td>Description</td>
  *  </tr>
  *  <tr>
  *   <td>wifi</td>
  *   <td>allowed / disallowed</td>
  *   <td>Specifies wether the use of Wi-Fi is allowed or not</td>
  *  </tr>
  *  <tr>
  *   <td>wifi-hotspot</td>
  *   <td>allowed / disallowed</td>
  *   <td>Specifies whether the use of Wi-Fi hotspot is allowed or not</td>
  *  </tr>
  *  <tr>
  *   <td>bluetooth</td>
  *   <td>allowed / disallowed</td>
  *   <td>Specifies whether the use of bluetooth is allowed or not</td>
  *  </tr>
  *  <tr>
  *   <td>camera</td>
  *   <td>allowed / disallowed</td>
  *   <td>Specifies whether the use of camera is allowed or not</td>
  *  </tr>
  *  <tr>
  *   <td>microphone</td>
  *   <td>allowed / disallowed</td>
  *   <td>Specifies whether the use of microphone is allowed or not</td>
  *  </tr>
  *  <tr>
  *   <td>location</td>
  *   <td>allowed / disallowed</td>
  *   <td>Specifies whether the use of GPS is allowed or not</td>
  *  </tr>
  *  <tr>
  *   <td>external-storage</td>
  *   <td>allowed / disallowed</td>
  *   <td>Specifies whether the use of external storage is allowed or not</td>
  *  </tr>
  *  <tr>
  *   <td>text-messaging</td>
  *   <td>allowed / disallowed</td>
  *   <td>Specifies whether the use of text messaging is allowed or not</td>
  *  </tr>
  *  <tr>
  *   <td>pop-imap-email</td>
  *   <td>allowed / disallowed</td>
  *   <td>Specifies whether the use of POP/IMAP Email is allowed or not</td>
  *  </tr>
  *  <tr>
  *   <td>browser</td>
  *   <td>allowed / disallowed</td>
  *   <td>Specifies whether the use of browser is allowed or not</td>
  *  </tr>
  *  <tr>
  *   <td>bluetooth-tethering</td>
  *   <td>allowed / disallowed</td>
  *   <td>Specifies whether the use of bluetooth tethering is allowed or not</td>
  *  </tr>
  *  <tr>
  *   <td>usb-tethering</td>
  *   <td>allowed / disallowed</td>
  *   <td>Specifies whether the use of usb tethering is allowed or not</td>
  *  </tr>
  * </table>
  * <br>
  * @subsection CAPI_DPM_SECURITY_MODULE_SIGNAL Signal List
  * <table>
  *  <tr>
  *   <td>Signal Name</td>
  *   <td>Object</td>
  *   <td>Description</td>
  *  </tr>
  *  <tr>
  *   <td>on-admin-client-registered</td>
  *   <td>Policy admin client name</td>
  *   <td>Triggered by the device policy manager when new policy admin client is registered</td>
  *  </tr>
  *  <tr>
  *   <td>on-admin-client-deregistered</td>
  *   <td>Policy admin client name</td>
  *   <td>Triggered by the device policy manager when policy admin client is deregistered</td>
  *  </tr>
  * </table>
  * <br>
  * @subsection Privilege
  * In order to enforce policy on the device, the client must be registered to the device policy
  * manager as the device admin client by enrollment process. Other unregistered
  * application are only allowed to subscribe policy change notification published by
  * the device policy manager.
  *<br>
  */

 /**
  * @ingroup CAPI_SECURITY_DPM_MODULE
  * @defgroup CAPI_DPM_MANAGER_MODULE Policy Manager Interface
  * @brief The policy client provides functions required to access the device policy APIs.
  * @section CAPI_DPM_MANAGER_MODULE_HEADER Required Header
  *  \#include <dpm/device-policy-manager.h>
  * @section CAPI_DPM_MANAGER_MODULE_OVERVIEW Overview
  * The Policy Manager module provides Device Policy Manager handle which keeps track of
  * state between the device admin client and the device policy manager.
  * The device admin clients must create the handle by using dpm_manager_create()
  * before attempting to use almost any of the device policy APIs, and the handle should be
  * freed by using dpm_manager_destroy() when interaction with the device policy manager
  * is no longer required.
  */

 /**
  * @ingroup CAPI_SECURITY_DPM_MODULE
  * @defgroup CAPI_DPM_PASSWORD_POLICY_MODULE Password policy group
  * @brief Password policy group provides APIs to control password policies.
  * @section CAPI_DPM_PASSWORD_POLICY_MODULE_HEADER Required Header
  *  \#include <dpm/password.h>
  * @section CAPI_DPM_PASSWORD_POLICY_MODULE_OVERVIEW Overview
  * Password policy group provides APIs to control password policies such as
  * password type, password quality and expiration date. Any API which requires privileges
  * is only available for the registered device admin client which is enrolled to the device
  * by enrollment process.
  */

 /**
  * @ingroup CAPI_SECURITY_DPM_MODULE
  * @defgroup CAPI_DPM_RESTRICTION_POLICY_MODULE Restriction policy group
  * @brief Restriction policy group provides APIs to control restriction functionality.
  * @section CAPI_DPM_RESTRICTION_POLICY_MODULE_HEADER Required Header
  *  \#include <dpm/restriction.h>
  * @section CAPI_DPM_RESTRICTION_POLICY_MODULE_OVERVIEW Overview
  * Restriction policy group provides APIs to restrict functions of the device such as
  * Camera, Microphone, Wi-Fi, bluetooth, USB and so on. Any API which requires privileges
  * is only available for the registered device admin client which is enrolled to the device
  * by enrollment process.
  */

 /**
  * @ingroup CAPI_SECURITY_DPM_MODULE
  * @defgroup CAPI_DPM_SECURITY_POLICY_MODULE Security policy group
  * @brief Security policy group provides APIs to control security functionality
  * @section CAPI_DPM_SECURITY_POLICY_MODULE_HEADER Required Header
  *  \#include <dpm/security.h>
  * @section CAPI_DPM_SECURITY_POLICY_MODULE_OVERVIEW Overview
  * Security policy group provides APIs to control security functionality such as
  * device encryption and screen lock. Any API which requires privileges
  * is only available for the registered device admin client which is enrolled to the device
  * by enrollment process.
  */

 /**
  * @ingroup CAPI_SECURITY_DPM_MODULE
  * @defgroup CAPI_DPM_ZONE_POLICY_MODULE Zone policy group
  * @brief Zone policy group provides APIs to control containers
  * @section CAPI_DPM_ZONE_POLICY_MODULE_HEADER Required Header
  * \#include <dpm/zone.h>
  * @section CAPI_DPM_ZONE_POLICY_MODULE_OVERVIEW Overview
  * This module provides APIs to manage application containers. Any API which requires
  * privileges is only available for the registered device policy client which is enrolled to
  * the device by enrollment process.
  *
  */

#endif /* __DPM_DOC_H__ */
