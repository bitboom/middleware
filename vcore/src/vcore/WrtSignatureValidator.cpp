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
 * @file        WrtSignatureValidator.cpp
 * @author      Bartlomiej Grzelewski (b.grzelewski@samsung.com)
 * @version     1.0
 * @brief       Implementatin of tizen signature validation protocol.
 */
#include <vcore/WrtSignatureValidator.h>

#include <vcore/CertificateCollection.h>
#include <vcore/Certificate.h>
#include <vcore/OCSPCertMgrUtil.h>
#include <vcore/ReferenceValidator.h>
#include <vcore/ValidatorFactories.h>
#include <vcore/XmlsecAdapter.h>

#include <dpl/log/log.h>

namespace {
const time_t TIMET_DAY = 60 * 60 * 24;

const std::string TOKEN_ROLE_AUTHOR_URI =
    "http://www.w3.org/ns/widgets-digsig#role-author";
const std::string TOKEN_ROLE_DISTRIBUTOR_URI =
    "http://www.w3.org/ns/widgets-digsig#role-distributor";
const std::string TOKEN_PROFILE_URI =
    "http://www.w3.org/ns/widgets-digsig#profile";

} // namespace anonymouse

static tm _ASN1_GetTimeT(ASN1_TIME* time)
{
    struct tm t;
    const char* str = (const char*) time->data;
    size_t i = 0;

    memset(&t, 0, sizeof(t));

    if (time->type == V_ASN1_UTCTIME) /* two digit year */
    {
        t.tm_year = (str[i] - '0') * 10 + (str[i+1] - '0');
        i += 2;
        if (t.tm_year < 70)
            t.tm_year += 100;
    }
    else if (time->type == V_ASN1_GENERALIZEDTIME) /* four digit year */
    {
        t.tm_year =
            (str[i] - '0') * 1000
            + (str[i+1] - '0') * 100
            + (str[i+2] - '0') * 10
            + (str[i+3] - '0');
        i += 4;
        t.tm_year -= 1900;
    }
    t.tm_mon = ((str[i] - '0') * 10 + (str[i+1] - '0')) - 1; // -1 since January is 0 not 1.
    t.tm_mday = (str[i+2] - '0') * 10 + (str[i+3] - '0');
    t.tm_hour = (str[i+4] - '0') * 10 + (str[i+5] - '0');
    t.tm_min  = (str[i+6] - '0') * 10 + (str[i+7] - '0');
    t.tm_sec  = (str[i+8] - '0') * 10 + (str[i+9] - '0');

    /* Note: we did not adjust the time based on time zone information */
    return t;
}


namespace ValidationCore {

class WrtSignatureValidator::Impl {
public:
    virtual WrtSignatureValidator::Result check(
        SignatureData &data,
        const std::string &widgetContentPath) = 0;

    explicit Impl(bool ocspEnable,
                  bool crlEnable,
                  bool complianceMode)
      : m_complianceModeEnabled(complianceMode)
    {
        (void) ocspEnable;
        (void) crlEnable;
    }

    virtual ~Impl() {}

    bool checkRoleURI(const SignatureData &data) {
        std::string roleURI = data.getRoleURI();

        if (roleURI.empty()) {
            LogWarning("URI attribute in Role tag couldn't be empty.");
            return false;
        }

        if (roleURI != TOKEN_ROLE_AUTHOR_URI && data.isAuthorSignature()) {
            LogWarning("URI attribute in Role tag does not "
              "match with signature filename.");
            return false;
        }

        if (roleURI != TOKEN_ROLE_DISTRIBUTOR_URI && !data.isAuthorSignature()) {
            LogWarning("URI attribute in Role tag does not "
              "match with signature filename.");
            return false;
        }
        return true;
    }

    bool checkProfileURI(const SignatureData &data) {
        if (TOKEN_PROFILE_URI != data.getProfileURI()) {
            LogWarning("Profile tag contains unsupported value in URI attribute " << data.getProfileURI());
            return false;
        }
        return true;
    }

