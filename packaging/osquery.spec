Name: osquery
Version: 0.0.0
Release: 0
License: Apache-2.0 and GPLv2 and MIT
Summary: A SQL powered operating system instrumentation, monitoring framework.
Url: https://github.com/facebook/osquery
Group: Security/Libraries
Source0: file://%{name}-%{version}.tar.gz
Source1: %name.manifest
### Core dependencies
BuildRequires: gcc-c++
BuildRequires: make
BuildRequires: cmake
BuildRequires: glog-devel
BuildRequires: gflags-devel
BuildRequires: rocksdb-devel
BuildRequires: snappy-devel
BuildRequires: zlib-devel
BuildRequires: bzip2-devel
BuildRequires: lz4-devel
BuildRequires: zstd-devel
BuildRequires: boost-devel
BuildRequires: readline-devel
BuildRequires: pkgconfig(libprocps)
BuildRequires: pkgconfig(libsystemd)
BuildRequires: pkgconfig(openssl)
BuildRequires: iptables-devel
BuildRequires: python-jinja2
BuildRequires: libuuid-devel
Requires: glog
Requires: gflag
Requires: rocksdb
Requires: snappy
Requires: zlib
Requires: bzip2
Requires: lz4
Requires: zstd
Requires: boost-regex boost-system boost-thread boost-filesystem
Requires: libreadline
Requires: procps-ng
Requires: libsystemd
Requires: iptables

%global user_name        security_fw
%global group_name       security_fw
%global smack_label      System

%global dpm_base         %{TZ_SYS_DATA}/dpm
%global dpm_event        %{dpm_base}/event
%global dpm_plugins      %{_libdir}/dpm/plugins


%description
Osquery exposes an operating system as a high-performance relational database.
This allows you to write SQL-based queries to explore operating system data.

%files
%manifest %{name}.manifest
%license MIT-LICENSE

%prep
%setup -q
cp %SOURCE1 .

%build
%{!?build_type:%define build_type "RELEASE"}

%if %{build_type} == "DEBUG" || %{build_type} == "PROFILING" || %{build_type} == "CCOV"
	CFLAGS="$CFLAGS -Wp,-U_FORTIFY_SOURCE"
	CXXFLAGS="$CXXFLAGS -Wp,-U_FORTIFY_SOURCE"
%endif

%cmake . -DCMAKE_BUILD_TYPE=%{build_type} \
		 -DOSQUERY_BUILD_VERSION=%{version} \
		 -DGBS_BUILD="TRUE" \
		 -DVERSION=%{version} \
		 -DUSER_NAME=%{user_name} \
		 -DGROUP_NAME=%{group_name} \
		 -DSMACK_LABEL=%{smack_label} \
		 -DSCRIPT_INSTALL_DIR=%{_scriptdir} \
		 -DSYSTEMD_UNIT_INSTALL_DIR=%{_unitdir} \
		 -DDATA_INSTALL_DIR=%{dpm_base} \
		 -DEVENT_CONFIGURE_DIR=%{dpm_event} \
		 -DPLUGIN_INSTALL_DIR=%{dpm_plugins} \
		 -DDB_INSTALL_DIR=%{TZ_SYS_DB} \
		 -DRUN_INSTALL_DIR=%{TZ_SYS_RUN} \
		 -DAPP_INSTALL_PREFIX="%{TZ_SYS_RO_APP}" \
		 -DAPP_SHARE_PACKAGES_DIR="%{TZ_SYS_RO_PACKAGES}" \
		 -DPAMD_INSTALL_DIR=/etc/pam.d \
		 -DDBUS_CONFIGURE_DIR=/etc/dbus-1/system.d \
		 -DDBUS_SERVICE_DIR=/usr/share/dbus-1/system-services


make %{?jobs:-j%jobs}

%install
%make_install
mkdir -p %{buildroot}%{dpm_event}
mkdir -p %{buildroot}%{dpm_plugins}
mkdir -p %{buildroot}/%{_unitdir}/multi-user.target.wants
mkdir -p %{buildroot}/%{_unitdir}/sockets.target.wants
ln -s ../src/policyd/device-policy-manager.service %{buildroot}/%{_unitdir}/multi-user.target.wants/device-policy-manager.service
ln -s ../src/policyd/device-policy-manager.socket %{buildroot}/%{_unitdir}/sockets.target.wants/device-policy-manager.socket

%find_lang dpm-syspopup

%clean
rm -rf %{buildroot}

## Test Package ##############################################################
%package test 
Summary: Osquery test
Group: Security/Testing
BuildRequires: gtest-devel
Requires: gtest

%description test 
Testcases for osquery 

%files test
%manifest %{name}.manifest
%{_bindir}/osquery-test
%{_bindir}/apix-test


