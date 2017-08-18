Name:    device-policy-manager
Version: 1.0.1
Release: 0
License: Apache-2.0
Source0: %{name}-%{version}.tar.gz
Summary: Tizen Device Policy Manager
Group:   Security/Other
BuildRequires: gcc
BuildRequires: cmake
BuildRequires: gettext-tools
BuildRequires: pkgconfig(klay)
BuildRequires: pkgconfig(glib-2.0)
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
BuildRequires: pkgconfig(notification)
BuildRequires: pkgconfig(cynara-client)
BuildRequires: pkgconfig(cynara-session)
BuildRequires: pkgconfig(libtzplatform-config)
BuildRequires: pkgconfig(security-privilege-manager)
BuildRequires: pkgconfig(capi-system-system-settings)

%description
The device-policy-manager package provides a daemon which is responsible for
managing device policies.

%files
%manifest device-policy-manager.manifest
%defattr(644,root,root,755)
%attr(700,root,root) %{_bindir}/dpm-admin-cli
%attr(755,root,root) %{_bindir}/dpm-syspopup
%attr(755,root,root) %{_bindir}/dpm-storage-builder
%attr(755,root,root) %{_bindir}/device-policy-manager
%attr(755,root,root) %{_bindir}/device-policy-syspopup
%attr(755,root,root) %{_libdir}/libdpm-pil.so.%{version}
%{_libdir}/libdpm-pil.so.0
%{_unitdir}/device-policy-manager.service
%{_unitdir}/multi-user.target.wants/device-policy-manager.service
%{_unitdir}/device-policy-syspopup.service
/etc/dbus-1/system.d/org.tizen.dpm.syspopup.conf
/usr/share/dbus-1/system-services/org.tizen.dpm.syspopup.service

%prep
%setup -q

%build
%{!?build_type:%define build_type "RELEASE"}

%if %{build_type} == "DEBUG" || %{build_type} == "PROFILING" || %{build_type} == "CCOV"
	CFLAGS="$CFLAGS -Wp,-U_FORTIFY_SOURCE"
	CXXFLAGS="$CXXFLAGS -Wp,-U_FORTIFY_SOURCE"
%endif

%cmake . -DVERSION=%{version} \
         -DCMAKE_BUILD_TYPE=%{build_type} \
         -DSCRIPT_INSTALL_DIR=%{_scriptdir} \
         -DSYSTEMD_UNIT_INSTALL_DIR=%{_unitdir} \
         -DDATA_INSTALL_DIR=%{TZ_SYS_DATA}/dpm \
         -DDB_INSTALL_DIR=%{TZ_SYS_DB} \
         -DRUN_INSTALL_DIR=%{TZ_SYS_RUN} \
         -DAPP_INSTALL_PREFIX="%{TZ_SYS_RO_APP}" \
         -DAPP_SHARE_PACKAGES_DIR="%{TZ_SYS_RO_PACKAGES}" \
         -DPAMD_INSTALL_DIR=/etc/pam.d \
         -DDUBS_CONFIGURE_DIR=/etc/dbus-1/system.d \
         -DDUBS_SERVICE_DIR=/usr/share/dbus-1/system-services

make %{?jobs:-j%jobs}

%install
%make_install
mkdir -p %{buildroot}/%{_unitdir}/multi-user.target.wants
ln -s ../device-policy-manager.service %{buildroot}/%{_unitdir}/multi-user.target.wants/device-policy-manager.service

%find_lang dpm-syspopup

%clean
rm -rf %{buildroot}

%post

%preun

%postun

## Devel Package ##############################################################
%package -n device-policy-manager-devel
Summary: Libraries and header files for device policy client development
Group: Development/Libraries
Requires: device-policy-manager = %{version}-%{release}
BuildRequires: pkgconfig(bundle)
BuildRequires: pkgconfig(pkgmgr)
BuildRequires: pkgconfig(pkgmgr-info)
BuildRequires: pkgconfig(aul)
BuildRequires: pkgconfig(appsvc)
BuildRequires: pkgconfig(syspopup-caller)

%description -n device-policy-manager-devel
The libdpm-pil-devel package includes the libraries and header files necessary for
developing the device policy module.

%files -n device-policy-manager-devel
#%manifest device-policy-client.manifest
%defattr(644,root,root,755)
%{_libdir}/libdpm-pil.so
%{_includedir}/dpm/pil
%{_libdir}/pkgconfig/dpm-pil.pc

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

%files -n org.tizen.dpm-syspopup -f dpm-syspopup.lang
%defattr(-,root,root,-)
%manifest tools/syspopup/org.tizen.dpm-syspopup.manifest
%{TZ_SYS_RO_APP}/org.tizen.dpm-syspopup/bin/*
%{TZ_SYS_RO_PACKAGES}/org.tizen.dpm-syspopup.xml
/usr/share/icons/default/small/org.tizen.dpm-syspopup.png
%{TZ_SYS_RO_APP}/org.tizen.dpm-syspopup/res/images/*
