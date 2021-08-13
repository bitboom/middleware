/**
 * Copyright (c) 2016 Samsung Electronics Co., Ltd All Rights Reserved
 *
 *    Licensed under the Apache License, Version 2.0 (the "License");
 *    you may not use this file except in compliance with the License.
 *    You may obtain a copy of the License at
 *
 *    http://www.apache.org/licenses/LICENSE-2.0
 *
 *    Unless required by applicable law or agreed to in writing, software
 *    distributed under the License is distributed on an "AS IS" BASIS,
 *    WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *    See the License for the specific language governing permissions and
 *    limitations under the License.
 */
/*
 * @file        ccert.h
 * @author      Bartlomiej Grzelewski (b.grzelewski@samsung.com)
 * @version     1.0
 * @brief       This is part of C api for ValidationCore.
 */
#ifndef _CERTSVC_CCERT_H_
#define _CERTSVC_CCERT_H_

#include <time.h>

#include <cert-svc/cinstance.h>
#include <cert-svc/cstring.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct CertSvcCertificate_t {
	size_t privateHandler;
	CertSvcInstance privateInstance;
} CertSvcCertificate;

typedef struct CertSvcCertificateList_t {
	size_t privateHandler;
	CertSvcInstance privateInstance;
} CertSvcCertificateList;

#define MAX_STORE_ENUMS 5
typedef enum certImportType_t {
	NONE_STORE   =  0,
	VPN_STORE    =  1 << 0,
	WIFI_STORE   =  1 << 1,
	EMAIL_STORE  =  1 << 2,
	SYSTEM_STORE =  1 << 3,
	ALL_STORE    =  VPN_STORE | WIFI_STORE | EMAIL_STORE | SYSTEM_STORE
} CertStoreType;

typedef enum certStatus_t {
	DISABLED     =  0,
	ENABLED      =  1
} CertStatus;

typedef struct CertSvcStoreCertList_t {
	char *gname;            // keyfile group name
	char *title;            // common Name / Alias provided by the user
	CertStatus status;
	CertStoreType storeType;
	struct CertSvcStoreCertList_t *next;
} CertSvcStoreCertList;

typedef enum certType_t {
	PEM_CRT          = 1 << 0,
	P12_END_USER     = 1 << 1,
	P12_INTERMEDIATE = 1 << 2,
	P12_TRUSTED      = 1 << 3,
	P12_PKEY         = 1 << 4,
	INVALID_DATA     = 1 << 5
} CertType;

typedef enum CertSvcCertificateForm_t {
	/*    CERTSVC_FORM_PEM, */
	CERTSVC_FORM_DER,
	CERTSVC_FORM_DER_BASE64
} CertSvcCertificateForm;

typedef enum CertSvcCertificateField_t {
	CERTSVC_SUBJECT,
	CERTSVC_SUBJECT_COMMON_NAME,
	CERTSVC_SUBJECT_COUNTRY_NAME,
	CERTSVC_SUBJECT_STATE_NAME,
	CERTSVC_SUBJECT_LOCALITY_NAME,
	CERTSVC_SUBJECT_ORGANIZATION_NAME,
	CERTSVC_SUBJECT_ORGANIZATION_UNIT_NAME,
	CERTSVC_SUBJECT_EMAIL_ADDRESS,
	/*    CERTSVC_SUBJECT_UID, */
	CERTSVC_ISSUER,
	CERTSVC_ISSUER_COMMON_NAME,
	CERTSVC_ISSUER_COUNTRY_NAME,
	CERTSVC_ISSUER_STATE_NAME,
	CERTSVC_ISSUER_LOCALITY_NAME,
	CERTSVC_ISSUER_ORGANIZATION_NAME,
	CERTSVC_ISSUER_ORGANIZATION_UNIT_NAME,
	CERTSVC_ISSUER_EMAIL_ADDRESS,
	/*    CERTSVC_ISSUER_UID, */
	CERTSVC_VERSION,
	CERTSVC_SERIAL_NUMBER,
	CERTSVC_KEY_USAGE,
	CERTSVC_KEY,
	CERTSVC_KEY_ALGO,
	CERTSVC_SIGNATURE_ALGORITHM
} CertSvcCertificateField;

