Name: vist
Version: 0.0.0
Release: 0
License: Apache-2.0 and GPLv2 and MIT
Summary: ViST(Virtual Security Table) is a security monitoring framework using SQL query.
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
# vist BRs
BuildRequires: pkgconfig(dlog)
BuildRequires: pkgconfig(klay)
BuildRequires: pkgconfig(libtzplatform-config)
Requires: glog
Requires: gflag
Requires: boost-regex boost-system boost-thread boost-filesystem
Requires: procps-ng
Requires: libsystemd
Requires: klay

%global osquery_version 4.0.0

%global user_name   security_fw
%global group_name  security_fw
%global smack_label System

%global ro_dir %{_datadir}
%global rw_dir %{TZ_SYS_DATA}

%global vist_ro_dir %{ro_dir}/vist
%global vist_rw_dir %{rw_dir}/vist

%global vist_db_dir     %{vist_rw_dir}/db
%global vist_plugin_dir %{vist_ro_dir}/plugin
%global vist_script_dir %{vist_ro_dir}/script

%description
ViST provides APIs for monitoring and controlling security resources.
ViST views security resources as virtual tables and manipulates them through SQL queries.
ViST adopts a plug-in architecture and uses osquery as the query analysis engine.

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
		 -DVERSION=%{version} \
		 -DOSQUERY_VERSION=%{osquery_version} \
		 -DGBS_BUILD="TRUE" \
		 -DUSER_NAME=%{user_name} \
		 -DGROUP_NAME=%{group_name} \
		 -DSMACK_LABEL=%{smack_label} \
		 -DDEFAULT_ADMIN_PATH=%{_bindir}/vist-cli \
		 -DDB_INSTALL_DIR:PATH=%{vist_db_dir} \
		 -DPLUGIN_INSTALL_DIR:PATH=%{vist_plugin_dir} \
		 -DSCRIPT_INSTALL_DIR:PATH=%{vist_script_dir}

make %{?jobs:-j%jobs}

%install
%make_install
mkdir -p %{buildroot}/%{vist_db_dir}
mkdir -p %{buildroot}/%{vist_plugin_dir}
mkdir -p %{buildroot}/%{vist_script_dir}

cp data/script/*.sql %{buildroot}/%{vist_script_dir}

%clean
rm -rf %{buildroot}

%files
%manifest %{name}.manifest
%license LICENSE-Apache-2.0
%license LICENSE-GPL-2.0
%license LICENSE-MIT
%{_bindir}/vist-cli
%{_bindir}/vistd
%{vist_script_dir}/*.sql
%dir %attr(-, %{user_name}, %{group_name}) %{vist_db_dir}
%dir %attr(-, %{user_name}, %{group_name}) %{vist_plugin_dir}
%dir %attr(-, %{user_name}, %{group_name}) %{vist_script_dir}

## Test Package ##############################################################
%package test 
Summary: Virtaul Security Table (unit test)
Group: Security/Testing
BuildRequires: gtest-devel
Requires: gtest

%description test 
Provides internal testcases for ViST implementation.

%files test
%{_bindir}/osquery-test
%{_bindir}/vist-test

## ViST Plugins - ###########################################################
%package plugins
Summary: Virtaul Security Table (policy modules)
Group: Security/Other
## Common
BuildRequires: pkgconfig(buxton2)
BuildRequires: pkgconfig(dlog)
BuildRequires: pkgconfig(capi-system-info)
BuildRequires: pkgconfig(capi-base-common)
BuildRequires: pkgconfig(klay)

## Bluetooth
BuildRequires: pkgconfig(bluetooth-api)
BuildRequires: pkgconfig(capi-network-bluetooth)

## Wifi
BuildRequires: pkgconfig(capi-network-wifi-manager)
BuildRequires: pkgconfig(capi-network-connection)
Requires: klay

%description plugins
Provides plugins for controlling policies.

%files plugins
%manifest packaging/%{name}-plugins.manifest
%{vist_plugin_dir}/bluetooth
%{vist_plugin_dir}/wifi
