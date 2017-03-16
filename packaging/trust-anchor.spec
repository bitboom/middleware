Name: trust-anchor
Version: 0.0.1
Release: 0
License: Apache-2.0
Source0: file://%{name}-%{version}.tar.gz
Summary: Trust Anchor API
Group: Security/Certificate Management
BuildRequires: gcc
BuildRequires: cmake

%description
The package provides trust-anchor which the application can assign
SSL root certificates for its HTTPS communication.

%files
%manifest %{name}.manifest
%license LICENSE

%prep
%setup -q

%build
%{!?build_type:%define build_type "RELEASE"}

%cmake . -DCMAKE_BUILD_TYPE=%{build_type}

make %{?_smp_mflags}

%install
%make_install

## Devel Package ###############################################################
%package devel
Summary: Trust Anchor API (development files)
Group:   Security/Development
Requires: %{name} = %{version}-%{release}

%description devel
The package provides Trust Anchor API development files.

%files devel
%{_includedir}/tanchor/error.h
%{_includedir}/tanchor/trust-anchor.h
%{_includedir}/tanchor/trust-anchor.hxx