typedef enum CertSvcVisibility_t {
	CERTSVC_VISIBILITY_DEVELOPER            = 1,
	CERTSVC_VISIBILITY_PUBLIC               = 1 << 6,
	CERTSVC_VISIBILITY_PARTNER              = 1 << 7,
	CERTSVC_VISIBILITY_PLATFORM             = 1 << 10
} CertSvcVisibility;

/**
 * @breif Get certificate with gname provided.
 *
 * @remarks @a certificate should be released using certsvc_certificate_free().
 *
 * @param[in]  instance     CertSvcInstance returned by certsvc_instance_new().
 * @param[in]  storeType    cert-svc store type to query.
 * @oaran[in]  gname        Single certificate identifier.
 * @param[out] certificate  A pointer of CertSvcCertificate.
 *
 * @return #CERTSVC_SUCCESS on success, otherwise a zero or negative error value
 *
 * @see certsvc_instance_new()
 * @see certsvc_instance_free()
 * @see certsvc_certificate_free()
 * @see #CertStoreType
 * @see #CertSvcCertificate
 */
int certsvc_get_certificate(CertSvcInstance instance,
							CertStoreType storeType,
							const char *gname,
							CertSvcCertificate *certificate);

/**
 * @brief Load certificate from file.
 *
 * @remarks A file of @a location must be in PEM/CER/DER format.
 * @remarks @a certificate should be released using certsvc_certificate_free().
 *
 * @param[in]  instance     CertSvcInstance returned by certsvc_instance_new().
 * @param[in]  location     A path of file to load.
 * @param[out] certificate  A pointer of CertSvcCertificate.
 *
 * @return #CERTSVC_SUCCESS on success, otherwise a zero or negative error value
 *
 * @see certsvc_instance_new()
 * @see certsvc_instance_free()
 * @see certsvc_certificate_free()
 * @see #CertSvcCertificate
 */
int certsvc_certificate_new_from_file(CertSvcInstance instance,
									  const char *location,
									  CertSvcCertificate *certificate);

/**
 * @breif Load certificate from memory.
 *
 * @remarks @a certificate should be released using certsvc_certificate_free().
 *
 * @param[in]  instance     CertSvcInstance returned by certsvc_instance_new().
 * @param[in]  memory       A pointer to memory with certificate data.
 * @param[in]  len          Size of certificate in @a memory.
 * @param[in]  form         Certificate format in @a memory.
 * @param[out] certificate  A pointer of CertSvcCertificate.
 *
 * @return #CERTSVC_SUCCESS on success, otherwise a zero or negative error value
 *
 * @see certsvc_instance_new()
 * @see certsvc_instance_free()
 * @see certsvc_certificate_free()
 * @see #CertSvcCertificate
 * @see #CertSvcCertificateForm
 */
int certsvc_certificate_new_from_memory(CertSvcInstance instance,
										const unsigned char *memory,
										size_t len,
										CertSvcCertificateForm form,
										CertSvcCertificate *certificate);

/**
 * @brief Release structures connected with certificate.
 *
 * @param[in] certificate  Certificate to free.
 */
void certsvc_certificate_free(CertSvcCertificate certificate);

/**
 * @breif Save certificate to file in DER format.
 *
 * @param[in] certificate  Certificate to be saved.
 * @param[in] location     A path of file to save.
 *
 * @return #CERTSVC_SUCCESS on success, otherwise a zero or negative error value
 *
 * @see #CertSvcCertificate
 */
int certsvc_certificate_save_file(CertSvcCertificate certificate,
								  const char *location);

/**
 * @breif Get certificate from certificate list.
 *
 * @remarks Can be called multiple times to get all results.
 * @remarks Returned certificate can be freed.
 * @remarks certsvc_certificate_list_free() doesn't free certificates in the list.
 *
 * @param[in]  handler     CertSvcCertificateList handler.
 * @param[in]  position    List index start from 0.
 * @param[out] certificate A pointer of CertSvcCertificate.
 *
 * @return #CERTSVC_SUCCESS on success, otherwise a zero or negative error value
 *
 * @see certsvc_certificate_free()
 * @see certsvc_certificate_list_free()
 * @see #CertSvcCertificate
 * @see #CertSvcCertificateList
 */
