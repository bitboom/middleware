%define certsvc_test_build 0

Name:    cert-svc
Summary: Certification service
Version: 2.1.3
Release: 0
Group:   Security/Certificate Management
License: Apache-2.0
Source0: %{name}-%{version}.tar.gz
BuildRequires: cmake
BuildRequires: coreutils
BuildRequires: findutils
BuildRequires: openssl
BuildRequires: pkgconfig(dlog)
BuildRequires: pkgconfig(openssl)
BuildRequires: pkgconfig(libpcrecpp)
BuildRequires: pkgconfig(xmlsec1)
BuildRequires: pkgconfig(libxml-2.0)
BuildRequires: pkgconfig(libxslt)
BuildRequires: pkgconfig(db-util)
BuildRequires: pkgconfig(libsystemd-daemon)
BuildRequires: pkgconfig(key-manager)
BuildRequires: pkgconfig(libtzplatform-config)
BuildRequires: pkgconfig(libsystemd-journal)
BuildRequires: pkgconfig(sqlite3)
BuildRequires: ca-certificates
BuildRequires: ca-certificates-devel
BuildRequires: ca-certificates-tizen-devel
Requires: ca-certificates
Requires: ca-certificates-tizen
Requires: security-config
%if "%{?profile}" == "mobile"
BuildRequires: pkgconfig(cert-checker)
%endif

%global USER_NAME security_fw
%global GROUP_NAME security_fw
%global SMACK_DOMAIN_NAME System

%global TZ_SYS_BIN              %{?TZ_SYS_BIN:%TZ_SYS_BIN}%{!?TZ_SYS_BIN:%_bindir}
%global TZ_SYS_ETC              %{?TZ_SYS_ETC:%TZ_SYS_ETC}%{!?TZ_SYS_ETC:/opt/etc}
%global TZ_SYS_SHARE            %{?TZ_SYS_SHARE:%TZ_SYS_SHARE}%{!?TZ_SYS_SHARE:/opt/share}
%global TZ_SYS_RO_SHARE         %{?TZ_SYS_RO_SHARE:%TZ_SYS_RO_SHARE}%{!?TZ_SYS_RO_SHARE:%_datadir}
%global TZ_SYS_RW_APP           %{?TZ_SYS_RW_APP:%TZ_SYS_RW_APP}%{!?TZ_SYS_RW_APP:/opt/usr/apps}

%global TZ_SYS_CA_CERTS         %{?TZ_SYS_CA_CERTS:%TZ_SYS_CA_CERTS}%{!?TZ_SYS_CA_CERTS:%TZ_SYS_ETC/ssl/certs}
%global TZ_SYS_RO_CA_CERTS_ORIG %{?TZ_SYS_RO_CA_CERTS_ORIG:%TZ_SYS_RO_CA_CERTS_ORIG}%{!?TZ_SYS_CA_RO_CERTS_ORGIN:%TZ_SYS_RO_SHARE/ca-certificates/certs}
%global TZ_SYS_CA_BUNDLE        %{?TZ_SYS_CA_BUNDLE:%TZ_SYS_CA_BUNDLE}%{!?TZ_SYS_CA_BUNDLE:/var/lib/ca-certificates/ca-bundle.pem}

%global CERT_SVC_PATH           %TZ_SYS_SHARE/cert-svc
%global CERT_SVC_RO_PATH        %TZ_SYS_RO_SHARE/cert-svc
%global CERT_SVC_DB_PATH        %CERT_SVC_PATH/dbspace
%global CERT_SVC_PKCS12         %CERT_SVC_PATH/pkcs12
%global CERT_SVC_CA_BUNDLE      %CERT_SVC_PATH/ca-certificate.crt
%global CERT_SVC_TESTS          %TZ_SYS_RW_APP/cert-svc-tests

%global CERT_SVC_OLD_DB_PATH    /opt/share/cert-svc/dbspace
%global UPGRADE_SCRIPT_PATH     %TZ_SYS_RO_SHARE/upgrade/scripts
%global UPGRADE_DATA_PATH       %TZ_SYS_RO_SHARE/upgrade/data

%description
Certification service

%package devel
Summary:    Certification service (development files)
Group:      Development/Libraries
Requires:   %{name} = %{version}-%{release}

%description devel
Certification service (development files)

%if 0%{?certsvc_test_build}
%package test
Summary:  Certification service (tests)
Group:    Security/Testing
Requires: ca-certificates-tizen
Requires: %{name} = %{version}-%{release}

%description test
Certification service (tests)
%endif

%prep
%setup -q

%build
export CFLAGS="$CFLAGS -DTIZEN_DEBUG_ENABLE"
export CXXFLAGS="$CXXFLAGS -DTIZEN_DEBUG_ENABLE"
export FFLAGS="$FFLAGS -DTIZEN_DEBUG_ENABLE"

export CFLAGS="$CFLAGS -DTIZEN_ENGINEER_MODE"
export CXXFLAGS="$CXXFLAGS -DTIZEN_ENGINEER_MODE"
export FFLAGS="$FFLAGS -DTIZEN_ENGINEER_MODE"

%ifarch %{ix86}
export CFLAGS="$CFLAGS -DTIZEN_EMULATOR_MODE"
export CXXFLAGS="$CXXFLAGS -DTIZEN_EMULATOR_MODE"
export FFLAGS="$FFLAGS -DTIZEN_EMULATOR_MODE"
%endif