    bool checkObjectReferences(const SignatureData &data) {
        ObjectList objectList = data.getObjectList();
        ObjectList::const_iterator iter;
        for (iter = objectList.begin(); iter != objectList.end(); ++iter) {
            if (!data.containObjectReference(*iter)) {
                LogWarning("Signature does not contain reference for object " << *iter);
                return false;
            }
        }
        return true;
    }
protected:
    bool m_complianceModeEnabled;

};

class ImplTizen : public WrtSignatureValidator::Impl {
public:
    WrtSignatureValidator::Result check(SignatureData &data,
            const std::string &widgetContentPath);

    explicit ImplTizen(bool ocspEnable,
                       bool crlEnable,
                       bool complianceMode)
      : Impl(ocspEnable, crlEnable, complianceMode)
    {}

    virtual ~ImplTizen() {}
};

WrtSignatureValidator::Result ImplTizen::check(
        SignatureData &data,
        const std::string &widgetContentPath)
{
    bool disregard = false;

    if (!checkRoleURI(data)) {
        return WrtSignatureValidator::SIGNATURE_INVALID;
    }

    if (!checkProfileURI(data)) {
        return WrtSignatureValidator::SIGNATURE_INVALID;
    }

    //  CertificateList sortedCertificateList = data.getCertList();

    CertificateCollection collection;
    collection.load(data.getCertList());

    // First step - sort certificate
    if (!collection.sort()) {
        LogWarning("Certificates do not form valid chain.");
        return WrtSignatureValidator::SIGNATURE_INVALID_CERT_CHAIN;//SIGNATURE_INVALID;
    }

    // Check for error
    if (collection.empty()) {
        LogWarning("Certificate list in signature is empty.");
        return WrtSignatureValidator::SIGNATURE_INVALID_CERT_CHAIN;//SIGNATURE_INVALID;
    }

    CertificateList sortedCertificateList = collection.getChain();

    // TODO move it to CertificateCollection
    // Add root CA and CA certificates (if chain is incomplete)
    sortedCertificateList =
        OCSPCertMgrUtil::completeCertificateChain(sortedCertificateList);

    CertificatePtr root = sortedCertificateList.back();

    // Is Root CA certificate trusted?
    CertStoreId::Set storeIdSet = createCertificateIdentifier().find(root);

    LogDebug("Is root certificate from TIZEN_DEVELOPER domain : " << storeIdSet.contains(CertStoreId::TIZEN_DEVELOPER));
    LogDebug("Is root certificate from TIZEN_TEST domain      : " << storeIdSet.contains(CertStoreId::TIZEN_TEST));
    LogDebug("Is root certificate from TIZEN_VERIFY domain    : " << storeIdSet.contains(CertStoreId::TIZEN_VERIFY));
    LogDebug("Is root certificate from TIZEN_PUBLIC domain    : " << storeIdSet.contains(CertStoreId::VIS_PUBLIC));
    LogDebug("Is root certificate from TIZEN_PARTNER domain   : " << storeIdSet.contains(CertStoreId::VIS_PARTNER));
    LogDebug("Is root certificate from TIZEN_PLATFORM domain  : " << storeIdSet.contains(CertStoreId::VIS_PLATFORM));
    LogDebug("Visibility level is public   : " << storeIdSet.contains(CertStoreId::VIS_PUBLIC));
    LogDebug("Visibility level is partner  : " << storeIdSet.contains(CertStoreId::VIS_PARTNER));
	LogDebug("Visibility level is platform : " << storeIdSet.contains(CertStoreId::VIS_PLATFORM));

	if (data.isAuthorSignature())
	{
		if (!storeIdSet.contains(CertStoreId::TIZEN_DEVELOPER))
		{
			LogWarning("author-signature.xml has got unrecognized Root CA "
					"certificate. Signature will be disregarded.");
			disregard = true;
		}
	}
	else // distributor
	{
		if (storeIdSet.contains(CertStoreId::TIZEN_DEVELOPER))
		{
			LogWarning("distributor has author level siganture! Signature will be disregarded.");
			return WrtSignatureValidator::SIGNATURE_IN_DISTRIBUTOR_CASE_AUTHOR_CERT;//SIGNATURE_INVALID;
		}
		LogDebug("signaturefile name = " << data.getSignatureFileName());


		if (data.getSignatureNumber() == 1)
		{
			if (storeIdSet.contains(CertStoreId::VIS_PUBLIC) || storeIdSet.contains(CertStoreId::VIS_PARTNER) || storeIdSet.contains(CertStoreId::VIS_PLATFORM))
			{
				LogDebug("Root CA for signature1.xml is correct.");
			}
			else
			{
				LogWarning("signature1.xml has got unrecognized Root CA "
				        "certificate. Signature will be disregarded.");
				disregard = true;
			}
		}
	}

    data.setStorageType(storeIdSet);
    data.setSortedCertificateList(sortedCertificateList);

    // We add only Root CA certificate because WAC ensure that the rest
    // of certificates are present in signature files ;-)
    XmlSec::XmlSecContext context;
    context.signatureFile = data.getSignatureFileName();
    context.certificatePtr = root;

    // Now we should have full certificate chain.
    // If the end certificate is not ROOT CA we should disregard signature
    // but still signature must be valid... Aaaaaa it's so stupid...
    if (!(root->isSignedBy(root))) {
        LogWarning("Root CA certificate not found. Chain is incomplete.");
        //context.allowBrokenChain = true;
    }

    // WAC 2.0 SP-2066 The wrt must not block widget installation
    // due to expiration of the author certificate.
    time_t nowTime = time(NULL);
#define CHECK_TIME
#ifdef CHECK_TIME

    ASN1_TIME* notAfterTime = data.getEndEntityCertificatePtr()->getNotAfterTime();
    ASN1_TIME* notBeforeTime = data.getEndEntityCertificatePtr()->getNotBeforeTime();

	if (X509_cmp_time(notBeforeTime, &nowTime) > 0  || X509_cmp_time(notAfterTime, &nowTime) < 0)
    {
      struct tm *t;
      struct tm ta, tb, tc;
      char msg[1024];

      t = localtime(&nowTime);
      if (!t)
          return WrtSignatureValidator::SIGNATURE_INVALID_CERT_TIME;

      memset(&tc, 0, sizeof(tc));

      snprintf(msg, sizeof(msg), "Year: %d, month: %d, day : %d", t->tm_year + 1900, t->tm_mon + 1,t->tm_mday );
      LogDebug("## System's currentTime : " << msg);
      fprintf(stderr, "## System's currentTime : %s\n", msg);

      tb = _ASN1_GetTimeT(notBeforeTime);
      snprintf(msg, sizeof(msg), "Year: %d, month: %d, day : %d", tb.tm_year + 1900, tb.tm_mon + 1,tb.tm_mday );
      LogDebug("## certificate's notBeforeTime : " << msg);
      fprintf(stderr, "## certificate's notBeforeTime : %s\n", msg);

      ta = _ASN1_GetTimeT(notAfterTime);
      snprintf(msg, sizeof(msg), "Year: %d, month: %d, day : %d", ta.tm_year + 1900, ta.tm_mon + 1,ta.tm_mday );
      LogDebug("## certificate's notAfterTime : " << msg);
      fprintf(stderr, "## certificate's notAfterTime : %s\n", msg);

	  if (storeIdSet.contains(CertStoreId::TIZEN_TEST) || storeIdSet.contains(CertStoreId::TIZEN_VERIFY))
	  {
         LogDebug("## TIZEN_VERIFY : check certificate Time : FALSE");
         fprintf(stderr, "## TIZEN_VERIFY : check certificate Time : FALSE\n");
         return WrtSignatureValidator::SIGNATURE_INVALID_CERT_TIME;//SIGNATURE_INVALID;
      }

      int year = (ta.tm_year - tb.tm_year) / 4;

      if(year == 0)
      {
          tc.tm_year = tb.tm_year; 
          tc.tm_mon = tb.tm_mon + 1;
          tc.tm_mday = tb.tm_mday;

          if(tc.tm_mon == 12)
          {
              tc.tm_year = ta.tm_year;       
              tc.tm_mon = ta.tm_mon - 1;
              tc.tm_mday = ta.tm_mday;
              
              if(tc.tm_mon < 0)
              {
                 tc.tm_year = ta.tm_year;
                 tc.tm_mon = ta.tm_mon;
                 tc.tm_mday = ta.tm_mday -1;

                 if(tc.tm_mday == 0)
                 {
                    tc.tm_year = tb.tm_year;                
                    tc.tm_mon = tb.tm_mon;
                    tc.tm_mday = tb.tm_mday +1;
                 }
              }
          }          
      }
      else{
         tc.tm_year = tb.tm_year + year;
         tc.tm_mon = (tb.tm_mon + ta.tm_mon )/2;
         tc.tm_mday = (tb.tm_mday + ta.tm_mday)/2;  
      }

      snprintf(msg, sizeof(msg), "Year: %d, month: %d, day : %d", tc.tm_year + 1900, tc.tm_mon + 1,tc.tm_mday );
      LogDebug("## cmp cert with validation time : " << msg);
      fprintf(stderr, "## cmp cert with validation time : %s\n", msg);

      time_t outCurrent = mktime(&tc);
      context.validationTime = outCurrent;

      fprintf(stderr, "## cmp outCurrent time : %ld\n", outCurrent);

      //return WrtSignatureValidator::SIGNATURE_INVALID;
    }	

#endif

#if 0
    time_t notAfter = data.getEndEntityCertificatePtr()->getNotAfter();
    time_t notBefore = data.getEndEntityCertificatePtr()->getNotBefore();

	struct tm *t;

	if (data.isAuthorSignature())
	{
		// time_t 2038 year bug exist. So, notAtter() cann't check...
		/*
		if (notAfter < nowTime)
		{
			context.validationTime = notAfter - TIMET_DAY;
			LogWarning("Author certificate is expired. notAfter...");
		}
		*/

		if (notBefore > nowTime)
		{
			LogWarning("Author certificate is expired. notBefore time is greater than system-time.");

			t = localtime(&nowTime);
			LogDebug("System's current Year : " << (t->tm_year + 1900));
			LogDebug("System's current month : " << (t->tm_mon + 1));
			LogDebug("System's current day : " << (t->tm_mday));

			t = localtime(&notBefore);
			LogDebug("Author certificate's notBefore Year : " << (t->tm_year + 1900));
			LogDebug("Author certificate's notBefore month : " << (t->tm_mon + 1));
			LogDebug("Author certificate's notBefore day : " << (t->tm_mday));

			context.validationTime = notBefore + TIMET_DAY;

			t = localtime(&context.validationTime);
			LogDebug("Modified current Year : " << (t->tm_year + 1900));
			LogDebug("Modified current notBefore month : " << (t->tm_mon + 1));
			LogDebug("Modified current notBefore day : " << (t->tm_mday));
		}
	}
#endif
    // WAC 2.0 SP-2066 The wrt must not block widget installation
	//context.allowBrokenChain = true;

	// end
	if (!data.isAuthorSignature())
	{
		if (XmlSec::NO_ERROR != XmlSecSingleton::Instance().validate(&context)) {
			LogWarning("Installation break - invalid package!");
			return WrtSignatureValidator::SIGNATURE_INVALID_HASH_SIGNATURE;//SIGNATURE_INVALID;
		}

		data.setReference(context.referenceSet);

		if (!checkObjectReferences(data)) {
			LogWarning("Failed to check Object References");
			return WrtSignatureValidator::SIGNATURE_INVALID_HASH_SIGNATURE;//SIGNATURE_INVALID;
		}

		ReferenceValidator fileValidator(widgetContentPath);
		if (ReferenceValidator::NO_ERROR != fileValidator.checkReferences(data)) {
			LogWarning("Invalid package - file references broken");
			return WrtSignatureValidator::SIGNATURE_INVALID_NO_HASH_FILE;//SIGNATURE_INVALID;
		}
	}

    if (disregard) {
        LogWarning("Signature is disregard. RootCA is not a member of Tizen");
        return WrtSignatureValidator::SIGNATURE_INVALID_DISTRIBUTOR_CERT;//SIGNATURE_DISREGARD;
    }
    return WrtSignatureValidator::SIGNATURE_VERIFIED;
}

class ImplWac : public WrtSignatureValidator::Impl
{
public:
    WrtSignatureValidator::Result check(SignatureData &data,
            const std::string &widgetContentPath);

