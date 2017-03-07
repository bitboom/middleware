%define certsvc_test_build 0

Name:    cert-svc
Summary: Certification service
Version: 2.1.6
Release: 0
Group:   Security/Certificate Management
License: Apache-2.0 and OpenSSL
Source0: %{name}-%{version}.tar.gz
BuildRequires: cmake
BuildRequires: coreutils
BuildRequires: findutils
BuildRequires: openssl
BuildRequires: pkgconfig(dlog)
BuildRequires: pkgconfig(klay)
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
Requires: openssl

%global user_name security_fw
%global group_name security_fw
%global server_stream /tmp/.cert-server.socket
%global smack_domain_name System

%global bin_dir                 %{?TZ_SYS_BIN:%TZ_SYS_BIN}%{!?TZ_SYS_BIN:%_bindir}
%global etc_dir                 %{?TZ_SYS_ETC:%TZ_SYS_ETC}%{!?TZ_SYS_ETC:/opt/etc}
%global rw_data_dir             %{?TZ_SYS_SHARE:%TZ_SYS_SHARE}%{!?TZ_SYS_SHARE:/opt/share}
%global ro_data_dir             %{?TZ_SYS_RO_SHARE:%TZ_SYS_RO_SHARE}%{!?TZ_SYS_RO_SHARE:%_datadir}
%global rw_app_dir              %{?TZ_SYS_RW_APP:%TZ_SYS_RW_APP}%{!?TZ_SYS_RW_APP:/opt/usr/apps}

%global cert_svc_path           %rw_data_dir/cert-svc
%global cert_svc_ro_path        %ro_data_dir/cert-svc
%global cert_svc_db_path        %cert_svc_path/dbspace
%global cert_svc_pkcs12         %cert_svc_path/pkcs12
%global cert_svc_transec        %cert_svc_path/transec
%global cert_svc_transec_res    %cert_svc_transec/res
%global cert_svc_transec_usr    %cert_svc_transec/usr
%global cert_svc_transec_global %cert_svc_transec/global
%global cert_svc_transec_bundle %cert_svc_transec_res/ca-bundle.pem
%global cert_svc_ca_bundle      %cert_svc_path/ca-certificate.crt
%global cert_svc_examples       %cert_svc_ro_path/examples
%global cert_svc_tests          %rw_app_dir/cert-svc-tests

%global cert_svc_old_db_path    /opt/share/cert-svc/dbspace
%global upgrade_script_path     %ro_data_dir/upgrade/scripts
%global upgrade_data_path       %ro_data_dir/upgrade/data

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
%if 0%{?sec_build_binary_debug_enable}
export CFLAGS="$CFLAGS -DTIZEN_DEBUG_ENABLE"
export CXXFLAGS="$CXXFLAGS -DTIZEN_DEBUG_ENABLE"
export FFLAGS="$FFLAGS -DTIZEN_DEBUG_ENABLE"
%endif

%if 0%{?tizen_build_devel_mode}
export CFLAGS="$CFLAGS -DTIZEN_ENGINEER_MODE"
export CXXFLAGS="$CXXFLAGS -DTIZEN_ENGINEER_MODE"
export FFLAGS="$FFLAGS -DTIZEN_ENGINEER_MODE"
%endif

%ifarch %{ix86}
export CFLAGS="$CFLAGS -DTIZEN_EMULATOR_MODE"
export CXXFLAGS="$CXXFLAGS -DTIZEN_EMULATOR_MODE"
export FFLAGS="$FFLAGS -DTIZEN_EMULATOR_MODE"
%endif

