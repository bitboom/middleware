/*
 * Copyright (c) 2016 Samsung Electronics Co., Ltd All Rights Reserved
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
 * @file        common-res.cpp
 * @author      Kyungwook Tak (k.tak@samsung.com)
 * @version     1.0
 * @brief       This file is the common resource for certsvc capi test
 */

#include "common-res.h"

const std::string TestData::SelfSignedCAPath = std::string(TESTAPP_RES_DIR) +
		"certs/root_cacert0.pem";
const std::string TestData::subjectGoogleCA =
	"/C=US/O=VeriSign, Inc./OU=Class 3 Public Primary Certification Authority";

const std::string TestData::googleCA =
	"MIICPDCCAaUCEHC65B0Q2Sk0tjjKewPMur8wDQYJKoZIhvcNAQECBQAwXzELMAkG"
	"A1UEBhMCVVMxFzAVBgNVBAoTDlZlcmlTaWduLCBJbmMuMTcwNQYDVQQLEy5DbGFz"
	"cyAzIFB1YmxpYyBQcmltYXJ5IENlcnRpZmljYXRpb24gQXV0aG9yaXR5MB4XDTk2"
	"MDEyOTAwMDAwMFoXDTI4MDgwMTIzNTk1OVowXzELMAkGA1UEBhMCVVMxFzAVBgNV"
	"BAoTDlZlcmlTaWduLCBJbmMuMTcwNQYDVQQLEy5DbGFzcyAzIFB1YmxpYyBQcmlt"
	"YXJ5IENlcnRpZmljYXRpb24gQXV0aG9yaXR5MIGfMA0GCSqGSIb3DQEBAQUAA4GN"
	"ADCBiQKBgQDJXFme8huKARS0EN8EQNvjV69qRUCPhAwL0TPZ2RHP7gJYHyX3KqhE"
	"BarsAx94f56TuZoAqiN91qyFomNFx3InzPRMxnVx0jnvT0Lwdd8KkMaOIG+YD/is"
	"I19wKTakyYbnsZogy1Olhec9vn2a/iRFM9x2Fe0PonFkTGUugWhFpwIDAQABMA0G"
	"CSqGSIb3DQEBAgUAA4GBALtMEivPLCYATxQT3ab7/AoRhIzzKBxnki98tsX63/Do"
	"lbwdj2wsqFHMc9ikwFPwTtYmwHYBV4GSXiHx0bH/59AhWM1pF+NEHJwZRDmJXNyc"
	"AA9WjQKZ7aKQRUzkuxCkPfAyAw7xzvjoyVGM5mKf5p/AfbdynMk2OmufTqj/ZA1k";

const std::string TestData::google2nd =
	"MIIDIzCCAoygAwIBAgIEMAAAAjANBgkqhkiG9w0BAQUFADBfMQswCQYDVQQGEwJV"
	"UzEXMBUGA1UEChMOVmVyaVNpZ24sIEluYy4xNzA1BgNVBAsTLkNsYXNzIDMgUHVi"
	"bGljIFByaW1hcnkgQ2VydGlmaWNhdGlvbiBBdXRob3JpdHkwHhcNMDQwNTEzMDAw"
	"MDAwWhcNMTQwNTEyMjM1OTU5WjBMMQswCQYDVQQGEwJaQTElMCMGA1UEChMcVGhh"
	"d3RlIENvbnN1bHRpbmcgKFB0eSkgTHRkLjEWMBQGA1UEAxMNVGhhd3RlIFNHQyBD"
	"QTCBnzANBgkqhkiG9w0BAQEFAAOBjQAwgYkCgYEA1NNn0I0Vf67NMf59HZGhPwtx"
	"PKzMyGT7Y/wySweUvW+Aui/hBJPAM/wJMyPpC3QrccQDxtLN4i/1CWPN/0ilAL/g"
	"5/OIty0y3pg25gqtAHvEZEo7hHUD8nCSfQ5i9SGraTaEMXWQ+L/HbIgbBpV8yeWo"
	"3nWhLHpo39XKHIdYYBkCAwEAAaOB/jCB+zASBgNVHRMBAf8ECDAGAQH/AgEAMAsG"
	"A1UdDwQEAwIBBjARBglghkgBhvhCAQEEBAMCAQYwKAYDVR0RBCEwH6QdMBsxGTAX"
	"BgNVBAMTEFByaXZhdGVMYWJlbDMtMTUwMQYDVR0fBCowKDAmoCSgIoYgaHR0cDov"
	"L2NybC52ZXJpc2lnbi5jb20vcGNhMy5jcmwwMgYIKwYBBQUHAQEEJjAkMCIGCCsG"
	"AQUFBzABhhZodHRwOi8vb2NzcC50aGF3dGUuY29tMDQGA1UdJQQtMCsGCCsGAQUF"
	"BwMBBggrBgEFBQcDAgYJYIZIAYb4QgQBBgpghkgBhvhFAQgBMA0GCSqGSIb3DQEB"
	"BQUAA4GBAFWsY+reod3SkF+fC852vhNRj5PZBSvIG3dLrWlQoe7e3P3bB+noOZTc"
	"q3J5Lwa/q4FwxKjt6lM07e8eU9kGx1Yr0Vz00YqOtCuxN5BICEIlxT6Ky3/rbwTR"
	"bcV0oveifHtgPHfNDs5IAn8BL7abN+AqKjbc1YXWrOU/VG+WHgWv";