    explicit ImplWac(bool ocspEnable,
                     bool crlEnable,
                     bool complianceMode)
      : Impl(ocspEnable, crlEnable, complianceMode)
    {}

    virtual ~ImplWac() {}
};

WrtSignatureValidator::Result ImplWac::check(
    SignatureData &data,
    const std::string &widgetContentPath)
{
    bool disregard = false;

    if (!checkRoleURI(data)) {
        return WrtSignatureValidator::SIGNATURE_INVALID;
    }

    if (!checkProfileURI(data)) {
        return WrtSignatureValidator::SIGNATURE_INVALID;
    }

    //  CertificateList sortedCertificateList = data.getCertList();

    CertificateCollection collection;
    collection.load(data.getCertList());

    // First step - sort certificate
    if (!collection.sort()) {
        LogWarning("Certificates do not form valid chain.");
        return WrtSignatureValidator::SIGNATURE_INVALID;
    }

    // Check for error
    if (collection.empty()) {
        LogWarning("Certificate list in signature is empty.");
        return WrtSignatureValidator::SIGNATURE_INVALID;
    }

    CertificateList sortedCertificateList = collection.getChain();

    // TODO move it to CertificateCollection
    // Add root CA and CA certificates (if chain is incomplete)
    sortedCertificateList =
        OCSPCertMgrUtil::completeCertificateChain(sortedCertificateList);

    CertificatePtr root = sortedCertificateList.back();

    // Is Root CA certificate trusted?
    CertStoreId::Set storeIdSet = createCertificateIdentifier().find(root);

    LogDebug("Is root certificate from TIZEN_DEVELOPER domain : " << storeIdSet.contains(CertStoreId::TIZEN_DEVELOPER));
    LogDebug("Is root certificate from TIZEN_TEST domain      : " << storeIdSet.contains(CertStoreId::TIZEN_TEST));
    LogDebug("Is root certificate from TIZEN_VERIFY domain    : " << storeIdSet.contains(CertStoreId::TIZEN_VERIFY));
    LogDebug("Is root certificate from TIZEN_PUBLIC domain    : " << storeIdSet.contains(CertStoreId::VIS_PUBLIC));
    LogDebug("Is root certificate from TIZEN_PARTNER domain   : " << storeIdSet.contains(CertStoreId::VIS_PARTNER));
    LogDebug("Is root certificate from TIZEN_PLATFORM domain  : " << storeIdSet.contains(CertStoreId::VIS_PLATFORM));
    LogDebug("Visibility level is public   : " << storeIdSet.contains(CertStoreId::VIS_PUBLIC));
    LogDebug("Visibility level is partner  : " << storeIdSet.contains(CertStoreId::VIS_PARTNER));
	LogDebug("Visibility level is platform : " << storeIdSet.contains(CertStoreId::VIS_PLATFORM));

	if (data.isAuthorSignature())
	{
		if (!storeIdSet.contains(CertStoreId::TIZEN_DEVELOPER))
		{
			LogWarning("author-signature.xml has got unrecognized Root CA "
					"certificate. Signature will be disregarded.");
			disregard = true;
		}
	}
	else
	{
		if (storeIdSet.contains(CertStoreId::TIZEN_DEVELOPER))
		{
			LogWarning("distributor has author level siganture! Signature will be disregarded.");
			return WrtSignatureValidator::SIGNATURE_INVALID;
		}
		LogDebug("signaturefile name = " << data.getSignatureFileName());

		if (data.getSignatureNumber() == 1)
		{
			if (storeIdSet.contains(CertStoreId::VIS_PUBLIC) || storeIdSet.contains(CertStoreId::VIS_PARTNER) || storeIdSet.contains(CertStoreId::VIS_PLATFORM))
			{
				LogDebug("Root CA for signature1.xml is correct.");
			}
			else
			{
				LogWarning("signature1.xml has got unrecognized Root CA "
				        "certificate. Signature will be disregarded.");
				disregard = true;
			}
		}
	}

    data.setStorageType(storeIdSet);
    data.setSortedCertificateList(sortedCertificateList);

    // We add only Root CA certificate because WAC ensure that the rest
    // of certificates are present in signature files ;-)
    XmlSec::XmlSecContext context;
    context.signatureFile = data.getSignatureFileName();
    context.certificatePtr = root;

    // Now we should have full certificate chain.
    // If the end certificate is not ROOT CA we should disregard signature
    // but still signature must be valid... Aaaaaa it's so stupid...
    if (!(root->isSignedBy(root))) {
        LogWarning("Root CA certificate not found. Chain is incomplete.");
//        context.allowBrokenChain = true;
    }

    time_t nowTime = time(NULL);
    // WAC 2.0 SP-2066 The wrt must not block widget installation
	// due to expiration of the author certificate.
#define CHECK_TIME
#ifdef CHECK_TIME

    ASN1_TIME* notAfterTime = data.getEndEntityCertificatePtr()->getNotAfterTime();
    ASN1_TIME* notBeforeTime = data.getEndEntityCertificatePtr()->getNotBeforeTime();

		if (X509_cmp_time(notBeforeTime, &nowTime) > 0  || X509_cmp_time(notAfterTime, &nowTime) < 0)
    {
      struct tm *t;
      struct tm ta, tb, tc;
      char msg[1024];

      t = localtime(&nowTime);
      if (!t)
          return WrtSignatureValidator::SIGNATURE_INVALID_CERT_TIME;

      memset(&tc, 0, sizeof(tc));

      snprintf(msg, sizeof(msg), "Year: %d, month: %d, day : %d", t->tm_year + 1900, t->tm_mon + 1,t->tm_mday );
      LogDebug("## System's currentTime : " << msg);
      fprintf(stderr, "## System's currentTime : %s\n", msg);

      tb = _ASN1_GetTimeT(notBeforeTime);
      snprintf(msg, sizeof(msg), "Year: %d, month: %d, day : %d", tb.tm_year + 1900, tb.tm_mon + 1,tb.tm_mday );
      LogDebug("## certificate's notBeforeTime : " << msg);
      fprintf(stderr, "## certificate's notBeforeTime : %s\n", msg);

      ta = _ASN1_GetTimeT(notAfterTime);
      snprintf(msg, sizeof(msg), "Year: %d, month: %d, day : %d", ta.tm_year + 1900, ta.tm_mon + 1,ta.tm_mday );
      LogDebug("## certificate's notAfterTime : " << msg);
      fprintf(stderr, "## certificate's notAfterTime : %s\n", msg);

      if (storeIdSet.contains(CertStoreId::TIZEN_VERIFY))
      {
         LogDebug("## TIZEN_VERIFY : check certificate Time : FALSE");
         fprintf(stderr, "## TIZEN_VERIFY : check certificate Time : FALSE\n");
         return WrtSignatureValidator::SIGNATURE_INVALID;
      }

      int year = (ta.tm_year - tb.tm_year) / 4;

      if(year == 0)
      {
          tc.tm_year = tb.tm_year; 
          tc.tm_mon = tb.tm_mon + 1;
          tc.tm_mday = tb.tm_mday;

          if(tc.tm_mon == 12)
          {
              tc.tm_year = ta.tm_year;       
              tc.tm_mon = ta.tm_mon - 1;
              tc.tm_mday = ta.tm_mday;
              
              if(tc.tm_mon < 0)
              {
                 tc.tm_year = ta.tm_year;
                 tc.tm_mon = ta.tm_mon;
                 tc.tm_mday = ta.tm_mday -1;

                 if(tc.tm_mday == 0)
                 {
                    tc.tm_year = tb.tm_year;                
                    tc.tm_mon = tb.tm_mon;
                    tc.tm_mday = tb.tm_mday +1;
                 }
              }
          }          
      }
      else{
         tc.tm_year = tb.tm_year + year;
         tc.tm_mon = (tb.tm_mon + ta.tm_mon )/2;
         tc.tm_mday = (tb.tm_mday + ta.tm_mday)/2;  
      }

      snprintf(msg, sizeof(msg), "Year: %d, month: %d, day : %d", tc.tm_year + 1900, tc.tm_mon + 1,tc.tm_mday );
      LogDebug("## cmp cert with validation time : " << msg);
      fprintf(stderr, "## cmp cert with validation time : %s\n", msg);

      time_t outCurrent = mktime(&tc);

      fprintf(stderr, "## cmp outCurrent time : %ld\n", outCurrent);

      context.validationTime = outCurrent;
      //return WrtSignatureValidator::SIGNATURE_INVALID;
    }	

#endif

#if 0
	time_t notAfter = data.getEndEntityCertificatePtr()->getNotAfter();
	time_t notBefore = data.getEndEntityCertificatePtr()->getNotBefore();

	struct tm *t;

	if (data.isAuthorSignature())
	{
		// time_t 2038 year bug exist. So, notAtter() cann't check...
		/*
		if (notAfter < nowTime)
		{
			context.validationTime = notAfter - TIMET_DAY;
			LogWarning("Author certificate is expired. notAfter...");
		 }
		 */

		if (notBefore > nowTime)
		{
			LogWarning("Author certificate is expired. notBefore time is greater than system-time.");

			t = localtime(&nowTime);
			LogDebug("System's current Year : " << (t->tm_year + 1900));
			LogDebug("System's current month : " << (t->tm_mon + 1));
			LogDebug("System's current day : " << (t->tm_mday));

			t = localtime(&notBefore);
			LogDebug("Author certificate's notBefore Year : " << (t->tm_year + 1900));
			LogDebug("Author certificate's notBefore month : " << (t->tm_mon + 1));
			LogDebug("Author certificate's notBefore day : " << (t->tm_mday));

			context.validationTime = notBefore + TIMET_DAY;

			t = localtime(&context.validationTime);
			LogDebug("Modified current Year : " << (t->tm_year + 1900));
			LogDebug("Modified current notBefore month : " << (t->tm_mon + 1));
			LogDebug("Modified current notBefore day : " << (t->tm_mday));
		}
	}
#endif

	if (!data.isAuthorSignature())
	{
		if (XmlSec::NO_ERROR != XmlSecSingleton::Instance().validate(&context)) {
			LogWarning("Installation break - invalid package!");
			return WrtSignatureValidator::SIGNATURE_INVALID;
		}

		data.setReference(context.referenceSet);

		if (!checkObjectReferences(data)) {
			return WrtSignatureValidator::SIGNATURE_INVALID;
		}

		ReferenceValidator fileValidator(widgetContentPath);
		if (ReferenceValidator::NO_ERROR != fileValidator.checkReferences(data)) {
			LogWarning("Invalid package - file references broken");
			return WrtSignatureValidator::SIGNATURE_INVALID;
		}
	}

    if (disregard) {
        LogWarning("Signature is disregard. RootCA is not a member of Tizen.");
        return WrtSignatureValidator::SIGNATURE_DISREGARD;
    }
    return WrtSignatureValidator::SIGNATURE_VERIFIED;
}

// Implementation of WrtSignatureValidator

WrtSignatureValidator::WrtSignatureValidator(
    AppType appType,
    bool ocspEnable,
    bool crlEnable,
    bool complianceMode)
  : m_impl(0)
{
    if (appType == TIZEN)
        m_impl = new ImplTizen(ocspEnable,crlEnable,complianceMode);
    else
        m_impl = new ImplWac(ocspEnable,crlEnable,complianceMode);
}

WrtSignatureValidator::~WrtSignatureValidator()
{
    delete m_impl;
}

WrtSignatureValidator::Result WrtSignatureValidator::check(
    SignatureData &data,
    const std::string &widgetContentPath)
{
    return m_impl->check(data, widgetContentPath);
}

} // namespace ValidationCore

