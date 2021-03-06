Name:       auth-fw
Summary:    Authentication framework
Version:    1.1.0
Release:    1
Group:      Security/Service
License:    Apache-2.0
Source0:    %{name}-%{version}.tar.gz
Source1001: %{name}.manifest
Source1002: lib%{name}-client.manifest
Source1003: lib%{name}-client-admin.manifest
Source1004: lib%{name}-sw-backend.manifest
Source1005: %{name}-test.manifest
BuildRequires: cmake
BuildRequires: pkgconfig(dlog)
BuildRequires: pkgconfig(libsmack)
BuildRequires: pkgconfig(libsystemd)
BuildRequires: pkgconfig(libtzplatform-config)
Requires: security-config
Requires: lib%{name}-sw-backend = %{version}-%{release}
%{?systemd_requires}

%description
Authentication framework which is consist of client library and server daemon

%global user_name security_fw
%global group_name security_fw
%global run_dir %{?TZ_SYS_RUN:%TZ_SYS_RUN}%{!?TZ_SYS_RUN:/var/run}
%global bin_dir %{?TZ_SYS_BIN:%TZ_SYS_BIN}%{!?TZ_SYS_BIN:%_bindir}
# image creation error occured if /usr/sbin used for ldconfig
#%global sbin_dir %{?TZ_SYS_SBIN:%TZ_SYS_SBIN}%{!?TZ_SYS_SBIN:%_sbindir}
%global sbin_dir /sbin
%global plugin_dir %{_datadir}/auth-fw
%global rw_data_dir %{?TZ_SYS_DATA:%TZ_SYS_DATA/%name}%{!?TZ_SYS_DATA:/opt/data/%name}
%global sock_passwd_check %{name}-passwd-check.socket
%global sock_passwd_set %{name}-passwd-set.socket
%global sock_passwd_reset %{name}-passwd-reset.socket
%global sock_passwd_policy %{name}-passwd-policy.socket

%package -n lib%{name}-client
Summary:    Authentication framework (client)
Group:      Security/Libraries
Requires:   %{name} = %{version}-%{release}
Requires(post): %{sbin_dir}/ldconfig
Requires(postun): %{sbin_dir}/ldconfig

%description -n lib%{name}-client
Authentication framework package (client)

%package -n lib%{name}-client-admin
Summary:    Authentication framework (client-admin)
Group:      Security/Libraries
Requires:   %{name} = %{version}-%{release}
Requires(post): %{sbin_dir}/ldconfig
Requires(postun): %{sbin_dir}/ldconfig

%description -n lib%{name}-client-admin
Authentication framework package (client-admin)

%package -n lib%{name}-client-devel
Summary:    Authentication framework (client-devel)
Group:      Security/Development
Requires:   lib%{name}-client = %{version}-%{release}
Requires:   lib%{name}-client-admin = %{version}-%{release}
Requires:   lib%{name}-sw-backend = %{version}-%{release}

%description -n lib%{name}-client-devel
Authentication framework package (client-devel)

%prep
%setup -q

%build
export CFLAGS="$CFLAGS -DTIZEN_DEBUG_ENABLE"
export CXXFLAGS="$CXXFLAGS -DTIZEN_DEBUG_ENABLE"
export FFLAGS="$FFLAGS -DTIZEN_DEBUG_ENABLE"
export LDFLAGS+="-Wl,--rpath=%{_libdir}"

%cmake . -DVERSION=%{version} \
        -DCMAKE_BUILD_TYPE=%{?build_type:%build_type}%{!?build_type:RELEASE} \
        -DCMAKE_VERBOSE_MAKEFILE=ON \
        -DSERVICE_NAME=%{name} \
        -DBIN_DIR:PATH=%{bin_dir} \
        -DSYS_CONFIG_DIR:PATH=%{_sysconfdir} \
        -DRUN_DIR:PATH=%{run_dir} \
        -DRW_DATA_DIR:PATH=%{rw_data_dir} \
        -DPLUGIN_DIR:PATH=%{plugin_dir} \
        -DPLUGIN_SW_BACKEND_PATH=%{plugin_dir}/lib%{name}-sw-backend.so.%{version} \
        -DPLUGIN_TZ_BACKEND_PATH=%{plugin_dir}/lib%{name}-tz-backend.so.%{version} \
        -DSYSTEMD_UNIT_DIR:PATH=%{_unitdir} \
        -DINCLUDE_DIR:PATH=%{_includedir} \
        -DSOCK_PASSWD_CHECK=%{sock_passwd_check} \
        -DSOCK_PASSWD_SET=%{sock_passwd_set} \
        -DSOCK_PASSWD_RESET=%{sock_passwd_reset} \
        -DSOCK_PASSWD_POLICY=%{sock_passwd_policy} \
        -DADDITIONAL_LIBRARIES=%{?asan:"-lpthread"}

