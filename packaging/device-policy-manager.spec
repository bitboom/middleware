Name:    device-policy-manager
Version: 0.0.1
Release: 0
License: Apache-2.0
Source0: file://%{name}-%{version}.tar.gz
Summary: Tizen Device Policy Manager
Group:   Security/Other
BuildRequires: gcc
BuildRequires: cmake
BuildRequires: pam-devel
BuildRequires: gettext-tools
BuildRequires: pkgconfig(klay)
BuildRequires: pkgconfig(glib-2.0)
BuildRequires: pkgconfig(libxml-2.0)
BuildRequires: pkgconfig(sqlite3)
BuildRequires: pkgconfig(bundle)
BuildRequires: pkgconfig(pkgmgr)
BuildRequires: pkgconfig(pkgmgr-info)
BuildRequires: pkgconfig(aul)
BuildRequires: pkgconfig(appsvc)
BuildRequires: pkgconfig(syspopup-caller)
BuildRequires: pkgconfig(deviced)
BuildRequires: pkgconfig(vconf)
BuildRequires: pkgconfig(vconf-internal-keys)
BuildRequires: pkgconfig(bluetooth-api)
BuildRequires: pkgconfig(capi-network-bluetooth)
BuildRequires: pkgconfig(libtzplatform-config)
BuildRequires: pkgconfig(security-privilege-manager)
BuildRequires: pkgconfig(capi-base-common)
BuildRequires: pkgconfig(capi-system-info)
BuildRequires: pkgconfig(capi-network-wifi)
BuildRequires: pkgconfig(capi-network-connection)
BuildRequires: pkgconfig(capi-network-bluetooth)
BuildRequires: pkgconfig(capi-system-system-settings)
BuildRequires: pkgconfig(notification)
BuildRequires: pkgconfig(key-manager)
BuildRequires: pkgconfig(cynara-client)
BuildRequires: pkgconfig(cynara-session)

%if "%{profile}" != "tv"
BuildRequires: pkgconfig(capi-location-manager)
BuildRequires: pkgconfig(auth-fw-admin)
%if "%{profile}" != "wearable"
BuildRequires: pkgconfig(krate)
%endif
%endif


%description
The device-policy-manager package provides a daemon which is responsible for
managing device policies.

%files
%manifest device-policy-manager.manifest
%defattr(644,root,root,755)
%attr(755,root,root) %{_bindir}/device-policy-manager
%attr(700,root,root) %{_bindir}/dpm-admin-cli
%attr(755,root,root) %{_bindir}/dpm-syspopup
%{_unitdir}/device-policy-manager.service
%{_unitdir}/multi-user.target.wants/device-policy-manager.service

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
         -DSCRIPT_INSTALL_DIR=%{_scriptdir} \
         -DSYSTEMD_UNIT_INSTALL_DIR=%{_unitdir} \
         -DDATA_INSTALL_DIR=%{TZ_SYS_DATA}/dpm \
         -DDB_INSTALL_DIR=%{TZ_SYS_DB} \
         -DRUN_INSTALL_DIR=%{TZ_SYS_RUN} \
         -DAPP_INSTALL_PREFIX="%{TZ_SYS_RO_APP}" \
         -DAPP_SHARE_PACKAGES_DIR="%{TZ_SYS_RO_PACKAGES}" \
         -DPAMD_INSTALL_DIR=/etc/pam.d

make %{?jobs:-j%jobs}

%install
%make_install
mkdir -p %{buildroot}/%{_unitdir}/multi-user.target.wants
ln -s ../device-policy-manager.service %{buildroot}/%{_unitdir}/multi-user.target.wants/device-policy-manager.service

%find_lang dpm-syspopup

%clean
rm -rf %{buildroot}

%preun

%postun

## Client Package #############################################################
%package -n libdpm
Summary: Tizen Device Policy Client library
Group: Development/Libraries
Requires: %{name} = %{version}-%{release}
Requires(post): /sbin/ldconfig
Requires(postun): /sbin/ldconfig

%description -n libdpm
The libdpm package contains the libraries needed to run DPM client.

%post -n libdpm -p /sbin/ldconfig

%postun -n libdpm -p /sbin/ldconfig

%files -n libdpm
%manifest device-policy-manager.manifest
%defattr(644,root,root,755)
%attr(755,root,root) %{_libdir}/libdpm.so.%{version}
%{_libdir}/libdpm.so.0

## Devel Package ##############################################################
%package -n libdpm-devel
Summary: Libraries and header files for device policy client development
Group: Development/Libraries
Requires: libdpm = %{version}-%{release}

%description -n libdpm-devel
The libdpm-devel package includes the libraries and header files necessary for
developing the DPM client program.

%files -n libdpm-devel
%manifest device-policy-manager.manifest
%defattr(644,root,root,755)
%{_libdir}/libdpm.so
%{_includedir}/dpm
%{_libdir}/pkgconfig/dpm.pc

## Test Package ##############################################################
%package -n dpm-testcases
Summary: Device Policy Manager test cases
Group: Development/Libraries
Requires: libdpm = %{version}-%{release}

%description -n dpm-testcases
Testcases for device policy manager and device policy client

%files -n dpm-testcases
%defattr(644,root,root,755)
%attr(755,root,root) %{_bindir}/dpm-api-tests

## DPM Syspopup Package ######################################################
%package -n org.tizen.dpm-syspopup
Summary: Tizen DPM system popup Interface
Group: Security/Other
BuildRequires: pkgconfig(elementary)
BuildRequires: pkgconfig(capi-appfw-application)
BuildRequires: pkgconfig(capi-system-system-settings)
BuildRequires: pkgconfig(capi-ui-efl-util)

%description -n org.tizen.dpm-syspopup
Tizen DPM system popup interface package

%files -n org.tizen.dpm-syspopup -f dpm-syspopup.lang
%defattr(-,root,root,-)
%manifest tools/syspopup/org.tizen.dpm-syspopup.manifest
%{TZ_SYS_RO_APP}/org.tizen.dpm-syspopup/bin/*
%{TZ_SYS_RO_PACKAGES}/org.tizen.dpm-syspopup.xml
/usr/share/icons/default/small/org.tizen.dpm-syspopup.png
%{TZ_SYS_RO_APP}/org.tizen.dpm-syspopup/res/images/*
