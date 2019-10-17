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
%{vist_script_dir}/*.sql
%dir %attr(-, %{user_name}, %{group_name}) %{vist_db_dir}

## Test Package ##############################################################
%package test 
Summary: Osquery test
Group: Security/Testing
BuildRequires: gtest-devel
Requires: gtest

%description test 
Testcases for osquery 

%files test
%{_bindir}/osquery-test
%{_bindir}/apix-test
%{_bindir}/policyd-test

## ViST Plugins - ###########################################################
%package policyd-plugins
Summary: ViST plugins
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
%{vist_plugin_dir}/bluetooth