const std::string TestData::certEE =
	"MIIDIjCCAougAwIBAgIQK59+5colpiUUIEeCdTqbuTANBgkqhkiG9w0BAQUFADBM"
	"MQswCQYDVQQGEwJaQTElMCMGA1UEChMcVGhhd3RlIENvbnN1bHRpbmcgKFB0eSkg"
	"THRkLjEWMBQGA1UEAxMNVGhhd3RlIFNHQyBDQTAeFw0xMTEwMjYwMDAwMDBaFw0x"
	"MzA5MzAyMzU5NTlaMGkxCzAJBgNVBAYTAlVTMRMwEQYDVQQIEwpDYWxpZm9ybmlh"
	"MRYwFAYDVQQHFA1Nb3VudGFpbiBWaWV3MRMwEQYDVQQKFApHb29nbGUgSW5jMRgw"
	"FgYDVQQDFA9tYWlsLmdvb2dsZS5jb20wgZ8wDQYJKoZIhvcNAQEBBQADgY0AMIGJ"
	"AoGBAK85FZho5JL+T0/xu/8NLrD+Jaq9aARnJ+psQ0ynbcvIj36B7ocmJRASVDOe"
	"qj2bj46Ss0sB4/lKKcMP/ay300yXKT9pVc9wgwSvLgRudNYPFwn+niAkJOPHaJys"
	"Eb2S5LIbCfICMrtVGy0WXzASI+JMSo3C2j/huL/3OrGGvvDFAgMBAAGjgecwgeQw"
	"DAYDVR0TAQH/BAIwADA2BgNVHR8ELzAtMCugKaAnhiVodHRwOi8vY3JsLnRoYXd0"
	"ZS5jb20vVGhhd3RlU0dDQ0EuY3JsMCgGA1UdJQQhMB8GCCsGAQUFBwMBBggrBgEF"
	"BQcDAgYJYIZIAYb4QgQBMHIGCCsGAQUFBwEBBGYwZDAiBggrBgEFBQcwAYYWaHR0"
	"cDovL29jc3AudGhhd3RlLmNvbTA+BggrBgEFBQcwAoYyaHR0cDovL3d3dy50aGF3"
	"dGUuY29tL3JlcG9zaXRvcnkvVGhhd3RlX1NHQ19DQS5jcnQwDQYJKoZIhvcNAQEF"
	"BQADgYEANYARzVI+hCn7wSjhIOUCj19xZVgdYnJXPOZeJWHTy60i+NiBpOf0rnzZ"
	"wW2qkw1iB5/yZ0eZNDNPPQJ09IHWOAgh6OKh+gVBnJzJ+fPIo+4NpddQVF4vfXm3"
	"fgp8tuIsqK7+lNfNFjBxBKqeecPStiSnJavwSI4vw6e7UN0Pz7A=";

const std::string TestData::certFullField =
	"MIICvzCCAigCCQCpduCB5TcrmDANBgkqhkiG9w0BAQsFADCBlzELMAkGA1UEBhMC"
	"S08xEzARBgNVBAgMCkt5ZW9uZ2tpZG8xDjAMBgNVBAcMBVN1d29uMRAwDgYDVQQK"
	"DAdTYW1zdW5nMRcwFQYDVQQLDA5Tb2Z0d2FyZUNlbnRlcjEWMBQGA1UEAwwNVGl6"
	"ZW5TZWN1cml0eTEgMB4GCSqGSIb3DQEJARYRay50YWtAc2Ftc3VuZy5jb20wHhcN"
	"MTUwODE5MDQzNTQxWhcNMjAwODE3MDQzNTQxWjCBrzELMAkGA1UEBhMCUE8xEzAR"
	"BgNVBAgMClNlb3VsU3RhdGUxDjAMBgNVBAcMBVNlb3VsMRYwFAYDVQQKDA1TYW1z"
	"dW5nU2Vjb25kMR0wGwYDVQQLDBRTb2Z0d2FyZUNlbnRlclNlY29uZDEcMBoGA1UE"
	"AwwTVGl6ZW5TZWN1cml0eVNlY29uZDEmMCQGCSqGSIb3DQEJARYXa3l1bmd3b29r"
	"LnRha0BnbWFpbC5jb20wgZ8wDQYJKoZIhvcNAQEBBQADgY0AMIGJAoGBANgIo6MF"
	"++LfNs3jSC87WRfO4zK/n+/xfPsn+Xwyi4jtsMxk2v/ye/SGESAACdCFFBL/EZ9j"
	"Adu/6kzuKDJ5SpphW++XoUM2YdNxHzf6+z8JK9IPVmhy3b/hQlVbtBiFAMuLOn1D"
	"C0gfTEnYRgZBs3v5Z/N35ZO1FoCz8y9wHmAXAgMBAAEwDQYJKoZIhvcNAQELBQAD"
	"gYEAb3/k6KLLGysGEMFk2rVVvroqGqzeKHfk+t/pbrdGHEr0EIIXIYBk7YCGhMVF"
	"Flw7g6Cpxyo1FQEFQhcdiv4/Iio2/RemtTYRILOnoaHSh9j9Fl1LT69MZq20hjId"
	"8WNW2Ah9APv29fpZQFXo8dMsy1cnh4hJhWVWWttSLUBd1uE=";

