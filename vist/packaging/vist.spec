Name: vist
Version: 0.1.4
Release: 0
License: Apache-2.0
Summary: ViST(Virtual Security Table) is an unified security API platform based virtual tables.
Url: https://github.com/facebook/osquery
Group: Security/Libraries
Source0: file://%{name}-%{version}.tar.gz
Source1: %name.manifest
BuildRequires: gcc-c++
BuildRequires: make
BuildRequires: cmake
# osquery BRs
BuildRequires: glog-devel
BuildRequires: boost-devel
# vist BRs
BuildRequires: gflags-devel
BuildRequires: pkgconfig(dlog)
BuildRequires: pkgconfig(sqlite3)
BuildRequires: pkgconfig(libtzplatform-config)
BuildRequires: pkgconfig(libsystemd)
Requires: glog
Requires: gflag
Requires: boost-regex boost-system boost-thread boost-filesystem
Requires: libsystemd

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
%global vist_table_dir %{vist_ro_dir}/table
%global vist_script_dir %{vist_ro_dir}/script

%description
ViST provides unified interface of security functions.
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

%if 0%{?gcov:1}
export CFLAGS+=" -fprofile-arcs -ftest-coverage"
export CXXFLAGS+=" -fprofile-arcs -ftest-coverage"
export FFLAGS+=" -fprofile-arcs -ftest-coverage"
export LDFLAGS+=" -lgcov"
%endif

%cmake . -DCMAKE_BUILD_TYPE=%{build_type} \
		 -DVERSION=%{version} \
		 -DOSQUERY_VERSION=%{osquery_version} \
		 -DGBS_BUILD="TRUE" \
		 -DUSER_NAME=%{user_name} \
		 -DGROUP_NAME=%{group_name} \
		 -DSMACK_LABEL=%{smack_label} \
		 -DDEFAULT_POLICY_ADMIN=vist-cli \
		 -DDB_INSTALL_DIR:PATH=%{vist_db_dir} \
		 -DPLUGIN_INSTALL_DIR:PATH=%{vist_plugin_dir} \
		 -DTABLE_INSTALL_DIR:PATH=%{vist_table_dir} \
		 -DSCRIPT_INSTALL_DIR:PATH=%{vist_script_dir} \
		 -DSYSTEMD_UNIT_DIR:PATH=%{_unitdir} \
		 -DBUILD_GCOV=%{?gcov:1}%{!?gcov:0}

make %{?jobs:-j%jobs}

%install
%make_install
mkdir -p %{buildroot}/%{vist_db_dir}
mkdir -p %{buildroot}/%{vist_table_dir}
mkdir -p %{buildroot}/%{vist_plugin_dir}
mkdir -p %{buildroot}/%{vist_script_dir}

cp data/script/*.sql %{buildroot}/%{vist_script_dir}

%install_service sockets.target.wants %{name}.socket

%clean
rm -rf %{buildroot}

%post
rm -f %{vist_db_dir}/.%{name}.db*

systemctl daemon-reload
if [ $1 = 1 ]; then
	systemctl start %{name}.socket
elif [ $1 = 2 ]; then
	systemctl stop %{name}.socket
	systemctl stop %{name}.service
	systemctl restart %{name}.socket
fi

%files
%manifest %{name}.manifest
%license LICENSE-Apache-2.0
%attr(-, %{user_name}, %{group_name}) %{_bindir}/vist-cli
%attr(-, %{user_name}, %{group_name}) %{_bindir}/vistd
%{_libdir}/libvist-rmi.so
%{_unitdir}/vist.service
%{_unitdir}/vist.socket
%{_unitdir}/sockets.target.wants/vist.socket
%{vist_script_dir}/*.sql
%dir %attr(-, %{user_name}, %{group_name}) %{vist_db_dir}
%dir %attr(-, %{user_name}, %{group_name}) %{vist_table_dir}
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
%manifest packaging/%{name}-test.manifest
%{_bindir}/osquery-test
%attr(4755 %{user_name}, %{group_name}) %{_bindir}/vist-test
%dir %attr(-, %{user_name}, %{group_name}) %{vist_table_dir}
%attr(-, %{user_name}, %{group_name}) %{vist_table_dir}/libvist-table-sample.so
%attr(-, %{user_name}, %{group_name}) %{vist_plugin_dir}/libtest-plugin.so

## ViST Plugins #############################################################
%package plugins
Summary: Virtaul Security Table (policy modules)
Group: Security/Other
## Bluetooth
BuildRequires: pkgconfig(bluetooth-api)
BuildRequires: pkgconfig(capi-network-bluetooth)

## Wifi
BuildRequires: pkgconfig(klay)
BuildRequires: pkgconfig(capi-network-wifi-manager)
BuildRequires: pkgconfig(capi-network-connection)
Requires: klay

%description plugins
Provides plugins for controlling policies.

%pre plugins
#rm -f %{vist_plugin_dir}/bluetooth
#rm -f %{vist_plugin_dir}/wifi

%files plugins
%manifest packaging/%{name}-plugins.manifest
%attr(-, %{user_name}, %{group_name}) %{vist_table_dir}/libvist-bluetooth-policy.so
%attr(4755 %{user_name}, %{group_name}) %{_bindir}/vist-bluetooth-policy-test
