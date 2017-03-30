Name: trust-anchor
Version: 0.0.1
Release: 0
License: Apache-2.0
Source0: file://%{name}-%{version}.tar.gz
Summary: Trust Anchor API
Group: Security/Certificate Management
BuildRequires: gcc
BuildRequires: cmake
BuildRequires: coreutils
BuildRequires: pkgconfig(klay)
BuildRequires: pkgconfig(openssl)
BuildRequires: pkgconfig(libtzplatform-config)
BuildRequires: ca-certificates
BuildRequires: ca-certificates-devel
Requires(post): /sbin/ldconfig
Requires(postun): /sbin/ldconfig

%global lib_name        tanchor
%global user_name       security_fw
%global group_name      security_fw
%global smack_label     System

%global tanchor_base    %{TZ_SYS_DATA}/%{lib_name}
%global tanchor_usr     %{tanchor_base}/usr
%global tanchor_global  %{tanchor_base}/global
%global tanchor_bundle  %{tanchor_base}/ca-bundle.pem
%global tanchor_test    %{tanchor_base}/test

%description
The package provides trust-anchor which the application can assign
SSL root certificates for its HTTPS communication.

%files
%manifest %{name}.manifest
%license LICENSE
%{_libdir}/lib%{lib_name}.so.0
%{_libdir}/lib%{lib_name}.so.%{version}
%dir %attr(770, %{user_name}, %{group_name}) %{tanchor_base}
%dir %attr(-, %{user_name}, %{group_name}) %{tanchor_usr}
%dir %attr(-, %{user_name}, %{group_name}) %{tanchor_global}
%attr(-, %{user_name}, %{group_name}) %{tanchor_bundle}

%prep
%setup -q

%build
%{!?build_type:%define build_type "RELEASE"}

%if %{build_type} == "DEBUG" || %{build_type} == "PROFILING" || %{build_type} == "CCOV"
	CFLAGS="$CFLAGS -Wp,-U_FORTIFY_SOURCE"
	CXXFLAGS="$CXXFLAGS -Wp,-U_FORTIFY_SOURCE"
%endif

%cmake . -DCMAKE_BUILD_TYPE=%{build_type} \
		 -DLIB_NAME=%{lib_name} \
		 -DLIB_VERSION=%{version} \
		 -DUSER_NAME=%{user_name} \
		 -DGROUP_NAME=%{group_name} \
		 -DSMACK_LABEL=%{smack_label} \
		 -DTANCHOR_BASE=%{tanchor_base} \
		 -DTANCHOR_USR=%{tanchor_usr} \
		 -DTANCHOR_GLOBAL=%{tanchor_global} \
		 -DTANCHOR_BUNDLE=%{tanchor_bundle} \
		 -DTANCHOR_TEST=%{tanchor_test} \
		 -DTZ_SYS_CA_CERTS=%{TZ_SYS_CA_CERTS} \
		 -DTZ_SYS_CA_BUNDLE=%{TZ_SYS_CA_BUNDLE} \
		 -DTZ_SYS_RO_CA_CERTS=%{TZ_SYS_RO_CA_CERTS} \
		 -DTZ_SYS_RO_CA_BUNDLE=%{TZ_SYS_RO_CA_BUNDLE}

make %{?_smp_mflags}

%install
%make_install

mkdir -p %{buildroot}%{tanchor_usr}
mkdir -p %{buildroot}%{tanchor_global}

touch %{buildroot}%{tanchor_bundle}

%post -p /sbin/ldconfig

%postun -p /sbin/ldconfig

## Devel Package ###############################################################
%package devel
Summary: Trust Anchor API (development files)
Group:   Security/Development
Requires: %{name} = %{version}-%{release}

%description devel
The package provides Trust Anchor API development files.

%files devel
%{_includedir}/%{lib_name}/error.h
%{_includedir}/%{lib_name}/trust-anchor.h
%{_includedir}/%{lib_name}/trust-anchor.hxx
%{_libdir}/lib%{lib_name}.so
%{_libdir}/pkgconfig/%{lib_name}.pc

## Test Package ##############################################################
%package -n trust-anchor-test
Summary: Trust Anchor API test
Group: Development/Libraries

%description -n trust-anchor-test
Testcases for trust anchor library

%files -n trust-anchor-test
%{_bindir}/%{lib_name}-test-installer
%{_bindir}/%{lib_name}-test-launcher
%{_bindir}/%{lib_name}-test-clauncher
%{_bindir}/%{lib_name}-test-internal
%{TZ_SYS_DATA}/%{lib_name}/test
%{TZ_SYS_DATA}/%{lib_name}/test/certs
%{TZ_SYS_DATA}/%{lib_name}/test/script
