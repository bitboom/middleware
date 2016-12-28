Name:    device-policy-manager
Version: 0.0.1
Release: 0
License: Apache-2.0
Source0: %{name}-%{version}.tar.gz
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
BuildRequires: sed

# The usage of "profile" in the following expressions is for
# backward compatility and GBM build optimization only. They will be
# ignored in Tizen 4.0 Unified Build Environment.
%if "%{?profile}" != "tv"
BuildRequires: pkgconfig(capi-location-manager)
BuildRequires: pkgconfig(auth-fw-admin)
%if "%{?profile}" != "wearable"
BuildRequires: pkgconfig(krate)
%endif
%endif
Requires:      %{name}-compat = %{version}-%{release}
# The usage of "profile" in the following expressions is for
# backward compatility and GBM build optimization only. They will be
# ignored in Tizen 4.0 Unified Build Environment.
%if "%{?profile}" == "tv"
# Obsolete Build System Support
Requires:      %{name}-minimal
%else
%if "%{?profile}" == "wearable"
# Obsolete Build System Support
Requires:      %{name}-ext-password
%else
# 4.0 Build System Will Always Profile-Agnostic
Recommends:    %{name}-full
%endif
%endif

%description
The device-policy-manager package provides a daemon which is responsible for
managing device policies.

%files
%manifest device-policy-manager.manifest
%defattr(644,root,root,755)
%attr(700,root,root) %{_bindir}/dpm-admin-cli
# dpm-admin-cli: no differences per profile
%attr(755,root,root) %{_bindir}/dpm-syspopup
%attr(755,root,root) %{_bindir}/dpm-preference
%{_unitdir}/device-policy-manager.service
%{_unitdir}/multi-user.target.wants/device-policy-manager.service

%if "%{?profile}" != "tv" && "%{?profile}" != "wearable"
%package full
Summary: Device-policy-manager binaries with full features (common/mobile/ivi)
Provides: %{name}-compat = %{version}-%{release}
Provides: %{name}-profile_common = %{version}-%{release}
Provides: %{name}-profile_mobile = %{version}-%{release}
Provides: %{name}-profile_ivi = %{version}-%{release}
Conflicts: %{name}-minimal
Conflicts: %{name}-ext-password
%description full
The device-policy-manager server binary with full features (common/mobile/ivi).
%files full
%attr(755,root,root) %{_bindir}/device-policy-manager
%endif

%if "%{?profile}" != "tv"
%package ext-password
Summary: Device-policy-manager binaries with min+password features (wearable)
Provides: %{name}-compat = %{version}-%{release}
Provides: %{name}-profile_wearable = %{version}-%{release}
Conflicts: %{name}-minimal
Conflicts: %{name}-full
%description ext-password
The device-policy-manager server binary with min+password features (wearable)
%post ext-password
ln -sf device-policy-manager.ext-pwd %{_bindir}/device-policy-manager
%preun ext-password
rm device-policy-manager
%files ext-password
%attr(755,root,root) %{_bindir}/device-policy-manager.ext-pwd
%endif

%package minimal
Summary: Device-policy-manager binaries with minimal features (tv)
Provides: %{name}-compat = %{version}-%{release}
Provides: %{name}-profile_tv = %{version}-%{release}
Conflicts: %{name}-ext-password
Conflicts: %{name}-full
%description minimal
The device-policy-manager server binary with minimal features (tv)
%post minimal
ln -sf device-policy-manager.minimal %{_bindir}/device-policy-manager
%preun minimal
rm device-policy-manager
%files minimal
%attr(755,root,root) %{_bindir}/device-policy-manager.minimal

%prep
%setup -q

%build
# The usage of "profile" in the following expressions is for
# backward compatility and GBM build optimization only. They will be
# ignored in Tizen 4.0 Unified Build Environment.
%if "%{?profile}" == "tv"
sed -i "s|-ldpm-ext-password||" libs/dpm.pc.in
sed -i "s|-ldpm-ext-krate_zone||" libs/dpm.pc.in
%endif
%if "%{?profile}" == "wearable"
sed -i "s|-ldpm-ext-krate_zone||" libs/dpm.pc.in
%endif

%{!?build_type:%define build_type "RELEASE"}

%if %{build_type} == "DEBUG" || %{build_type} == "PROFILING" || %{build_type} == "CCOV"
	CFLAGS="$CFLAGS -Wp,-U_FORTIFY_SOURCE"
	CXXFLAGS="$CXXFLAGS -Wp,-U_FORTIFY_SOURCE"
%endif

# Add -DNO_EXT_KRATE_ZONE for wearable, tv GBM only
# Add -DNO_EXT_PASSWORD for tv GBM only (implied no bluetooth/location as well)
#     -DNO_EXT_PASSWORD requires -DNO_EXT_KRATE_ZONE. (otherwise: unexpected behavior)
# The usage of "profile" in the following expressions is for
# backward compatility and GBM build optimization only. They will be
# ignored in Tizen 4.0 Unified Build Environment.
%cmake . -DVERSION=%{version} \
         -DCMAKE_BUILD_TYPE=%{build_type} \
