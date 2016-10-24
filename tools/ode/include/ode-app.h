/*
 * Tizen ODE application
 *
 * Copyright (c) 2016 Samsung Electronics Co., Ltd All Rights Reserved
 *
 * Contact: SeolHeui Kim <s414.kim@samsung.com>
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 */

#ifndef __ODE_H__
#define __ODE_H__

#include <stdio.h>

#include <app.h>
#include <app_control.h>
#include <dlog.h>
#include <efl_extension.h>
#include <Elementary.h>
#include <system_settings.h>
#include <limits.h>
#include <device/battery.h>
#include <device/callback.h>
#include <runtime_info.h>
#include <vconf.h>
#include <gio/gio.h>
#include <app_control.h>
#include <aul.h>
#include <bundle.h>

#include "ode-app-widget.h"

#ifdef  LOG_TAG
#undef  LOG_TAG
#endif
#define LOG_TAG "ode"

#if !defined(PACKAGE)
#define PACKAGE "org.tizen.ode"
#endif

typedef struct dpm_encryption_info {
	int battery_status;
	int charger_status;
	int locktype_status;

	int sdcard_status;
	int sdcard_pw_status;

	Evas_Object *next_button;
} dpm_encryption_info_s;

typedef enum {
	ENCRYPT_DEVICE,
	DECRYPT_DEVICE,
	ENCRYPT_SD_CARD,
	DECRYPT_SD_CARD,
} dpm_encryption_viewtype;

typedef enum {
	ICON_CHECK,
	ICON_BAR,
} dpm_encryption_icontype;

typedef struct appdata {
	Evas_Object* win;
	Evas_Object* conform;
	Evas_Object* nf;
	Evas_Object* base;
	Evas_Object* navi_bar;
	Evas_Object* dpm_encryption_layout;
	dpm_encryption_info_s device_info;

	char* edj_path;
	char* icon_check_path;
	char* icon_bar_path;
	int view_type;
} appdata_s;

#define MSG_TEXT_STYLE "DEFAULT='font=Tizen:style=Regular font_size=30 color=#000000 wrap=mixed align=left'"
#define NEEDED_BATTERY_PERCENT 80

/* for Encrypt device */
#define IDS_ST_HEADER_ENCRYPTION "Encryption"
#define IDS_ST_MBODY_ENCRYPT_DEVICE "Encrypt device"
#define IDS_ST_MBODY_DECRYPT_DEVICE "Decrypt device"
#define IDS_ST_SBODY_DEVICE_NOT_ENCRYPTED_M_STATUS "Device not encrypted."
#define IDS_ST_SBODY_DEVICE_ENCRYPTED_M_STATUS "Device encrypted."

#define IDS_ST_MBODY_ENCRYPT_SD_CARD_ABB "Encrypt SD card"
#define IDS_ST_MBODY_DECRYPT_SD_CARD "Decrypt SD card"
#define IDS_ST_SBODY_SD_CARD_NOT_INSERTED_M_STATUS "SD card not inserted"
#define IDS_ST_SBODY_SD_CARD_NOT_ENCRYPTED_M_STATUS_SBODY "SD card not encrypted"
#define IDS_ST_SBODY_SD_CARD_ENCRYPTED_M_STATUS "SD card encrypted"
#define IDS_ST_SBODY_ENCRYPTING_ING_M_STATUS "Encrypting..."
#define IDS_ST_SBODY_DECRYPTING_ING_M_STATUS "Decrypting..."
#define IDS_ST_SBODY_FAILED_TO_ENCRYPT_SD_CARD_TAP_HERE_TO_TRY_AGAIN "Failed to encrypt SD card. Tap here to try again."
#define IDS_ST_SBODY_FAILED_TO_DECRYPT_SD_CARD_TAP_HERE_TO_TRY_AGAIN "Failed to decrypt SD card. Tap here to try again."
#define IDS_ST_SBODY_TAP_HERE_TO_ENTER_PW_FOR_SD_CARD_ABB "Tap here to enter PW for SD card."