make %{?jobs:-j%jobs}

%install
%make_install
%install_service multi-user.target.wants %{name}.service
%install_service sockets.target.wants %{sock_passwd_check}
%install_service sockets.target.wants %{sock_passwd_set}
%install_service sockets.target.wants %{sock_passwd_reset}
%install_service sockets.target.wants %{sock_passwd_policy}

mkdir -p %{buildroot}/%{rw_data_dir}

cp -a %{SOURCE1001} %{SOURCE1002} %{SOURCE1003} %{SOURCE1004} %{SOURCE1005} %{buildroot}%{_datadir}/

%post
%{sbin_dir}/ldconfig
systemctl daemon-reload
if [ $1 = 1 ]; then
    # installation
    systemctl start %{name}.service
fi

if [ $1 = 2 ]; then
    # update
    systemctl restart %{name}.service
fi
chsmack -a System %{rw_data_dir}

%preun
if [ $1 = 0 ]; then
    # unistall
    systemctl stop %{name}.service
fi

%postun
%{sbin_dir}/ldconfig
if [ $1 = 0 ]; then
    # unistall
    systemctl daemon-reload
fi

%post -n lib%{name}-client -p %{sbin_dir}/ldconfig

%postun -n lib%{name}-client -p %{sbin_dir}/ldconfig

%post -n lib%{name}-client-admin -p %{sbin_dir}/ldconfig

%postun -n lib%{name}-client-admin -p %{sbin_dir}/ldconfig

%files
%manifest %{_datadir}/%{name}.manifest
%license LICENSE
%{bin_dir}/%{name}
%{_libdir}/lib%{name}-commons.so.*
%{_unitdir}/%{name}.service
%{_unitdir}/multi-user.target.wants/%{name}.service
%{_unitdir}/%{sock_passwd_check}
%{_unitdir}/%{sock_passwd_set}
%{_unitdir}/%{sock_passwd_reset}
%{_unitdir}/%{sock_passwd_policy}
%{_unitdir}/sockets.target.wants/%{sock_passwd_check}
%{_unitdir}/sockets.target.wants/%{sock_passwd_set}
%{_unitdir}/sockets.target.wants/%{sock_passwd_reset}
%{_unitdir}/sockets.target.wants/%{sock_passwd_policy}
%{_sysconfdir}/%{name}/client-whitelist
%{_sysconfdir}/%{name}/admin-client-whitelist
%dir %attr(770, %{user_name}, %{group_name}) %{rw_data_dir}

%files -n lib%{name}-client
%manifest %{_datadir}/lib%{name}-client.manifest
%license LICENSE
%{_libdir}/lib%{name}-client.so.*

%files -n lib%{name}-client-admin
%manifest %{_datadir}/lib%{name}-client-admin.manifest
%license LICENSE
%{_libdir}/lib%{name}-client-admin.so.*

%files -n lib%{name}-client-devel
%{_libdir}/lib%{name}-client.so
%{_libdir}/lib%{name}-client-admin.so
%{_libdir}/lib%{name}-commons.so
%{_includedir}/%{name}/auth-passwd.h
%{_includedir}/%{name}/auth-passwd-admin.h
%{_includedir}/%{name}/auth-passwd-error.h
%{_includedir}/%{name}/auth-passwd-policy-types.h
%{_libdir}/pkgconfig/*.pc

## SW-Backend(PasswordFile) Package ##########################################
%package -n lib%{name}-sw-backend
Summary: Authentication framework (sw-backend)
Group: Security/Libraries
BuildRequires: pkgconfig(openssl1.1)
Requires: %{name} = %{version}
Requires(post): %{sbin_dir}/ldconfig
Requires(postun): %{sbin_dir}/ldconfig

%description -n lib%{name}-sw-backend
SW-Backend for authentication framework

%files -n lib%{name}-sw-backend
%manifest %{_datadir}/lib%{name}-sw-backend.manifest
%license LICENSE
%{plugin_dir}/lib%{name}-sw-backend.so
%{plugin_dir}/lib%{name}-sw-backend.so.*

## Test Package ##############################################################
%package test
Summary: Authentication framework (test)
Group: Security/Testing
BuildRequires: pkgconfig(klay)
Requires: %{name} = %{version}

%description test
Testcases for authentication framework

%files test
%manifest %{_datadir}/%{name}-test.manifest
%{bin_dir}/%{name}-test