int certsvc_certificate_list_get_one(CertSvcCertificateList handler,
									 size_t position,
									 CertSvcCertificate *certificate);

/**
 * @brief Get number of elements on the list.
 *
 * @param[in]  handler  CertSvcCertificateList handler.
 * @param[out] length   A size of @a handler.
 *
 * @return #CERTSVC_SUCCESS on success, otherwise a zero or negative error value
 *
 * @see #CertSvcCertificateList
 */
int certsvc_certificate_list_get_length(CertSvcCertificateList handler,
										size_t *size);

/**
 * @brief Free CertSvcCertificateList.
 *
 * @remarks It will not free certificates on the list.
 * @remarks You may free each certificate with certsvc_certificate_free().
 *
 * @param[in] handler  CertSvcCertificateList handler.
 *
 * @see certsvc_certificate_free()
 * @see #CertSvcCertificateList
 */
void certsvc_certificate_list_free(CertSvcCertificateList handler);

/**
 * @brief Free CertSvcCertificateList and all certificates on the list.
 *
 * @remarks Should not free each certificate with certsvc_certificate_free().
 *
 * @param[in] handler  CertSvcCertificateList handler.
 */
void certsvc_certificate_list_all_free(CertSvcCertificateList handler);

/**
 * @brief Compare parent certificate subject with child issuer field.
 *
 * @param[in]  child   Child certificate which issuer field will be used.
 * @param[in]  parent  Parent certificate which subject field will be used.
 * @param[out] status  #CERTSVC_TRUE if @a child is signed by @a parent,
 *                     else #CERTSVC_FALSE.
 *
 * @return #CERTSVC_SUCCESS on success, otherwise a zero or negative error value
 *
 * @see #CertSvcCertificate
 */
int certsvc_certificate_is_signed_by(CertSvcCertificate child,
									 CertSvcCertificate parent,
									 int *status);

/**
 * @brief Extract data field from certificate.
 *
 * @remarks @a buffer must be freed using certsvc_string_free().
 *
 * @param[in]  certificate  Certificate to get data.
 * @param[in]  field        A certificate field to get.
 * @param[out] buffer       A pointer of CertSvcString.
 *
 * @return #CERTSVC_SUCCESS on success, otherwise a zero or negative error value
 *
 * @see certsvc_string_free()
 * @see #CertSvcCertificate
 * @see #CertSvcCertificateField
 */
int certsvc_certificate_get_string_field(CertSvcCertificate certificate,
										 CertSvcCertificateField field,
										 CertSvcString *buffer);

/**
 * @brief Extract NOT AFTER field from certificate.
 *
 * @param[in]  certificate  Certificate to get data.
 * @param[out] result       A time of NOT AFTER field in @certificate.
 *
 * @return #CERTSVC_SUCCESS on success, otherwise a zero or negative error value
 *
 * @see #CertSvcCertificate
 */
int certsvc_certificate_get_not_after(CertSvcCertificate certificate,
									  time_t *result);

/**
 * @breif Extract NOT BEFORE field from certificate.
 *
 * @param[in]  certificate  Certificate to get data.
 * @param[out] result       A time of NOT BEFORE field in @certificate.
 *
 * @return #CERTSVC_SUCCESS on success, otherwise a zero or negative error value
 */
int certsvc_certificate_get_not_before(CertSvcCertificate certificate,
									   time_t *result);

/**
 * @brief Check whether the certificate is root ca by checking self-signedness.
 *
 * TODO: This fuction should also check ROOTCA field in certificate.
 *
 * @param[in]  certificate  Certificate to check.
 * @param[out] status       #CERTSVC_TRUE if @a certificate is self-signed,
 *                          else #CERTSVC_FALSE.
 *
 * @return #CERTSVC_SUCCESS on success, otherwise a zero or negative error value
 *
 * @see #CertSvcCertificate
 */
int certsvc_certificate_is_root_ca(CertSvcCertificate certificate, int *status);

/**
 * @breif Sort certificates chain.
 *
 * @remarks This fuction modifies certificate_array.
 *
 * @param[in/out] unsortedChain  unsorted chain in form of CertSvcCertificate
 *                               array which will be sorted from end entity
 *                               certificate on the first position and
 *                               (root) CA certificate on the last position.
 * @param[in]     size           A size of @a unsortedChain.
 *
 * @return #CERTSVC_SUCCESS on success, otherwise a zero or negative error value
 *
 * @see #CertSvcCertificate
 */
