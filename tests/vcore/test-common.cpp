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

#include "test-common.h"

const std::string TestData::widget_path = std::string(TESTAPP_RES_DIR) + "apps/wgt";
const std::string TestData::widget_dist22_path = std::string(TESTAPP_RES_DIR) + "apps/wgt_dist22";
const std::string TestData::widget_negative_hash_path = std::string(TESTAPP_RES_DIR) +
		"apps/wgt_negative_hash";
const std::string TestData::widget_negative_signature_path = std::string(
			TESTAPP_RES_DIR) + "apps/wgt_negative_signature";
const std::string TestData::widget_negative_certificate_path = std::string(
			TESTAPP_RES_DIR) + "apps/wgt_negative_certificate";
const std::string TestData::widget_partner_path = std::string(TESTAPP_RES_DIR) + "apps/wgt_partner";
const std::string TestData::widget_platform_path = std::string(TESTAPP_RES_DIR) +
		"apps/wgt_platform";
const std::string TestData::widget_positive_link_path = std::string(TESTAPP_RES_DIR) +
		"apps/wgt_positive_link";
const std::string TestData::widget_negative_link_path = std::string(TESTAPP_RES_DIR) +
		"apps/wgt_negative_link";

const std::string TestData::tpk_path = std::string(TESTAPP_RES_DIR) + "apps/tpk";
const std::string TestData::attacked_tpk_path = std::string(TESTAPP_RES_DIR) + "apps/attacked-tpk";
const std::string TestData::tpk_with_userdata_path = std::string(TESTAPP_RES_DIR) +
		"apps/tpk-with-userdata";
const std::string TestData::attacked_tpk_with_userdata_path = std::string(
			TESTAPP_RES_DIR) + "apps/attacked-tpk-with-userdata";

const std::string TestData::certEE =
	"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";

const std::string TestData::certIM =
	"MIIFKzCCBBOgAwIBAgIQfuFKb2/v8tN/P61lTTratDANBgkqhkiG9w0BAQsFADCB\n"
	"yjELMAkGA1UEBhMCVVMxFzAVBgNVBAoTDlZlcmlTaWduLCBJbmMuMR8wHQYDVQQL\n"
	"ExZWZXJpU2lnbiBUcnVzdCBOZXR3b3JrMTowOAYDVQQLEzEoYykgMjAwNiBWZXJp\n"
	"U2lnbiwgSW5jLiAtIEZvciBhdXRob3JpemVkIHVzZSBvbmx5MUUwQwYDVQQDEzxW\n"
	"ZXJpU2lnbiBDbGFzcyAzIFB1YmxpYyBQcmltYXJ5IENlcnRpZmljYXRpb24gQXV0\n"
	"aG9yaXR5IC0gRzUwHhcNMTMxMDMxMDAwMDAwWhcNMjMxMDMwMjM1OTU5WjB3MQsw\n"
	"CQYDVQQGEwJVUzEdMBsGA1UEChMUU3ltYW50ZWMgQ29ycG9yYXRpb24xHzAdBgNV\n"
	"BAsTFlN5bWFudGVjIFRydXN0IE5ldHdvcmsxKDAmBgNVBAMTH1N5bWFudGVjIENs\n"
	"YXNzIDMgRVYgU1NMIENBIC0gRzMwggEiMA0GCSqGSIb3DQEBAQUAA4IBDwAwggEK\n"
	"AoIBAQDYoWV0I+grZOIy1zM3PY71NBZI3U9/hxz4RCMTjvsR2ERaGHGOYBYmkpv9\n"
	"FwvhcXBC/r/6HMCqo6e1cej/GIP23xAKE2LIPZyn3i4/DNkd5y77Ks7Imn+Hv9hM\n"
	"BBUyydHMlXGgTihPhNk1++OGb5RT5nKKY2cuvmn2926OnGAE6yn6xEdC0niY4+wL\n"
	"pZLct5q9gGQrOHw4CVtm9i2VeoayNC6FnpAOX7ddpFFyRnATv2fytqdNFB5suVPu\n"
	"IxpOjUhVQ0GxiXVqQCjFfd3SbtICGS97JJRL6/EaqZvjI5rq+jOrCiy39GAI3Z8c\n"
	"zd0tAWaAr7MvKR0juIrhoXAHDDQPAgMBAAGjggFdMIIBWTAvBggrBgEFBQcBAQQj\n"
	"MCEwHwYIKwYBBQUHMAGGE2h0dHA6Ly9zMi5zeW1jYi5jb20wEgYDVR0TAQH/BAgw\n"
	"BgEB/wIBADBlBgNVHSAEXjBcMFoGBFUdIAAwUjAmBggrBgEFBQcCARYaaHR0cDov\n"
	"L3d3dy5zeW1hdXRoLmNvbS9jcHMwKAYIKwYBBQUHAgIwHBoaaHR0cDovL3d3dy5z\n"
	"eW1hdXRoLmNvbS9ycGEwMAYDVR0fBCkwJzAloCOgIYYfaHR0cDovL3MxLnN5bWNi\n"
	"LmNvbS9wY2EzLWc1LmNybDAOBgNVHQ8BAf8EBAMCAQYwKQYDVR0RBCIwIKQeMBwx\n"
	"GjAYBgNVBAMTEVN5bWFudGVjUEtJLTEtNTMzMB0GA1UdDgQWBBQBWavn3ToLWaZk\n"
	"Y9bPIAdX1ZHnajAfBgNVHSMEGDAWgBR/02Wnwt3su/AwCfNDOfoCrzMxMzANBgkq\n"
	"hkiG9w0BAQsFAAOCAQEAQgFVe9AWGl1Y6LubqE3X89frE5SG1n8hC0e8V5uSXU8F\n"
	"nzikEHzPg74GQ0aNCLxq1xCm+quvL2GoY/Jl339MiBKIT7Np2f8nwAqXkY9W+4nE\n"
	"qLuSLRtzsMarNvSWbCAI7woeZiRFT2cAQMgHVHQzO6atuyOfZu2iRHA0+w7qAf3P\n"
	"eHTfp61Vt19N9tY/4IbOJMdCqRMURDVLtt/JYKwMf9mTIUvunORJApjTYHtcvNUw\n"
	"LwfORELEC5n+5p/8sHiGUW3RLJ3GlvuFgrsEL/digO9i2n/2DqyQuFa9eT/ygG6j\n"
	"2bkPXToHHZGThkspTOHcteHgM52zyzaRS/6htO7w+Q==";