## DPM Policyd ###############################################################
%package policyd
Summary: Tizen Device Policy Manager
Group:   Security/Other
BuildRequires: gcc
BuildRequires: cmake
BuildRequires: gettext-tools
BuildRequires: pkgconfig(klay)
BuildRequires: pkgconfig(glib-2.0)
BuildRequires: pkgconfig(sqlite3)
BuildRequires: pkgconfig(bundle)
BuildRequires: pkgconfig(pkgmgr)
BuildRequires: pkgconfig(pkgmgr-info)
BuildRequires: pkgconfig(aul)
BuildRequires: pkgconfig(appsvc)
BuildRequires: pkgconfig(syspopup-caller)
BuildRequires: pkgconfig(deviced)
BuildRequires: pkgconfig(vconf)
BuildRequires: pkgconfig(vconf-internal-keys)
BuildRequires: pkgconfig(notification)
BuildRequires: pkgconfig(cynara-client)
BuildRequires: pkgconfig(cynara-session)
BuildRequires: pkgconfig(libtzplatform-config)
BuildRequires: pkgconfig(security-privilege-manager)
BuildRequires: pkgconfig(capi-system-system-settings)
BuildRequires: pkgconfig(capi-base-common)

%description policyd
The device-policy-manager package provides a daemon which is responsible for
managing device policies.

%files policyd
%manifest device-policy-manager.manifest
%defattr(644,root,root,755)
%attr(700,root,root) %{_bindir}/dpm-admin-cli
%attr(755,root,root) %{_bindir}/dpm-syspopup
%attr(755,root,root) %{_bindir}/dpm-storage-builder
%attr(755,root,root) %{_bindir}/device-policy-syspopup
%attr(-,%{user_name},%{group_name}) %{dpm_base}
%attr(-,%{user_name},%{group_name}) %{dpm_event}
%attr(-,%{user_name},%{group_name}) %{dpm_plugins}
%{_unitdir}/device-policy-manager.service
%{_unitdir}/device-policy-manager.socket
%{_unitdir}/sockets.target.wants/device-policy-manager.socket
%{_unitdir}/multi-user.target.wants/device-policy-manager.service
%{_unitdir}/device-policy-syspopup.service
/etc/dbus-1/system.d/org.tizen.dpm.syspopup.conf
/usr/share/dbus-1/system-services/org.tizen.dpm.syspopup.service

## Devel Package ##############################################################
%package policyd-devel
Summary: Libraries and header files for device policy client development
Group: Development/Libraries
Requires: policyd = %{version}-%{release}
BuildRequires: pkgconfig(bundle)
BuildRequires: pkgconfig(pkgmgr)
BuildRequires: pkgconfig(pkgmgr-info)
BuildRequires: pkgconfig(aul)
BuildRequires: pkgconfig(appsvc)
BuildRequires: pkgconfig(syspopup-caller)

%description policyd-devel
The libdpm-pil-devel package includes the libraries and header files necessary for
developing the device policy module.

%files policyd-devel
#%manifest device-policy-client.manifest
%defattr(644,root,root,755)
%{_includedir}/dpm
%{_libdir}/pkgconfig/dpm-pil.pc

## DPM Syspopup Package ######################################################
%package -n org.tizen.dpm-syspopup
Summary: Tizen DPM system popup Interface
Group: Security/Other
BuildRequires: pkgconfig(elementary)
BuildRequires: pkgconfig(capi-appfw-application)
BuildRequires: pkgconfig(capi-system-system-settings)
BuildRequires: pkgconfig(capi-ui-efl-util)
BuildRequires: pkgconfig(capi-system-info)
%description -n org.tizen.dpm-syspopup
Tizen DPM system popup interface package

%files -n org.tizen.dpm-syspopup -f dpm-syspopup.lang
%defattr(-,root,root,-)
%manifest src/policyd/tools/syspopup/org.tizen.dpm-syspopup.manifest
%{TZ_SYS_RO_APP}/org.tizen.dpm-syspopup/bin/*
%{TZ_SYS_RO_PACKAGES}/org.tizen.dpm-syspopup.xml
/usr/share/icons/default/small/org.tizen.dpm-syspopup.png
%{TZ_SYS_RO_APP}/org.tizen.dpm-syspopup/res/images/*

## Test Package ###############################################################
%package policyd-test
Summary: Tizen DPM internal test
Group: Security/Development
Requires: policyd = %{version}-%{release}
BuildRequires: pkgconfig(klay)

%description policyd-test
Testcases for device policy manager

%files policyd-test
%{_bindir}/policyd-test

## DPM Plugins - ############################################################
%package policyd-plugins
Summary: DPM plugins
Group: Security/Other
## Common
BuildRequires: pkgconfig(buxton2)
BuildRequires: pkgconfig(dlog)
#BuildRequires: pkgconfig(dpm-pil)
BuildRequires: pkgconfig(capi-system-info)
BuildRequires: pkgconfig(capi-base-common)

## Bluetooth
BuildRequires: pkgconfig(bluetooth-api)
BuildRequires: pkgconfig(capi-network-bluetooth)

## Wifi
BuildRequires: pkgconfig(capi-network-wifi-manager)
BuildRequires: pkgconfig(capi-network-connection)

%description policyd-plugins
Provides plugins for device policy manager

%files policyd-plugins
%manifest packaging/%{name}-plugins.manifest
%{_libdir}/dpm/plugins/bluetooth
#%{_libdir}/dpm/plugins/usb
#%{_libdir}/dpm/plugins/wifi