std::string magdaCert =
	"MIIEDzCCA3igAwIBAgIJAMdKgvadG/Z/MA0GCSqGSIb3DQEBBQUAMHIxCzAJBgNV"
	"BAYTAlBMMQwwCgYDVQQIEwNNYXoxEDAOBgNVBAoTB1NhbXN1bmcxDTALBgNVBAsT"
	"BFNQUkMxEDAOBgNVBAMTB1NhbXN1bmcxIjAgBgkqhkiG9w0BCQEWE3NhbXN1bmdA"
	"c2Ftc3VuZy5jb20wHhcNMTExMDA1MTIxMTMzWhcNMjExMDAyMTIxMTMzWjCBijEL"
	"MAkGA1UEBhMCUEwxFDASBgNVBAgTC01hem93aWVja2llMRIwEAYDVQQHEwlsZWdp"
	"b25vd28xEDAOBgNVBAoTB3NhbXN1bmcxDTALBgNVBAsTBHNwcmMxDjAMBgNVBAMT"
	"BW1hZ2RhMSAwHgYJKoZIhvcNAQkBFhFtYWdkYUBzYW1zdW5nLmNvbTCCAbcwggEr"
	"BgcqhkjOOAQBMIIBHgKBgQC1PCOasFhlfMc1yjdcp7zkzXGiW+MpVuFlsdYwkAa9"
	"sIvNrQLi2ulxcnNBeCHKDbk7U+J3/QwO2XanapQMUqvfjfjL1QQ5Vf7ENUWPNP7c"
	"Evx82Nb5jWdHyRfV//TciBZN8GLNEbfhtWlhI6CbDW1AaY0nPZ879rSIk7/aNKZ3"
	"FQIVALcr8uQAmnV+3DLIA5nTo0Bg0bjLAoGAJG7meUtQbMulRMdjzeCoya2FXdm+"
	"4acvInE9/+MybXTB3bFANMyw6WTvk4K9RK8tm52N95cykTjpAbxqTMaXwkdWbOFd"
	"VKAKnyxi/UKtY9Q6NmwJB2hbA1GUzhPko8rEda66CGl0VbyM1lKMJjA+wp9pG110"
	"L0ov19Q9fvqKp5UDgYUAAoGBAKxAQg7MqCgkC0MJftYjNaKM5n1iZv4j1li49zKf"
	"Y5nTLP+vYAvg0owLNYvJ5ncKfY1DACPU4/+tC7TTua95wgj5rwvAXnzgSyOGuSr0"
	"fK9DyrH6E0LfXT+WuIQHahm2iSbxqPrChlnp5/EXDTBaO6Qfdpq0BP48ClZebxcA"
	"+TYFo3sweTAJBgNVHRMEAjAAMCwGCWCGSAGG+EIBDQQfFh1PcGVuU1NMIEdlbmVy"
	"YXRlZCBDZXJ0aWZpY2F0ZTAdBgNVHQ4EFgQUmSpShswvWtEABd+l3WxccRcCydUw"
	"HwYDVR0jBBgwFoAUggh/2wAChuhTKqX6WK5nfxQ4yGAwDQYJKoZIhvcNAQEFBQAD"
	"gYEAgfnAu/gMJRC/BFwkgvrHL0TV4ffPVAf7RSnZS6ib4IHGgrvXJvL+Qh7vHykv"
	"ZIqD2L96nY2EaSNr0yXrT81YROndOQUJNx4Y/W8m6asu4hzANNZqWCbApPDIMK6V"
	"cPA1wrKgZqbWp218WBqI2v9pXV0O+jpzxq1+GeQV2UsbRwc=";

std::string magdaMes = "c2lnbmVkIGRhdGEK";

std::string magdaSig = "MC0CFQCL2pDA4S/zsHkDUCWOq7K6ebG14gIUHHoLsbeUd+BEqBXB6XjmcTncBRA=";

const TestData::SigComponents TestData::magda(magdaCert, magdaMes, magdaSig);

