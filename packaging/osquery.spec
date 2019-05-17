Name: osquery
Version: 0.0.0
Release: 0
License: Apache-2.0 and GPLv2
Summary: A SQL powered operating system instrumentation, monitoring framework.
Url: https://github.com/facebook/osquery
Group: Security/Libraries
Source0: file://%{name}-%{version}.tar.gz
Source1: %name.manifest
BuildRequires: gcc-c++
BuildRequires: make
BuildRequires: cmake
BuildRequires: glog-devel
BuildRequires: gflags-devel
BuildRequires: rocksdb
BuildRequires: snappy-devel
BuildRequires: zlib-devel
BuildRequires: bzip2-devel
BuildRequires: lz4-devel
BuildRequires: zstd-devel
BuildRequires: boost-devel
BuildRequires: pkgconfig(sqlite3)
BuildRequires: thrift
Requires: glog
Requires: gflag
Requires: rocksdb
Requires: snappy
Requires: zlib
Requires: bzip2
Requires: lz4
Requires: zstd
Requires: boost-system boost-thread boost-filesystem
Requires: sqlite3
Requires: thrift

%description
Osquery exposes an operating system as a high-performance relational database.
This allows you to write SQL-based queries to explore operating system data.

%files
%manifest %{name}.manifest

%prep
%setup -q
cp %SOURCE1 .

%build
%{!?build_type:%define build_type "RELEASE"}
%cmake . -DCMAKE_BUILD_TYPE=%{build_type}

make %{?jobs:-j%jobs}

%install
%make_install

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
%{_bindir}/osquery_status_tests
%{_bindir}/osquery_db_handle_tests
%{_bindir}/osquery_results_tests
%{_bindir}/osquery_registry_tests
%{_bindir}/osquery_config_tests
%{_bindir}/osquery_filesystem_tests
%{_bindir}/osquery_query_tests
%{_bindir}/osquery_sql_tests
%{_bindir}/osquery_sqlite_util_tests
%{_bindir}/osquery_scheduler_tests
%{_bindir}/osquery_test_util_tests
%{_bindir}/osquery_text_tests
%{_bindir}/osquery_logger_tests
%{_bindir}/osquery_conversions_tests
%{_bindir}/osquery_dispatcher_tests
%{_bindir}/osquery_events_tests
%{_bindir}/osquery_events_database_tests
%{_bindir}/osquery_inotify_tests