#define IDS_ST_HEADER_ENCRYPT_DEVICE "Encrypt device"
#define IDS_ST_BODY_ENCRYPTING_MSG_1 "Encrypting your device will secure the apps, data, and other content on your device. To decrypt your device once it has been encrypted, you will need to unlock the screen whenever the device is powered on."
#define IDS_ST_BODY_ENCRYPTING_MSG_2 "The encryption process may take a long time<br>depending on the amount of data that is encrypted. Interrupting this process may cause data loss, so make sure you have time for it to finish before you start."
#define IDS_ST_BODY_TO_ENCRYPTION_YOUR_DEVICE_C_ABB "To encrypt your device:"
#define IDS_ST_BODY_CHARGE_THE_BATTERY_TO_AT_LEAST_PDP "Charge the battery to at least 80%."
#define IDS_ST_BODY_CONNECT_THE_DEVICE_TO_A_CHARGER "Connect the device to a charger."
#define IDS_ST_BODY_SET_THE_SCREEN_LOCK_TYPE_AS_PATTERN_PIN_OR_PASSWORD "Set the screen lock type as pattern, PIN, or password."
#define IDS_ST_BUTTON_SCREEN_LOCK_TYPE_ABB "Screen lock type"
#define IDS_ST_BUTTON_NEXT "Next"

/* for Decrypt device */
#define IDS_ST_HEADER_DECRYPT_DEVICE "Decrypt device"
#define IDS_ST_BODY_DECRYPTING_MSG_1 "Decrypting your device will decrypt the apps, data, and other content on your device."
#define IDS_ST_BODY_DECRYPTING_MSG_2 "This may take a long time depending on the amount of data. Interrupting this process may cause data loss, so make sure you have time for it to finish before you start."
#define IDS_ST_HEADER_TO_DECRYPT_YOUR_DEVICE_C_ABB "To decrypt your device:"

/* for Encrypt SD card */
#define IDS_ST_HEADER_ENCRYPT_SD_CARD "Encrypt SD card"
#define IDS_ST_BODY_SD_CARD_ENCRYPTING_MSG_1 "Encrypting your SD card will protect its data if it's lost or stolen. SD cards encrypted on this device can't be used on any other device."
#define IDS_ST_BODY_SD_CARD_ENCRYPTING_MSG_2 "Once the SD card is encrypted, a password is required to use it. The same password must be used for all encrypted SD cards. If you enter an incorrect password 15 times in a row, you will no longer be able to use any of the SD cards encrypted by this device."
#define IDS_ST_BODY_SD_CARD_ENCRYPTING_MSG_3 "The encryption process may take a long time depending on the amount of data that is encrypted. Interrupting this process may cause data loss, so make sure you have time for it to finish before you start."
#define IDS_ST_BODY_TO_ENCRYPT_YOUR_SD_CARD_C "To encrypt your SD card :"
#define IDS_ST_BODY_INSERT_THE_SD_CARD_ABB "Insert the SD card to encrypt."
#define IDS_ST_BODY_CREATE_AN_SD_CARD_PASSWORD "Create an SD card password."
#define IDS_ST_BUTTON_CREATE_PASSWORD "Create password"
#define IDS_ST_BUTTON_CHANGE_PW_ABB "Change PW"
#define IDS_ST_BUTTON_DELETE_PW_ABB "Delete PW"

/* for Decrypt SD card */
#define IDS_ST_HEADER_DECRYPT_SD_CARD "Decrypt SD card"
#define IDS_ST_BODY_SD_CARD_DECRYPTION_MSG "The decryption process may take a long time depending on the amount of data that is decrypted. Interrupting this process may cause data loss, so make sure you have time for it to finish before you start."
#define IDS_ST_BUTTON_CHANGE_PASSWORD_ABB2 "Change password"