std::string filipCert =
	"MIIC4zCCAkygAwIBAgIJAMdKgvadG/Z+MA0GCSqGSIb3DQEBBQUAMHIxCzAJBgNV"
	"BAYTAlBMMQwwCgYDVQQIEwNNYXoxEDAOBgNVBAoTB1NhbXN1bmcxDTALBgNVBAsT"
	"BFNQUkMxEDAOBgNVBAMTB1NhbXN1bmcxIjAgBgkqhkiG9w0BCQEWE3NhbXN1bmdA"
	"c2Ftc3VuZy5jb20wHhcNMTExMDA1MTIwMDUxWhcNMjExMDAyMTIwMDUxWjB4MQsw"
	"CQYDVQQGEwJQTDEMMAoGA1UECBMDTUFaMQwwCgYDVQQHEwNMZWcxDDAKBgNVBAoT"
	"A1NhbTENMAsGA1UECxMEU1BSQzEOMAwGA1UEAxMFRmlsaXAxIDAeBgkqhkiG9w0B"
	"CQEWEWZpbGlwQHNhbXN1bmcuY29tMIGfMA0GCSqGSIb3DQEBAQUAA4GNADCBiQKB"
	"gQDS/sS0wXSCb34ojN8bWFd4Pl9eTLHh18UNGsPpLpp4itdfuc/OgyqaSoDwBzVh"
	"EWAVLCTxexUa4Ncva+41NbkW4RCsFzeGs0ktpu1+8Q+v0QEOGqVF2rQkgilzDF/o"
	"O56Fxw9vG1OA+qdQd3yOAV2EqLNBPrEYB9K5GFyffrakSQIDAQABo3sweTAJBgNV"
	"HRMEAjAAMCwGCWCGSAGG+EIBDQQfFh1PcGVuU1NMIEdlbmVyYXRlZCBDZXJ0aWZp"
	"Y2F0ZTAdBgNVHQ4EFgQUeyy3iV75KtOkpPFd6mnR9dFGZMwwHwYDVR0jBBgwFoAU"
	"ggh/2wAChuhTKqX6WK5nfxQ4yGAwDQYJKoZIhvcNAQEFBQADgYEADtv0CBrQ1QCM"
	"H9jKFjpSpq7zFKMXQeVtb/Zie823//woicg8kxnP5sS4dJWNXNb1iMLdhgV80g1y"
	"t3gTWPxTtFzprQyNiJHTmrbNWXLX1roRVGUE/I8Q4xexqpbNlJIW2Jjm/kqoKfnK"
	"xORG6HNPXZV29NY2fDRPPOIYoFQzrXI=";

std::string filipMes = "Q3plZ28gdHUgc3p1a2Fzej8K";

std::string filipSigSHA1 =
	"xEIpVjEIUoDkYGtX2ih6Gbya0/gr7OMdvbBKmjqzfNh9GHqwrgjglByeC5sspUzPBUF4Vmg/hZqL"
	"gSsxXw9bKEa8c6mTQoNX51IC0ELPsoUMIJF1gGdFu0SzKptvU0+ksiiOM+70+s5t8s3z0G5PeA7O"
	"99oq8UlrX7GDlxaoTU4=";

std::string filipSigSHA256 =
	"a5nGT6wnbQ8MLwLkG965E4e1Rv983E+v3nolLvvjuAKnfgWYb+70Da+T9ggYDTjngq+EBgC30w1p"
	"EScrwye8ELefvRxDWy1+tWR4QRW/Nd4oN2U/pvozoabDSpe9Cvt0ECEOWKDqIYYnoWFjOiXg9VwD"
	"HVVkQXvsSYu6thX/Xsk=";

const TestData::SigComponents TestData::filipSHA1(filipCert, filipMes, filipSigSHA1);
const TestData::SigComponents TestData::filipSHA256(filipCert, filipMes, filipSigSHA256);