const std::string TestData::certRoot =
	"MIIE0zCCA7ugAwIBAgIQGNrRniZ96LtKIVjNzGs7SjANBgkqhkiG9w0BAQUFADCB\n"
	"yjELMAkGA1UEBhMCVVMxFzAVBgNVBAoTDlZlcmlTaWduLCBJbmMuMR8wHQYDVQQL\n"
	"ExZWZXJpU2lnbiBUcnVzdCBOZXR3b3JrMTowOAYDVQQLEzEoYykgMjAwNiBWZXJp\n"
	"U2lnbiwgSW5jLiAtIEZvciBhdXRob3JpemVkIHVzZSBvbmx5MUUwQwYDVQQDEzxW\n"
	"ZXJpU2lnbiBDbGFzcyAzIFB1YmxpYyBQcmltYXJ5IENlcnRpZmljYXRpb24gQXV0\n"
	"aG9yaXR5IC0gRzUwHhcNMDYxMTA4MDAwMDAwWhcNMzYwNzE2MjM1OTU5WjCByjEL\n"
	"MAkGA1UEBhMCVVMxFzAVBgNVBAoTDlZlcmlTaWduLCBJbmMuMR8wHQYDVQQLExZW\n"
	"ZXJpU2lnbiBUcnVzdCBOZXR3b3JrMTowOAYDVQQLEzEoYykgMjAwNiBWZXJpU2ln\n"
	"biwgSW5jLiAtIEZvciBhdXRob3JpemVkIHVzZSBvbmx5MUUwQwYDVQQDEzxWZXJp\n"
	"U2lnbiBDbGFzcyAzIFB1YmxpYyBQcmltYXJ5IENlcnRpZmljYXRpb24gQXV0aG9y\n"
	"aXR5IC0gRzUwggEiMA0GCSqGSIb3DQEBAQUAA4IBDwAwggEKAoIBAQCvJAgIKXo1\n"
	"nmAMqudLO07cfLw8RRy7K+D+KQL5VwijZIUVJ/XxrcgxiV0i6CqqpkKzj/i5Vbex\n"
	"t0uz/o9+B1fs70PbZmIVYc9gDaTY3vjgw2IIPVQT60nKWVSFJuUrjxuf6/WhkcIz\n"
	"SdhDY2pSS9KP6HBRTdGJaXvHcPaz3BJ023tdS1bTlr8Vd6Gw9KIl8q8ckmcY5fQG\n"
	"BO+QueQA5N06tRn/Arr0PO7gi+s3i+z016zy9vA9r911kTMZHRxAy3QkGSGT2RT+\n"
	"rCpSx4/VBEnkjWNHiDxpg8v+R70rfk/Fla4OndTRQ8Bnc+MUCH7lP59zuDMKz10/\n"
	"NIeWiu5T6CUVAgMBAAGjgbIwga8wDwYDVR0TAQH/BAUwAwEB/zAOBgNVHQ8BAf8E\n"
	"BAMCAQYwbQYIKwYBBQUHAQwEYTBfoV2gWzBZMFcwVRYJaW1hZ2UvZ2lmMCEwHzAH\n"
	"BgUrDgMCGgQUj+XTGoasjY5rw8+AatRIGCx7GS4wJRYjaHR0cDovL2xvZ28udmVy\n"
	"aXNpZ24uY29tL3ZzbG9nby5naWYwHQYDVR0OBBYEFH/TZafC3ey78DAJ80M5+gKv\n"
	"MzEzMA0GCSqGSIb3DQEBBQUAA4IBAQCTJEowX2LP2BqYLz3q3JktvXf2pXkiOOzE\n"
	"p6B4Eq1iDkVwZMXnl2YtmAl+X6/WzChl8gGqCBpH3vn5fJJaCGkgDdk+bW48DW7Y\n"
	"5gaRQBi5+MHt39tBquCWIMnNZBU4gcmU7qKEKQsTb47bDN0lAtukixlE0kF6BWlK\n"
	"WE9gyn6CagsCqiUXObXbf+eEZSqVir2G3l6BFoMtEMze/aiCKm0oHw0LxOXnGiYZ\n"
	"4fQRbxC1lfznQgUy286dUV4otp6F01vvpX1FQHKOtw5rDgb7MzVIcbidJ4vEZV8N\n"
	"hnacRHr2lVz2XTIIM6RUthg/aFzyQkqFOFSDX9HoLPKsEdao7WNq";

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

