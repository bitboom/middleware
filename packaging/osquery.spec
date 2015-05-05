Name: osquery
Version: 1.4.7
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
BuildRequires: rocksdb-devel
BuildRequires: snappy-devel
BuildRequires: zlib-devel
BuildRequires: bzip2-devel
BuildRequires: lz4-devel
BuildRequires: zstd-devel
BuildRequires: boost-devel
BuildRequires: thrift
BuildRequires: readline-devel
BuildRequires: pkgconfig(libprocps)
BuildRequires: pkgconfig(libsystemd)
BuildRequires: pkgconfig(openssl)
BuildRequires: iptables-devel
BuildRequires: python-jinja2
Requires: glog
Requires: gflag
Requires: rocksdb
Requires: snappy
Requires: zlib
Requires: bzip2
Requires: lz4
Requires: zstd
Requires: boost-regex boost-system boost-thread boost-filesystem
Requires: thrift
Requires: libreadline
Requires: procps-ng
Requires: libsystemd
Requires: iptables

%description
Osquery exposes an operating system as a high-performance relational database.
This allows you to write SQL-based queries to explore operating system data.

%files
%manifest %{name}.manifest
%{_bindir}/osqueryi
%{_bindir}/osqueryd

%prep
%setup -q
cp %SOURCE1 .

%build
%{!?build_type:%define build_type "RELEASE"}
%cmake . -DCMAKE_BUILD_TYPE=%{build_type} \
		 -DOSQUERY_BUILD_VERSION=%{version}

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
%{_bindir}/osquery-test