%{!?build_type:%define build_type "Release"}
%cmake . -DVERSION=%version \
         -DINCLUDEDIR=%_includedir \
         -DUSER_NAME=%user_name \
         -DGROUP_NAME=%group_name \
         -DSERVER_STREAM=%server_stream \
         -DSMACK_DOMAIN_NAME=%smack_domain_name \
         -DRO_DATA_DIR=%ro_data_dir \
         -DBIN_DIR=%bin_dir \
         -DTZ_SYS_CA_CERTS=%TZ_SYS_CA_CERTS \
         -DTZ_SYS_CA_CERTS_ORIG=%TZ_SYS_CA_CERTS_ORIG \
         -DTZ_SYS_CA_BUNDLE=%TZ_SYS_CA_BUNDLE \
         -DTZ_SYS_RO_CA_CERTS=%TZ_SYS_RO_CA_CERTS \
         -DTZ_SYS_RO_CA_BUNDLE=%TZ_SYS_RO_CA_BUNDLE \
         -DCERT_SVC_CA_BUNDLE=%cert_svc_ca_bundle \
         -DFINGERPRINT_LIST_RW_PATH=%TZ_SYS_REVOKED_CERTS_FINGERPRINTS_RUNTIME \
         -DCERT_SVC_PATH=%cert_svc_path \
         -DCERT_SVC_RO_PATH=%cert_svc_ro_path \
         -DCERT_SVC_PKCS12=%cert_svc_pkcs12 \
         -DCERT_SVC_TRANSEC_USR=%cert_svc_transec_usr \
         -DCERT_SVC_TRANSEC_GLOBAL=%cert_svc_transec_global \
         -DCERT_SVC_TRANSEC_BUNDLE=%cert_svc_transec_bundle \
         -DCERT_SVC_DB_PATH=%cert_svc_db_path \
         -DCERT_SVC_OLD_DB_PATH=%cert_svc_old_db_path \
         -DUPGRADE_SCRIPT_PATH=%upgrade_script_path \
         -DUPGRADE_DATA_PATH=%upgrade_data_path \
%if 0%{?certsvc_test_build}
         -DCERTSVC_TEST_BUILD=1 \
         -DCERT_SVC_TESTS=%cert_svc_tests \
         -DCERT_SVC_EXAMPLES=%cert_svc_examples \
%endif
         -DCMAKE_BUILD_TYPE=%build_type \
         -DSYSTEMD_UNIT_DIR=%_unitdir

make %{?_smp_mflags}

%install
%make_install
%install_service sockets.target.wants cert-server.socket

mkdir -p %buildroot%cert_svc_pkcs12
mkdir -p %buildroot%cert_svc_transec_res
mkdir -p %buildroot%cert_svc_transec_usr
mkdir -p %buildroot%cert_svc_transec_global

touch %buildroot%cert_svc_db_path/certs-meta.db-journal
touch %buildroot%cert_svc_transec_bundle

ln -sf %TZ_SYS_CA_BUNDLE %buildroot%cert_svc_ca_bundle

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
%license LICENSE.OpenSSL
%_unitdir/cert-server.service
%_unitdir/cert-server.socket
%_unitdir/sockets.target.wants/cert-server.socket
%_libdir/libcert-svc-vcore.so.*
%_libdir/libcert-svc-transec.so.*
%bin_dir/cert-server
%dir %attr(-, %{user_name}, %{group_name}) %cert_svc_path
%dir %attr(-, %{user_name}, %{group_name}) %cert_svc_pkcs12
%dir %attr(-, %{user_name}, %{group_name}) %cert_svc_db_path
%attr(-, %{user_name}, %{group_name}) %cert_svc_ca_bundle
%attr(-, %{user_name}, %{group_name}) %cert_svc_db_path/certs-meta.db
%attr(-, %{user_name}, %{group_name}) %cert_svc_db_path/certs-meta.db-journal
%attr(-, %{user_name}, %{group_name}) %cert_svc_ro_path
%attr(-, %{user_name}, %{group_name}) %cert_svc_transec/*

%attr(755, root, root) %upgrade_script_path/202.cert-svc-db-upgrade.sh
%attr(755, root, root) %upgrade_script_path/203.cert-svc-disabled-certs-upgrade.sh
%upgrade_data_path/certs-meta.db

%files devel
%_includedir/*
%_libdir/pkgconfig/*
%_libdir/libcert-svc-vcore.so
%_libdir/libcert-svc-transec.so

%if 0%{?certsvc_test_build}
%files test
%bin_dir/cert-svc-test*
%cert_svc_tests
%_libdir/libcert-svc-validator-plugin.so

%attr(755, root, root) %upgrade_script_path/cert-svc-test-upgrade.sh
%upgrade_data_path/certs-meta-old.db

%bin_dir/cert-svc-example*
%cert_svc_examples

%endif