std::string certChainArr[3] = {
	"MIIDXTCCAsagAwIBAgIBAjANBgkqhkiG9w0BAQUFADB6MQswCQYDVQQGEwJLUjEO"
	"MAwGA1UECAwFU2VvdWwxEDAOBgNVBAoMB1NhbXN1bmcxEzARBgNVBAsMClRpemVu"
	"IFRlc3QxFzAVBgNVBAMMDlRlc3QgU2Vjb25kIENBMRswGQYJKoZIhvcNAQkBFgx0"
	"dEBnbWFpbC5jb20wHhcNMTQwNjE4MDgxMTA0WhcNMTUwNjE4MDgxMTA0WjB7MQsw"
	"CQYDVQQGEwJLUjEOMAwGA1UECAwFU2VvdWwxEDAOBgNVBAoMB1NhbXN1bmcxFzAV"
	"BgNVBAsMDlRpemVuIFRlc3QgQUlBMRQwEgYDVQQDDAtUZXN0IFNpZ25lcjEbMBkG"
	"CSqGSIb3DQEJARYMdHRAZ21haWwuY29tMIGfMA0GCSqGSIb3DQEBAQUAA4GNADCB"
	"iQKBgQCwgKw+/71jWXnx4bLLZrTPmE+NrDfHSfZx8yTGYeewMzP6ZlXM8WduxNiq"
	"pqm7G2XN182GEXsdoxwa09HtMVGqSGA/BCamD1Z6liHOEb4UTB3ROJ1lZDDkyJ9a"
	"gZOfoZst/Aj8+bwV3x3ie+p4a2w/8eSsalrfef2gX6khaSsJOwIDAQABo4HxMIHu"
	"MAkGA1UdEwQCMAAwLAYJYIZIAYb4QgENBB8WHU9wZW5TU0wgR2VuZXJhdGVkIENl"
	"cnRpZmljYXRlMB0GA1UdDgQWBBRL0nKiNUjzh1/LPvZoqLvnVfOZqjAfBgNVHSME"
	"GDAWgBSpSfNbE0V2NHn/V5f660v2cWwYgDBzBggrBgEFBQcBAQRnMGUwIQYIKwYB"
	"BQUHMAGGFWh0dHA6Ly8xMjcuMC4wLjE6ODg4ODBABggrBgEFBQcwAoY0aHR0cDov"
	"L1NWUlNlY3VyZS1HMy1haWEudmVyaXNpZ24uY29tL1NWUlNlY3VyZUczLmNlcjAN"
	"BgkqhkiG9w0BAQUFAAOBgQABP+yru9/2auZ4ekjV03WRg5Vq/rqmOHDruMNVbZ4H"
	"4PBLRLSpC//OGahgEgUKe89BcB10lUi55D5YME3Do89I+hFugv0BPGaA201iLOhL"
	"/0u0aVm1yJxNt1YjW2fMKqnCHgjoHzh0wQC1pIb5vxJrYCn3Pbhml7W6JPDDJHfm"
	"XQ==",

	"MIIDLzCCApigAwIBAgIBATANBgkqhkiG9w0BAQUFADB4MQswCQYDVQQGEwJLUjEO"
	"MAwGA1UECAwFU2VvdWwxEDAOBgNVBAoMB1NhbXN1bmcxEzARBgNVBAsMClRpemVu"
	"IFRlc3QxFTATBgNVBAMMDFRlc3QgUm9vdCBDQTEbMBkGCSqGSIb3DQEJARYMdHRA"
	"Z21haWwuY29tMB4XDTE0MDYxODA4MTA1OVoXDTE1MDYxODA4MTA1OVowejELMAkG"
	"A1UEBhMCS1IxDjAMBgNVBAgMBVNlb3VsMRAwDgYDVQQKDAdTYW1zdW5nMRMwEQYD"
	"VQQLDApUaXplbiBUZXN0MRcwFQYDVQQDDA5UZXN0IFNlY29uZCBDQTEbMBkGCSqG"
	"SIb3DQEJARYMdHRAZ21haWwuY29tMIGfMA0GCSqGSIb3DQEBAQUAA4GNADCBiQKB"
	"gQDLJrMAF/JzxIIrQzQ/3FGt7cGAUEYaEFSo+hcDKYRXaZC33/kkVANYFh+log9e"
	"MJUUlt0TBOg79tOnS/5MBwWaVLEOLalv0Uj2FfjEMpGd/xEF6Vv34mSTcWadMHyD"
	"wYwDZVwdFkrvOkA6WwgwS8XSrpbH/nkKUkKpk+YYljKEzQIDAQABo4HGMIHDMB0G"
	"A1UdDgQWBBSpSfNbE0V2NHn/V5f660v2cWwYgDAfBgNVHSMEGDAWgBRkHk9Lnhgv"
	"vOIwxHOma54FGt8SCDAMBgNVHRMEBTADAQH/MHMGCCsGAQUFBwEBBGcwZTAhBggr"
	"BgEFBQcwAYYVaHR0cDovLzEyNy4wLjAuMTo4ODg4MEAGCCsGAQUFBzAChjRodHRw"
	"Oi8vU1ZSU2VjdXJlLUczLWFpYS52ZXJpc2lnbi5jb20vU1ZSU2VjdXJlRzMuY2Vy"
	"MA0GCSqGSIb3DQEBBQUAA4GBAFonDQzs/Ts1sEDW3f5EmuKVZlpH9sLstSLJxZK8"
	"+v88Jbz451/Lf8hxvnMv3MwExXr9qPKPlvKRfj+bbLB5KTEcZ5zhDpJ7SDYesdUd"
	"RKOMSN0JIRL3JOCdYHOnJk6o+45vZ/TNv0lsiK90vxH2jo2EXnNG+jeyBGwp+3H6"
	"RWHw",

	"MIIDLTCCApagAwIBAgIBADANBgkqhkiG9w0BAQUFADB4MQswCQYDVQQGEwJLUjEO"
	"MAwGA1UECAwFU2VvdWwxEDAOBgNVBAoMB1NhbXN1bmcxEzARBgNVBAsMClRpemVu"
	"IFRlc3QxFTATBgNVBAMMDFRlc3QgUm9vdCBDQTEbMBkGCSqGSIb3DQEJARYMdHRA"
	"Z21haWwuY29tMB4XDTE0MDYxODA4MTA1MVoXDTE1MDYxODA4MTA1MVoweDELMAkG"
	"A1UEBhMCS1IxDjAMBgNVBAgMBVNlb3VsMRAwDgYDVQQKDAdTYW1zdW5nMRMwEQYD"
	"VQQLDApUaXplbiBUZXN0MRUwEwYDVQQDDAxUZXN0IFJvb3QgQ0ExGzAZBgkqhkiG"
	"9w0BCQEWDHR0QGdtYWlsLmNvbTCBnzANBgkqhkiG9w0BAQEFAAOBjQAwgYkCgYEA"
	"o6ZegsQ9hScM1yD7ejv44xUTJDjTlcGweHh76Im22x6yAljM2+dKdj3EIVGt0BA3"
	"6qdZFl8WOxzQGcAzQY7GFOXQVog4UjqHMxmWwAx5jQyBzIieAj4HZ2lquPBiyiIe"
	"HAo6sCSWsxnh7PqvWaAypPZVEqOJ3ga5rXyDCcjzQ8ECAwEAAaOBxjCBwzAdBgNV"
	"HQ4EFgQUZB5PS54YL7ziMMRzpmueBRrfEggwHwYDVR0jBBgwFoAUZB5PS54YL7zi"
	"MMRzpmueBRrfEggwDAYDVR0TBAUwAwEB/zBzBggrBgEFBQcBAQRnMGUwIQYIKwYB"
	"BQUHMAGGFWh0dHA6Ly8xMjcuMC4wLjE6ODg4ODBABggrBgEFBQcwAoY0aHR0cDov"
	"L1NWUlNlY3VyZS1HMy1haWEudmVyaXNpZ24uY29tL1NWUlNlY3VyZUczLmNlcjAN"
	"BgkqhkiG9w0BAQUFAAOBgQAyRJXTZcwRCkRNGZQCO8txHvrmgv8vQwnZZF6SwyY/"
	"Bry0fmlehtN52NLjjPEG6u9YFYfzSkjQlVR0qfQ2mNs3d6AKFlOdZOT6cuEIZuKe"
	"pDb2Tx5JJbIN6N3fE/lVSW88K9aSCF2n15gYTSVmD0juHuLAoWnIicaa+Sbe2Tsj"
	"AQ=="
};
std::vector<std::string> TestData::certChain(certChainArr, certChainArr + 3);

