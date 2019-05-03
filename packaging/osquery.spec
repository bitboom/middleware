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
BuildRequires: gtest-devel

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
Requires: gtest

%description test 
Testcases for osquery 

%files test
%manifest %{name}.manifest
%{_bindir}/osquery_status_test