const std::string TestData::google3rd =
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

const std::string TestData::certVerisign =
	"MIIG+DCCBeCgAwIBAgIQU9K++SSnJF6DygHkbKokdzANBgkqhkiG9w0BAQUFADCB"
	"vjELMAkGA1UEBhMCVVMxFzAVBgNVBAoTDlZlcmlTaWduLCBJbmMuMR8wHQYDVQQL"
	"ExZWZXJpU2lnbiBUcnVzdCBOZXR3b3JrMTswOQYDVQQLEzJUZXJtcyBvZiB1c2Ug"
	"YXQgaHR0cHM6Ly93d3cudmVyaXNpZ24uY29tL3JwYSAoYykwNjE4MDYGA1UEAxMv"
	"VmVyaVNpZ24gQ2xhc3MgMyBFeHRlbmRlZCBWYWxpZGF0aW9uIFNTTCBTR0MgQ0Ew"
	"HhcNMTAwNTI2MDAwMDAwWhcNMTIwNTI1MjM1OTU5WjCCASkxEzARBgsrBgEEAYI3"
	"PAIBAxMCVVMxGTAXBgsrBgEEAYI3PAIBAhMIRGVsYXdhcmUxGzAZBgNVBA8TElYx"
	"LjAsIENsYXVzZSA1LihiKTEQMA4GA1UEBRMHMjQ5Nzg4NjELMAkGA1UEBhMCVVMx"
	"DjAMBgNVBBEUBTk0MDQzMRMwEQYDVQQIEwpDYWxpZm9ybmlhMRYwFAYDVQQHFA1N"
	"b3VudGFpbiBWaWV3MSIwIAYDVQQJFBk0ODcgRWFzdCBNaWRkbGVmaWVsZCBSb2Fk"
	"MRcwFQYDVQQKFA5WZXJpU2lnbiwgSW5jLjEmMCQGA1UECxQdIFByb2R1Y3Rpb24g"
	"U2VjdXJpdHkgU2VydmljZXMxGTAXBgNVBAMUEHd3dy52ZXJpc2lnbi5jb20wggEi"
	"MA0GCSqGSIb3DQEBAQUAA4IBDwAwggEKAoIBAQCj+PvvK+fZOXwno0yT/OTy2Zm9"
	"ehnZjTtO/X2IWBEa3jG30C52uHFQI4NmXiQVNvJHkBaAj0ilVjvGdxXmkyyFsugt"
	"IWOTZ8pSKdX1tmGFIon6Ko9+lBFkVkudA1ogAUbtTB8IcdeOlpK78T4SjdVMhY18"
	"150YzSw6hRKlw52wBaDxtGZElvOth41K7TUcaDnQVzz5SBPW5MUhi7AWrdoSk17O"
	"BozOzmB/jkYDVDnwLcbR89SLHEOle/idSYSDQUmab3y0JS8RyQV1+DB70mnFALnD"
	"fLiL47nMQQCGxXgp5voQ2YmSXhevKmEJ9vvtC6C7yv2W6yomfS/weUEce9pvAgMB"
	"AAGjggKCMIICfjCBiwYDVR0RBIGDMIGAghB3d3cudmVyaXNpZ24uY29tggx2ZXJp"
	"c2lnbi5jb22CEHd3dy52ZXJpc2lnbi5uZXSCDHZlcmlzaWduLm5ldIIRd3d3LnZl"
	"cmlzaWduLm1vYmmCDXZlcmlzaWduLm1vYmmCD3d3dy52ZXJpc2lnbi5ldYILdmVy"
	"aXNpZ24uZXUwCQYDVR0TBAIwADAdBgNVHQ4EFgQU8oBwK/WBXCZDWi0dbuDgPyTK"
	"iJIwCwYDVR0PBAQDAgWgMD4GA1UdHwQ3MDUwM6AxoC+GLWh0dHA6Ly9FVkludGwt"
	"Y3JsLnZlcmlzaWduLmNvbS9FVkludGwyMDA2LmNybDBEBgNVHSAEPTA7MDkGC2CG"
	"SAGG+EUBBxcGMCowKAYIKwYBBQUHAgEWHGh0dHBzOi8vd3d3LnZlcmlzaWduLmNv"
	"bS9ycGEwKAYDVR0lBCEwHwYIKwYBBQUHAwEGCCsGAQUFBwMCBglghkgBhvhCBAEw"
	"HwYDVR0jBBgwFoAUTkPIHXbvN1N6T/JYb5TzOOLVvd8wdgYIKwYBBQUHAQEEajBo"
	"MCsGCCsGAQUFBzABhh9odHRwOi8vRVZJbnRsLW9jc3AudmVyaXNpZ24uY29tMDkG"
	"CCsGAQUFBzAChi1odHRwOi8vRVZJbnRsLWFpYS52ZXJpc2lnbi5jb20vRVZJbnRs"
	"MjAwNi5jZXIwbgYIKwYBBQUHAQwEYjBgoV6gXDBaMFgwVhYJaW1hZ2UvZ2lmMCEw"
	"HzAHBgUrDgMCGgQUS2u5KJYGDLvQUjibKaxLB4shBRgwJhYkaHR0cDovL2xvZ28u"
	"dmVyaXNpZ24uY29tL3ZzbG9nbzEuZ2lmMA0GCSqGSIb3DQEBBQUAA4IBAQB9VZxB"
	"wDMRGyhFWYkY5rwUVGuDJiGeas2xRJC0G4+riQ7IN7pz2a2BhktmZ5HbxXL4ZEY4"
	"yMN68DEVErhtKiuL02ng27alhlngadKQzSL8pLdmQ+3jEwm9nva5C/7pbeqy+qGF"
	"is4IWNYOc4HKNkABxXm5v0ouys8HPNkTLFLep0gLqRXW3gYN2XbKUWMs7z7hJpkY"
	"GxP8YQSxi513O2dWVCXB8S6erIz9E/bcfdXoCPyQdn42y3IEoJvPvBS3S55fD4+Q"
	"Q43GPhumSg9a6S3hnyw8DX5OiUGmqgQrtSeDRsNmWqtWizEQbe+fotZpEn/7zYTa"
	"tk1ni/k5jDH/QeuG";