std::string certChainSelfSignedArr[2] = {
	"MIICdzCCAeACAQcwDQYJKoZIhvcNAQEFBQAwgYIxCzAJBgNVBAYTAktSMQ4wDAYD"
	"VQQIDAVTZW91bDEQMA4GA1UECgwHU2Ftc3VuZzETMBEGA1UECwwKVGl6ZW4gVGVz"
	"dDEfMB0GA1UEAwwWVGVzdCBSb290IENBIFZlcnNpb24gMTEbMBkGCSqGSIb3DQEJ"
	"ARYMdHRAZ21haWwuY29tMB4XDTE0MDYxNDA4MTI1MFoXDTE1MDYxNDA4MTI1MFow"
	"gYQxCzAJBgNVBAYTAktSMQ4wDAYDVQQIDAVTZW91bDEQMA4GA1UECgwHU2Ftc3Vu"
	"ZzETMBEGA1UECwwKVGl6ZW4gVGVzdDEhMB8GA1UEAwwYVGVzdCBTZWNvbmQgQ0Eg"
	"VmVyc2lvbiAxMRswGQYJKoZIhvcNAQkBFgx0dEBnbWFpbC5jb20wgZ8wDQYJKoZI"
	"hvcNAQEBBQADgY0AMIGJAoGBAKOqFNxvO2jYcq5kqVehHH5k1D1dYwhBnH/SReWE"
	"OTSbH+3lbaKhJQHPHjsndENUxPInF6r0prO3TqoMB6774Pmc+znoVfLsHvWorhyr"
	"8iQNyaSgVWt0+8L0FU8iReqr5BR6YcZpnVRCV9dAIcf6FIVGUGZhTs/NvZDzIc4T"
	"9RrLAgMBAAEwDQYJKoZIhvcNAQEFBQADgYEAGDDvWhdMFg4GtDdytrK/GJ9TxX5F"
	"9iA/8qCl0+JU1U7jUVIcX77AxeZGBtq02X+DtjEWqnepS1iYO2TUHZBKRRCB2+wF"
	"ZsQ5XWngLSco+UvqUzMpWIQqslDXixWSR+Bef2S7iND3u8HJLjTncMcuJNpoXsFK"
	"bUiLqMVGQCkGZMo=",

	"MIICdTCCAd4CAQYwDQYJKoZIhvcNAQEFBQAwgYIxCzAJBgNVBAYTAktSMQ4wDAYD"
	"VQQIDAVTZW91bDEQMA4GA1UECgwHU2Ftc3VuZzETMBEGA1UECwwKVGl6ZW4gVGVz"
	"dDEfMB0GA1UEAwwWVGVzdCBSb290IENBIFZlcnNpb24gMTEbMBkGCSqGSIb3DQEJ"
	"ARYMdHRAZ21haWwuY29tMB4XDTE0MDYxNDA4MTIzNVoXDTE1MDYxNDA4MTIzNVow"
	"gYIxCzAJBgNVBAYTAktSMQ4wDAYDVQQIDAVTZW91bDEQMA4GA1UECgwHU2Ftc3Vu"
	"ZzETMBEGA1UECwwKVGl6ZW4gVGVzdDEfMB0GA1UEAwwWVGVzdCBSb290IENBIFZl"
	"cnNpb24gMTEbMBkGCSqGSIb3DQEJARYMdHRAZ21haWwuY29tMIGfMA0GCSqGSIb3"
	"DQEBAQUAA4GNADCBiQKBgQDtxGjhpaUK6xa4+sjMQfkKRAtjFkjZasVIt7uKUy/g"
	"GcC5i5aoorfyX/NBQLAVoIHMogHLgitehKL5l13tLR7DSETrG9V3Yx9bkWRcjyqH"
	"1TkD+NDOmhTtVuqIh4hrGKITlZK35hOh0IUEfYNNL8uq/11fVPpR3Yx97PT/j4w1"
	"uwIDAQABMA0GCSqGSIb3DQEBBQUAA4GBAOHjfa7nbPKhqR0mGfsscPQZZAZzKq9y"
	"ttdjTaNbnybzcJzcN3uwOdYKMf26Dn968nAPkukWe8j6GyMJ1C9LMAWqMn5hl0rI"
	"x6mUBfKZrl33BKH4KTYOrt0vnHdrCM2TwMkwMZ5ja5bBnbNrfF4e0HIAMor4rnVP"
	"WDSlESMMmtTm"
};
std::vector<std::string> TestData::certChainSelfSigned(certChainSelfSignedArr,
		certChainSelfSignedArr + 2);