/* for Encrypt Device Confirm */
#define IDS_ST_HEADER_CONFIRM_ENCRYPTION "Confirm encryption"
#define IDS_ST_BODY_CONFIRM_ENCRYPT_DEVICE_MSG_1 "Tap Encrypt device to start the encryption process."
#define IDS_ST_BODY_CONFIRM_ENCRYPT_DEVICE_MSG_2 "The encryption can't be cancelled and data will be lost if the process is interrupted. During the encryption process, your device can't be used. After the encryption, your device will restart."
#define IDS_ST_HEADER_ENCRYPTION_OPTIONS_ABB "Encryption options"
#define IDS_ST_MBODY_FAST_ENCRYPTION "Fast Encryption"
#define IDS_ST_SBODY_ENCRYPT_USED_DEVICE_STORAGE_ONLY "Encrypt used device storage only."
#define IDS_ST_BUTTON_ENCRYPT_DEVICE "Encrypt device"

/* for Decrypt Device Confirm */
#define IDS_ST_HEADER_CONFIRM_DECRYPTION "Confirm decryption"
#define IDS_ST_BODY_CONFIRM_DECRYPT_DEVICE_MSG_1 "Tap Decrypt device to start the decryption process."
#define IDS_ST_BODY_CONFIRM_DECRYPT_DEVICE_MSG_2 "The decryption can't be cancelled and data will be lost if the process is interrupted. During the decryption process, your device can't be used. After the decryption, your device will restart."
#define IDS_ST_BUTTON_DECRYPT_DEVICE "Decrypt device"

/* for Encrypt SD card Confirm */
#define IDS_ST_BODY_CONFIRM_ENCRYPT_SD_CARD_MSG_1 "Tap Encrypt SD card to start the encryption process. All open files on SD card will be closed."
#define IDS_ST_BODY_CONFIRM_ENCRYPT_SD_CARD_MSG_2 "The encryption can't be cancelled and data will be lost if the process is interrupted. During the encryption process, your SD card can't be used. After the encryption, your SD card will be mounted again."
#define IDS_ST_MBODY_ENCRYPT_NEW_FILES "Encrypt all files"
#define IDS_ST_BODY_ENCRYPT_NEW_FILES_MSG "Encrypt all files saved on the SD card. New files will also be encrypted."
#define IDS_ST_MBODY_ENCRYPT_ALL_FILES "Encrypt new files"
#define IDS_ST_BODY_ENCRYPT_ALL_FILES_MSG "Encrypt all new files created using this device. Existing files will not be encrypted."
#define IDS_ST_BUTTON_ENCRYPT_SD_CARD_ABB "Encrypt SD card"

/* for Decrypt SD Card Confirm */
#define IDS_ST_BODY_CONFIRM_DECRYPT_SD_CARD_MSG_1 "Tap Decrypt SD card to start the decryption process. All open files on SD card will be closed."
#define IDS_ST_BODY_CONFIRM_DECRYPT_SD_CARD_MSG_2 "The decryption can't be cancelled and data will be lost if the process is interrupted. During the decryption process, your SD card can't be used. After the decryption, your SD card will be mounted again."
#define IDS_ST_BUTTON_DECRYPT_SD_CARD_ABB3 "Decrypt SD card"

void dpm_encryption_create_view(appdata_s* ad);
void dpm_decryption_create_view(appdata_s* ad);
void dpm_encryption_sd_card_create_view(appdata_s* ad);
void dpm_decryption_sd_card_create_view(appdata_s* ad);

void dpm_encryption_device_confirm_create_view(appdata_s* ad);
void dpm_decryption_device_confirm_create_view(appdata_s* ad);
void dpm_encryption_sd_card_confirm_create_view(appdata_s* ad);
void dpm_decryption_sd_card_confirm_create_view(appdata_s* ad);

void locktype_status_changed_cb(keynode_t* node, void* data);
void sdcard_status_changed_cb(keynode_t* node, void* data);
void battery_changed_cb(device_callback_e type, void* value, void* user_data);

#endif							/* __ODE_H__ */