%if "%{profile}" == "tv"
	 -DNO_EXT_PASSWORD=on \
	 -DNO_EXT_KRATE_ZONE=on \
%endif
%if "%{profile}" == "wearable"
	 -DNO_EXT_KRATE_ZONE=on \
%endif
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

# Enforce devel pacakge to be independent to profile.
# This is a requirement of Tizen 4.0 (Configurability & Building Blocks)
# Do not worry for SDK rootstraping. You need to supply an XML that states
# public API sets for each profile anyway, which filters CAPI headers for SDK.
install -m 0644 libs/dpm/password.h %{buildroot}%{_includedir}/dpm/
install -m 0644 libs/dpm/password_internal.h %{buildroot}%{_includedir}/dpm/
install -m 0644 libs/dpm/zone.h %{buildroot}%{_includedir}/dpm/

%find_lang dpm-syspopup

%clean
rm -rf %{buildroot}

%preun

%postun

## Client Package #############################################################
%package -n libdpm
Summary: Metapacakge for Tizen Device Policy Client Library
Group: Development/Libraries
Requires: libdpm-compat = %{version}-%{release}
%if "%{?profile}" != "tv" && "%{?profile}" != "wearable" && "%{?profile}" != "mobile"
Recommends: libdpm-full = %{version}-%{release}
%endif
%if "%{?profile}" == "tv"
Requires: libdpm-profile_tv = %{version}-%{release}
%endif
%if "%{?profile}" == "wearable"
Requires: libdpm-profile_wearable = %{version}-%{release}
%endif
%if "%{?profile}" == "mobile"
Requires: libdpm-profile_mobile = %{version}-%{release}
%endif

%description -n libdpm
Metapackage for Tizen Device Policy Client Library.
This is to support libdpm-full/minimal/ext with the common name.
%files -n libdpm
# Null (this is a meta package)

%if "%{?profile}" != "tv" && "%{?profile}" != "wearable"
%package -n libdpm-full
Summary: Tizen Device Policy Client library (full: mobile, ivi, common)
Group: Development/Libraries
Requires: %{name}-full = %{version}-%{release}
Requires(post): /sbin/ldconfig
Requires(postun): /sbin/ldconfig
Provides: libdpm-compat = %{version}-%{release}
Provides: libdpm-profile_common = %{version}-%{release}
Provides: libdpm-profile_mobile = %{version}-%{release}
Provides: libdpm-profile_ivi = %{version}-%{release}
Requires: libdpm-ext-password = %{version}-%{release}

%description -n libdpm-full
The libdpm package contains the libraries needed to run DPM client.
This containts the full library (mobile/ivi/common)

%post -n libdpm-full -p /sbin/ldconfig

%postun -n libdpm-full -p /sbin/ldconfig

%files -n libdpm-full
%manifest device-policy-manager.manifest
%defattr(644,root,root,755)
%attr(755,root,root) %{_libdir}/libdpm-ext-krate_zone.so.%{version}
%{_libdir}/libdpm-ext-krate_zone.so.0
%endif

%package -n libdpm-minimal
Summary: Tizen Device Policy Client library (minimal: tv)
Group: Development/Libraries
Requires: %{name} = %{version}-%{release}
Requires(post): /sbin/ldconfig
Requires(postun): /sbin/ldconfig
Provides: libdpm-compat = %{version}-%{release}
Provides: libdpm-profile_tv = %{version}-%{release}
%description -n libdpm-minimal
The libdpm package contains the libraries needed to run DPM client.
This containts the minimal library (tv)
%post -n libdpm-minimal -p /sbin/ldconfig
%postun -n libdpm-minimal -p /sbin/ldconfig
%files -n libdpm-minimal
%manifest device-policy-manager.manifest
%defattr(644,root,root,755)
%attr(755,root,root) %{_libdir}/libdpm.so.%{version}
%{_libdir}/libdpm.so.0

%if "%{?profile}" != "tv"
%package -n libdpm-ext-password
Summary: Tizen Device Policy Client library (min+password: wearable)
Group: Development/Libraries
Requires: %{name} = %{version}-%{release}
Requires(post): /sbin/ldconfig
Requires(postun): /sbin/ldconfig
Provides: libdpm-compat = %{version}-%{release}
Provides: libdpm-profile_wearable = %{version}-%{release}
Requires: libdpm-minimal = %{version}-%{release}
%description -n libdpm-ext-password
The libdpm package contains the libraries needed to run DPM client.
This containts the minimal + password library (wearable)
%post -n libdpm-ext-password -p /sbin/ldconfig
%postun -n libdpm-ext-password -p /sbin/ldconfig
%files -n libdpm-ext-password
%manifest device-policy-manager.manifest
%defattr(644,root,root,755)
%attr(755,root,root) %{_libdir}/libdpm-ext-password.so.%{version}
%{_libdir}/libdpm-ext-password.so.0
%endif