const std::string TestData::certGenUriIncluded =
	"MIIFKjCCBBKgAwIBAgIBZTANBgkqhkiG9w0BAQsFADCBnjELMAkGA1UEBhMCS1Ix"
	"FDASBgNVBAgMC1NvdXRoIEtvcmVhMQ4wDAYDVQQHDAVTdXdvbjEmMCQGA1UECgwd"
	"U2Ftc3VuZyBFbGVjdHJvbmljcyBDby4sIEx0ZC4xDzANBgNVBAsMBk1vYmlsZTEw"
	"MC4GA1UEAwwnU2Ftc3VuZyBUaXplbiBERVZFTE9QRVIgUHVibGljIENBIENsYXNz"
	"MB4XDTE2MTAxMTA2MDcxNloXDTE3MTAxMTA2MDcxNlowcDERMA8GA1UEAwwIVGl6"
	"ZW5TREsxCTAHBgNVBAsMADEJMAcGA1UECgwAMQkwBwYDVQQHDAAxCTAHBgNVBAgM"
	"ADEJMAcGA1UEBhMAMSQwIgYJKoZIhvcNAQkBFhVqYzgxNS5sZWVAc2Ftc3VuZy5j"
	"b20wggEiMA0GCSqGSIb3DQEBAQUAA4IBDwAwggEKAoIBAQCNnAGGY63LX79FH4cq"
	"WXPCL995W3hZ8l1q9vlJgH00KV7jgPj0NTc2rjKgp/gmbco/vZV8Bp/tbAYDvVRu"
	"8ObgcjiUDANm9jRJLNu0GBVmhbBQJI5rmZ9xJofJSEHTdaO8aeGfyAvMgdrzyvE1"
	"QYX/QYKN5/YMMtf/A/uN1d+e20/F89UFsMGC/2LzkUwIJGe9PR1w6x/sb9+y6cap"
	"XbfjoFN5TEL+nI5qxUNDmTXwglN6HVE3eBfw4Doz9SIKM0RH5vbUPOsmuuw0obHh"
	"Qh5qpAGNGxzRibUM7Kv9rLpTtD6WpC7beVq8Gbct7zZCzEEEyAvf3vI4aDieOE/E"
	"0tZxAgMBAAGjggGeMIIBmjCCAZYGA1UdEQSCAY0wggGJhhRVUk46dGl6ZW46cGFj"
	"a2FnZWlkPYYzVVJOOnRpemVuOmRldmljZWlkPTIuMCNSMXErbFBzR0NNN1BBRVB2"
	"elVjYzM4eFBDTHc9hjNVUk46dGl6ZW46ZGV2aWNlaWQ9Mi4wI1IycStsUHNHQ003"
	"UEFFUHZ6VWNjMzh4UENMdz2GM1VSTjp0aXplbjpkZXZpY2VpZD0yLjAjUjNxK2xQ"
	"c0dDTTdQQUVQdnpVY2MzOHhQQ0x3PYYzVVJOOnRpemVuOmRldmljZWlkPTIuMCNS"
	"NHErbFBzR0NNN1BBRVB2elVjYzM4eFBDTHc9hjNVUk46dGl6ZW46ZGV2aWNlaWQ9"
	"Mi4wI1I1cStsUHNHQ003UEFFUHZ6VWNjMzh4UENMdz2GM1VSTjp0aXplbjpkZXZp"
	"Y2VpZD0yLjAjUjZxK2xQc0dDTTdQQUVQdnpVY2MzOHhQQ0x3PYYzVVJOOnRpemVu"
	"OmRldmljZWlkPTIuMCNSN3ErbFBzR0NNN1BBRVB2elVjYzM4eFBDTHc9MA0GCSqG"
	"SIb3DQEBCwUAA4IBAQBJ6uM+XOwTFl39wj5VdvDZr5GzreKG8PUw7L2wLeVxC2h9"
	"+q3jKannvhrHjCC6mPzjVhYQgoH02ywqDmfMJnE8h1GaF2KGZbuVkL/S02UZ+CwJ"
	"6J/2dBsakzn1yAjloGq1TQ/AFeN7tEC1ZB88XeLlH6fBKLC41HxLlEKZmyVU281Y"
	"Hu1gDU7+7BzMY21+RkPKVCJXnyZmv34LL+V199OnLphurx4kfrkRKz2ujTDefzLG"
	"qERauXufUJ8eWPBgjgxewkQJeUK+pqQ1XbvQA7hHDTxbtbU4sjYHIrDEm2LVJB7b"
	"+eUuTos3aMVELp3gK4wnj/WoSPJ4hudhGUco4cLT";

