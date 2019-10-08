Name: osquery
Version: 0.0.0
Release: 0
License: Apache-2.0 and GPLv2 and MIT
Summary: A SQL powered operating system instrumentation, monitoring framework.
Url: https://github.com/facebook/osquery
Group: Security/Libraries
Source0: file://%{name}-%{version}.tar.gz
Source1: %name.manifest
BuildRequires: gcc-c++
BuildRequires: make
BuildRequires: cmake
# osquery BRs
BuildRequires: glog-devel
BuildRequires: gflags-devel
BuildRequires: boost-devel
BuildRequires: python-jinja2
BuildRequires: libuuid-devel
BuildRequires: pkgconfig(libprocps)
BuildRequires: pkgconfig(libsystemd)
BuildRequires: pkgconfig(openssl)
# policyd BRs
BuildRequires: pkgconfig(klay)
BuildRequires: pkgconfig(libtzplatform-config)
Requires: glog
Requires: gflag
Requires: boost-regex boost-system boost-thread boost-filesystem
Requires: procps-ng
Requires: libsystemd

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
%{_bindir}/policyd-test

## DPM Plugins - ############################################################
%package policyd-plugins
Summary: DPM plugins
Group: Security/Other
## Common
BuildRequires: pkgconfig(buxton2)
BuildRequires: pkgconfig(dlog)
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