int certsvc_certificate_chain_sort(CertSvcCertificate *unsortedChain,
								   size_t size);

/**
 * @breif Encode message to base64 format.
 *
 * @remarks @a base64 must be freed using certsvc_string_free().
 *
 * @param[in]  message  CertSvcString to be base64 encode.
 * @param[out] base64   A pointer CertSvcString.
 *
 * @return #CERTSVC_SUCCESS on success, otherwise a zero or negative error value
 *
 * @see certsvc_string_free()
 */
int certsvc_base64_encode(CertSvcString message, CertSvcString *base64);

/**
 * @breif Decode base64 string.
 *
 * @remarks @a base64 must be freed using certsvc_string_free().
 *
 * @param[in]  base64   CertSvcString to be base64 decode.
 * @param[out] message  A pointer CertSvcString.
 *
 * @return #CERTSVC_SUCCESS on success, otherwise a zero or negative error value
 *
 * @see certsvc_string_free()
 */
int certsvc_base64_decode(CertSvcString base64, CertSvcString *message);

/**
 * @brief Verify signature with given arguments.
 *
 * @param[in]  certificate  Certificate to verify.
 * @param[in]  message      A result of @a algorithm with @a signature.
 * @param[in]  signature    A signature to make @message.
 * @param[in]  algorithm    A name of algorithm for hash.
 * @param[out] status       #CERTSVC_SUCCESS if success,
 *                          else #CERTSVC_INVALID_SIGNATURE.
 *
 * @return #CERTSVC_SUCCESS on success, otherwise a zero or negative error value
 *
 */
int certsvc_message_verify(CertSvcCertificate certificate,
						   CertSvcString message,
						   CertSvcString signature,
						   const char *algorithm,
						   int *status);

/**
 * @breif Verify certificate.
 *
 * @remarks Root CA certificate should be stored in @a trusted.
 *
 * @param[in]  certificate    Certificate to verify.
 * @param[in]  trusted        Array with trusted certificates.
 * @param[in]  trustedSize    Array size of @a trusted.
 * @param[in]  untrusted      Array with untrusted certificates.
 * @param[in]  untrustedSize  Array size of @a untrusted.
 * @param[out] status         #CERTSVC_SUCCESS if success, else #CERTSVC_FAIL.
 *
 * @return #CERTSVC_SUCCESS on success, otherwise a zero or negative error value
 */
int certsvc_certificate_verify(CertSvcCertificate certificate,
							   const CertSvcCertificate *trusted,
							   size_t trustedSize,
							   const CertSvcCertificate *untrusted,
							   size_t untrustedSize,
							   int *status);

/**
 * @brief Verify certificate with strict check of CA flag.
 *
 * @remarks Root CA certificate should be stored in @a trusted.
 *
 * @param[in]  certificate    Certificate to verify.
 * @param[in]  trusted        Array with trusted certificates.
 * @param[in]  trustedSize    Array size of @a trusted.
 * @param[in]  untrusted      Array with untrusted certificates.
 * @param[in]  untrustedSize  Array size of @a untrusted.
 * @param[out] status         #CERTSVC_SUCCESS if success, else #CERTSVC_FAIL.
 *
 * @return #CERTSVC_SUCCESS on success, otherwise a zero or negative error value
 */
int certsvc_certificate_verify_with_caflag(CertSvcCertificate certificate,
										   const CertSvcCertificate *trusted,
										   size_t trustedSize,
										   const CertSvcCertificate *untrusted,
										   size_t untrustedSize,
										   int *status);

/**
 * @breif Get visibility from Tizen app signing root certificate.
 *
 * @param[in]  certificate  Tizen app signing root certificate to get visibility.
 * @param[out] visibility   Visibilitay level of @a certificate.
 *
 * @return #CERTSVC_SUCCESS on success, otherwise a zero or negative error value
 *
 * @see #CertSvcVisibility
 */
int certsvc_certificate_get_visibility(CertSvcCertificate certificate,
									   CertSvcVisibility *visibility);


#ifdef __cplusplus
}
#endif

#endif

