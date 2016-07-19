%define keepstatic 1
Name:    klay
Version: 0.0.1
Release: 0
License: Apache-2.0
Source0: file://%{name}-%{version}.tar.gz
Summary: Tizen Klay static library and header files
Group:   Development/Libraries
BuildRequires: gcc
BuildRequires: cmake
BuildRequires: pkgconfig(glib-2.0)
BuildRequires: pkgconfig(sqlite3)
BuildRequires: pkgconfig(libxml-2.0)
BuildRequires: pkgconfig(libtzplatform-config)

%description
The klay package provides a static library and header files
which can be used for many servers and clients

%files
%defattr(644,root,root,755)
%{_libdir}/*.a
%{_libdir}/pkgconfig/*.pc
%{_includedir}/klay

%prep
%setup -q

%build
%{!?build_type:%define build_type "RELEASE"}

%if %{build_type} == "DEBUG" || %{build_type} == "PROFILING" || %{build_type} == "CCOV"
	CFLAGS="$CFLAGS -Wp,-U_FORTIFY_SOURCE"
	CXXFLAGS="$CXXFLAGS -Wp,-U_FORTIFY_SOURCE"
%endif

%{!?profile:%define profile "mobile"}

%cmake . -DVERSION=%{version} \
         -DCMAKE_BUILD_TYPE=%{build_type} \
         -DTIZEN_PROFILE_NAME=%{profile} \
         -DTEST_DATA_INSTALL_DIR=%{TZ_SYS_DATA}/klay-test \

make %{?jobs:-j%jobs}

%install
%make_install

%clean
rm -rf %{buildroot}

%preun

%postun

## Test Package ##############################################################
%package -n klay-test
Summary: Klay test
Group: Development/Libraries

%description -n klay-test
Testcases for klay library

%files -n klay-test
%defattr(755,root,root,755)
%{_bindir}/klay-test
%{TZ_SYS_DATA}/klay-test/test-proc.sh
%defattr(-,root,root,-)
%{TZ_SYS_DATA}/klay-test/sample-policy.xml