## Devel Package ##############################################################
%package -n libdpm-devel
Summary: Libraries and header files for device policy client development
Group: Development/Libraries
%if "%{?profile}" == "tv"
# Obsolete Build System Support
Requires: libdpm-minimal = %{version}-%{release}
%endif
%if "%{?profile}" == "wearable"
# Obsolete Build System Support
Requires: libdpm-ext-password = %{version}-%{release}
%endif
%if "%{?profile}" != "tv" && "%{?profile}" != "wearable"
Requires: libdpm-full = %{version}-%{release}
%endif
Requires: libdpm = %{version}-%{release}

%description -n libdpm-devel
The libdpm-devel package includes the libraries and header files necessary for
developing the DPM client program.

%files -n libdpm-devel
%manifest device-policy-manager.manifest
%defattr(644,root,root,755)
%{_libdir}/libdpm.so
# The usage of "profile" in the following expressions is for
# backward compatility and GBM build optimization only. They will be
# ignored in Tizen 4.0 Unified Build Environment.
%if "%{?profile}" != "tv"
%{_libdir}/libdpm-ext-password.so
%if "%{?profile}" != "wearable"
%{_libdir}/libdpm-ext-krate_zone.so
%endif
%endif
%{_includedir}/dpm
%{_libdir}/pkgconfig/dpm.pc

## Test Package ##############################################################
%if "%{?profile}" != "tv" && "%{?profile}" != "wearable"
%package -n dpm-testcases-full
Summary: Device Policy Manager test cases
Group: Development/Libraries
Requires: libdpm-full = %{version}-%{release}
Provides: dpm-testcases = %{version}-%{release}
Provides: dpm-testcases-profile_common = %{version}-%{release}
Provides: dpm-testcases-profile_mobile = %{version}-%{release}
Provides: dpm-testcases-profile_ivi = %{version}-%{release}
Conflicts: dpm-testcases-minimal
Conflicts: dpm-testcases-ext-password

%description -n dpm-testcases-full
Testcases for device policy manager and device policy client
This supports "full" version of dpm (common/mobile/ivi profiles)

%files -n dpm-testcases-full
%defattr(644,root,root,755)
%attr(755,root,root) %{_bindir}/dpm-api-tests
# common+ivi+wearable vs mobile vs tv
%endif

%package -n dpm-testcases-minimal
Summary: Device Policy Manager test cases (minimal for TV)
Group: Development/Libraries
Requires: libdpm-minimal = %{version}-%{release}
Provides: dpm-testcases-profile_tv = %{version}-%{release}
Conflicts: dpm-testcases-full
Conflicts: dpm-testcases-ext-password
%description -n dpm-testcases-minimal
Minimal testcases for device policy manager and device policy client
This support "minimal" version of dpm (tv profile)
%post -n dpm-testcases-minimal
ln -sf dpm-api-tests.minimal %{_bindir}/dpm-api-tests
%preun -n dpm-testcases-minimal
rm %{_bindir}/dpm-api-tests
%files -n dpm-testcases-minimal
%defattr(644,root,root,755)
%attr(755,root,root) %{_bindir}/dpm-api-tests.minimal

%if "%{?profile}" != "tv"
%package -n dpm-testcases-ext-password
Summary: Device Policy Manager test cases (minimal+password for wearable)
Group: Development/Libraries
Requires: libdpm-ext-password = %{version}-%{release}
Provides: dpm-testcases-profile_wearable = %{version}-%{release}
Conflicts: dpm-testcases-minimal
Conflicts: dpm-testcases-full
%description -n dpm-testcases-ext-password
Minimal+password testcases for device policy manager and device policy client
This support "minimal+password" version of dpm (wearable profile)
%post -n dpm-testcases-ext-password
ln -sf dpm-api-tests.ext-pwd %{_bindir}/dpm-api-tests
%preun -n dpm-testcases-ext-password
rm %{_bindir}/dpm-api-tests
%files -n dpm-testcases-ext-password
%defattr(644,root,root,755)
%attr(755,root,root) %{_bindir}/dpm-api-tests.ext-pwd
# common+ivi+wearable vs mobile vs tv
%endif

## DPM Syspopup Package ######################################################
%package -n org.tizen.dpm-syspopup
Summary: Tizen DPM system popup Interface
Group: Security/Other
BuildRequires: pkgconfig(elementary)
BuildRequires: pkgconfig(capi-appfw-application)
BuildRequires: pkgconfig(capi-system-system-settings)
BuildRequires: pkgconfig(capi-ui-efl-util)
BuildRequires: pkgconfig(capi-system-info)

%description -n org.tizen.dpm-syspopup
Tizen DPM system popup interface package
# No differences per profile.

%files -n org.tizen.dpm-syspopup -f dpm-syspopup.lang
%defattr(-,root,root,-)
%manifest tools/syspopup/org.tizen.dpm-syspopup.manifest
%{TZ_SYS_RO_APP}/org.tizen.dpm-syspopup/bin/*
%{TZ_SYS_RO_PACKAGES}/org.tizen.dpm-syspopup.xml
/usr/share/icons/default/small/org.tizen.dpm-syspopup.png
%{TZ_SYS_RO_APP}/org.tizen.dpm-syspopup/res/images/*