%{!?build_type:%define build_type "Release"}
%cmake . -DVERSION=%version \
         -DINCLUDEDIR=%_includedir \
         -DUSER_NAME=%USER_NAME \
         -DGROUP_NAME=%GROUP_NAME \
         -DSMACK_DOMAIN_NAME=%SMACK_DOMAIN_NAME \
         -DTZ_SYS_SHARE=%TZ_SYS_SHARE \
         -DTZ_SYS_RO_SHARE=%TZ_SYS_RO_SHARE \
         -DTZ_SYS_BIN=%TZ_SYS_BIN \
         -DTZ_SYS_CA_CERTS=%TZ_SYS_CA_CERTS \
         -DTZ_SYS_CA_CERTS_ORIG=%TZ_SYS_CA_CERTS_ORIG \
         -DTZ_SYS_CA_BUNDLE=%TZ_SYS_CA_BUNDLE \
         -DFINGERPRINT_LIST_RW_PATH=%TZ_SYS_REVOKED_CERTS_FINGERPRINTS_RUNTIME \
         -DCERT_SVC_PATH=%CERT_SVC_PATH \
         -DCERT_SVC_RO_PATH=%CERT_SVC_RO_PATH \
         -DCERT_SVC_PKCS12=%CERT_SVC_PKCS12 \
         -DCERT_SVC_DB_PATH=%CERT_SVC_DB_PATH \
         -DCERT_SVC_OLD_DB_PATH=%CERT_SVC_OLD_DB_PATH \
         -DUPGRADE_SCRIPT_PATH=%UPGRADE_SCRIPT_PATH \
         -DUPGRADE_DATA_PATH=%UPGRADE_DATA_PATH \
%if "%{?profile}" == "mobile"
         -DTIZEN_PROFILE_MOBILE:BOOL=ON \
%else
         -DTIZEN_PROFILE_MOBILE:BOOL=OFF \
%endif
%if 0%{?certsvc_test_build}
         -DCERTSVC_TEST_BUILD=1 \
         -DCERT_SVC_TESTS=%CERT_SVC_TESTS \
%endif
         -DCMAKE_BUILD_TYPE=%build_type \
         -DSYSTEMD_UNIT_DIR=%_unitdir

make %{?_smp_mflags}

%install
%make_install
%install_service sockets.target.wants cert-server.socket

mkdir -p %buildroot%CERT_SVC_PKCS12

touch %buildroot%CERT_SVC_DB_PATH/certs-meta.db-journal
mkdir -p %buildroot%UPGRADE_DATA_PATH
cp %buildroot%CERT_SVC_DB_PATH/certs-meta.db %buildroot%UPGRADE_DATA_PATH

ln -sf %TZ_SYS_CA_BUNDLE %buildroot%CERT_SVC_CA_BUNDLE

%preun
# erase
if [ $1 = 0 ]; then
    systemctl stop cert-server.service
fi

%post
/sbin/ldconfig
systemctl daemon-reload
# install
if [ $1 = 1 ]; then
    systemctl start cert-server.socket
fi
# upgrade / reinstall
if [ $1 = 2 ]; then
    systemctl restart cert-server.socket
fi

%postun -p /sbin/ldconfig

%files
%manifest %name.manifest
%license LICENSE
%_unitdir/cert-server.service
%_unitdir/cert-server.socket
%_unitdir/sockets.target.wants/cert-server.socket
%_libdir/libcert-svc-vcore.so.*
%TZ_SYS_BIN/cert-server
%dir %attr(-, %{USER_NAME}, %{GROUP_NAME}) %CERT_SVC_PATH
%dir %attr(-, %{USER_NAME}, %{GROUP_NAME}) %CERT_SVC_PKCS12
%dir %attr(-, %{USER_NAME}, %{GROUP_NAME}) %CERT_SVC_DB_PATH
%attr(-, %{USER_NAME}, %{GROUP_NAME}) %CERT_SVC_CA_BUNDLE
%attr(-, %{USER_NAME}, %{GROUP_NAME}) %CERT_SVC_DB_PATH/certs-meta.db
%attr(-, %{USER_NAME}, %{GROUP_NAME}) %CERT_SVC_DB_PATH/certs-meta.db-journal
%attr(-, %{USER_NAME}, %{GROUP_NAME}) %CERT_SVC_RO_PATH

%attr(755, root, root) %UPGRADE_SCRIPT_PATH/cert-svc-db-upgrade.sh
%attr(755, root, root) %UPGRADE_SCRIPT_PATH/cert-svc-disabled-certs-upgrade.sh
%{UPGRADE_DATA_PATH}/certs-meta.db

%files devel
%_includedir/*
%_libdir/pkgconfig/*
%_libdir/libcert-svc-vcore.so

%if 0%{?certsvc_test_build}
%files test
%TZ_SYS_BIN/cert-svc-test*
%CERT_SVC_TESTS
%_libdir/libcert-svc-validator-plugin.so
%attr(755, root, root) %UPGRADE_SCRIPT_PATH/cert-svc-test-upgrade.sh
%{UPGRADE_DATA_PATH}/certs-meta-old.db
%endif