const std::string TestData::certCAAuthor =
	"MIICnzCCAggCCQCn+GGT4zh+BjANBgkqhkiG9w0BAQUFADCBkzELMAkGA1UEBhMC"
	"S1IxDjAMBgNVBAgMBVN1d29uMQ4wDAYDVQQHDAVTdXdvbjEWMBQGA1UECgwNVGl6"
	"ZW4gVGVzdCBDQTElMCMGA1UECwwcVGl6ZW4gVGVzdCBEZXZlbG9wZXIgUm9vdCBD"
	"QTElMCMGA1UEAwwcVGl6ZW4gVGVzdCBEZXZlbG9wZXIgUm9vdCBDQTAeFw0xMjEw"
	"MjYwOTUwMTNaFw0yMjEwMjQwOTUwMTNaMIGTMQswCQYDVQQGEwJLUjEOMAwGA1UE"
	"CAwFU3V3b24xDjAMBgNVBAcMBVN1d29uMRYwFAYDVQQKDA1UaXplbiBUZXN0IENB"
	"MSUwIwYDVQQLDBxUaXplbiBUZXN0IERldmVsb3BlciBSb290IENBMSUwIwYDVQQD"
	"DBxUaXplbiBUZXN0IERldmVsb3BlciBSb290IENBMIGfMA0GCSqGSIb3DQEBAQUA"
	"A4GNADCBiQKBgQDWT6ZH5JyGadTUK1QmNwU8j+py4WtuElJE+4/wPFP8/KBmvvmI"
	"rGVjhUbKXToKIo8N6C/0SLxGEWuRAIoZHhg5JVbw1Ay7smgJJHizDUAqMTmV6LI9"
	"yTFbBV+OlO2Dir4LVdQ/XDBiqqslr7pqXgsg1V2g7x+tOI/f3dn2kWoVZQIDAQAB"
	"MA0GCSqGSIb3DQEBBQUAA4GBADGJYMtzUBDK+KKLZQ6zYmrKb+OWLlmEr/t/c2af"
	"KjTKUtommcz8VeTPqrDBOwxlVPdxlbhisCYzzvwnWeZk1aeptxxU3kdW9N3/wocN"
	"5nBzgqkkHJnj/ptqjrH2v/m0Z3hBuI4/akHIIfCBF8mUHwqcxYsRdcCIrkgp2Aiv"
	"bSaM";

const std::string TestData::certCAPublic =
	"MIIDOzCCAiOgAwIBAgIJAIghU0nmmUUlMA0GCSqGSIb3DQEBCwUAMFQxGjAYBgNV"
	"BAoMEVRpemVuIEFzc29jaWF0aW9uMTYwNAYDVQQDDC1UaXplbiBBc3NvY2lhdGlv"
	"biBERVZFTE9QRVIgUHVibGljIFJvb3QgQ2xhc3MwHhcNMTMxMjMwMTUwNTM5WhcN"
	"MzMxMjI1MTUwNTM5WjBUMRowGAYDVQQKDBFUaXplbiBBc3NvY2lhdGlvbjE2MDQG"
	"A1UEAwwtVGl6ZW4gQXNzb2NpYXRpb24gREVWRUxPUEVSIFB1YmxpYyBSb290IENs"
	"YXNzMIIBIjANBgkqhkiG9w0BAQEFAAOCAQ8AMIIBCgKCAQEA7Z1tVsO0gVYdAxyz"
	"+1mHNgHkdiLz8Drer9LXItILHOaJC21kBh3FByWL2fOxIAm7WGp/wCqqjY9GJRpz"
	"tj/3uYEIJKDLLfwgwQJF1WoV3XzwTtY/ZO/N9lH51PhW/qQy+1qP8E0H1meKgVM3"
	"m1IUuXLkpjzf1jfhcCM6gEQB9R4DBYd6ua0L/lGvkRQOjNMeAnEcmxCDXptRfu7v"
	"/fOx3ttIV81Lf18VdQIi3yzRbP68z/MTfPNwhm/QG+C+v+JCv4A/mchEhAuPuuSu"
	"1Op9QsiKZNY2wvJ0zOr44zM2smD3kbi4JgfYWUljFifDXsW1b6Lbzr42MsT+abpF"
	"ciJZCQIDAQABoxAwDjAMBgNVHRMEBTADAQH/MA0GCSqGSIb3DQEBCwUAA4IBAQAi"
	"nOTRl3o20ww/M6UJCnGaGUU1fx+E/u+JUybz/DZWQtwZDh2dyVCoTaiVXcY71OXq"
	"e4z+ZwarErdRnd6qfn6tNQBFJPj/quDULTVUHJwU72G/8Eh0zz0fQbCc7yD8vhMu"
	"y1ceP45exApLzqupc4bFq3N+/lJiDzVNU/fVWjsr1ut5lHsXzXmeYUEGh+iwoFgD"
	"z4zOJQ/0xwxcatkKBFZRnckUI46YyV9rlbeb9myTDqzRo/BXaor3lNbWJTvlKXrT"
	"rGGUot1OUwUY0ZqRInUlWE45uOdIO1FE0VvJiBr2nTfS/APbg475t4D4gq3hvOkg"
	"WdojCrFoNvutFOrtyxBz";

const std::string TestData::certCAPlatform =
	"MIIDPzCCAiegAwIBAgIJAMHhUV/SBOJTMA0GCSqGSIb3DQEBCwUAMFYxGjAYBgNV"
	"BAoMEVRpemVuIEFzc29jaWF0aW9uMTgwNgYDVQQDDC9UaXplbiBBc3NvY2lhdGlv"
	"biBERVZFTE9QRVIgUGxhdGZvcm0gUm9vdCBDbGFzczAeFw0xMzEyMzAxNTA1Mzla"
	"Fw0zMzEyMjUxNTA1MzlaMFYxGjAYBgNVBAoMEVRpemVuIEFzc29jaWF0aW9uMTgw"
	"NgYDVQQDDC9UaXplbiBBc3NvY2lhdGlvbiBERVZFTE9QRVIgUGxhdGZvcm0gUm9v"
	"dCBDbGFzczCCASIwDQYJKoZIhvcNAQEBBQADggEPADCCAQoCggEBAOSPcakP+RAY"
	"7zQ5CF/m+GgB6op0Bdn1Z90F72PRXZpBplyUqXhkx3Ug+x1EBAmOjo3BUh1f0u4u"
	"BXrxkI4H1wtgG3kMoprzMERLdIfCMDp+MCGslIAbApWbTZ7akv8FOcZyL3apQda+"
	"nLInd67uLOJ2YJ99CkeTZHKeoKy3P62gAZxw6j3UY+IBgdP8c+K3vII/KZj9Tl/G"
	"afliSibhK48yJ9TEEdYobnaTKG9dtk5aj+dr4I4LW33MVuOaW+l1lZfaUdFRPbq4"
	"eHw1tB/HOeV46yEoxmHKLNffsLuR2vhsXdyd/240JqnyEG1JoymPdRAM3uFSZYE1"
	"/jM69Hg/5uECAwEAAaMQMA4wDAYDVR0TBAUwAwEB/zANBgkqhkiG9w0BAQsFAAOC"
	"AQEADsv3CXY79VtNchvONSTU13OibFwcTZFaq3O3DB6ClZVRFChyad+ZwmhGEY1p"
	"rOwSFJZpPWOACuFdiSKg08FdJEUQMgQdQklUJY2kKAtSj40+4pIlvAd4MxGxj9JP"
	"6r0cdnp6/rWfPPVOSpPps6Vx3ML4lxYWcybSwcqdRtdXvQFlA6w7Syjs8KmbIuBf"
	"GmuFerjGJAawy3Uvlb7fuvtOXiPqLAOpNX/Qanj8yKxHPi4FcYdxD/n0yW+8SzeL"
	"y8B5VVJlUX0u1VYe07Q3mNwrto+xw/Un5GAd6nupz3MGduWODK1rC65+sz8Q5SwI"
	"Dbn/OGS3G5OJuLp3YnMLYpDkXA==";

const std::string TestData::certCAPartner =
	"MIICozCCAgwCCQD9IBoOxzq2hjANBgkqhkiG9w0BAQUFADCBlTELMAkGA1UEBhMC"
	"S1IxDjAMBgNVBAgMBVN1d29uMQ4wDAYDVQQHDAVTdXdvbjEWMBQGA1UECgwNVGl6"
	"ZW4gVGVzdCBDQTEiMCAGA1UECwwZVGl6ZW4gRGlzdHJpYnV0b3IgVGVzdCBDQTEq"
	"MCgGA1UEAwwhVGl6ZW4gUGFydG5lciBEaXN0cmlidXRvciBSb290IENBMB4XDTEy"
	"MTAyNjA4MTIzMVoXDTIyMTAyNDA4MTIzMVowgZUxCzAJBgNVBAYTAktSMQ4wDAYD"
	"VQQIDAVTdXdvbjEOMAwGA1UEBwwFU3V3b24xFjAUBgNVBAoMDVRpemVuIFRlc3Qg"
	"Q0ExIjAgBgNVBAsMGVRpemVuIERpc3RyaWJ1dG9yIFRlc3QgQ0ExKjAoBgNVBAMM"
	"IVRpemVuIFBhcnRuZXIgRGlzdHJpYnV0b3IgUm9vdCBDQTCBnzANBgkqhkiG9w0B"
	"AQEFAAOBjQAwgYkCgYEAnIBA2qQEaMzGalP0kzvwUxdCC6ybSC/fb+M9iGvt8QXp"
	"ic2yARQB+bIhfbEu1XHwE1jCAGxKd6uT91b4FWr04YwnBPoRX4rBGIYlqo/dg+pS"
	"rGyFjy7vfr0BOdWp2+WPlTe7SOS6bVauncrSoHxX0spiLaU5LU686BKr7YaABV0C"
	"AwEAATANBgkqhkiG9w0BAQUFAAOBgQAX0Tcfmxcs1TUPBdr1U1dx/W/6Y4PcAF7n"
	"DnMrR0ZNRPgeSCiVLax1bkHxcvW74WchdKIb24ZtAsFwyrsmUCRV842YHdfddjo6"
	"xgUu7B8n7hQeV3EADh6ft/lE8nalzAl9tALTxAmLtYvEYA7thvDoKi1k7bN48izL"
	"gS9G4WEAUg==";
