%define desktop_vendor newrpms
%{!?_dist: %{expand: %%define dist rhfc14}}

Summary:	GADMIN-SAMBA -- A GTK+ administation tool for the SAMBA server.
Name:		gadmin-samba
Version:	0.3.4
Release:	0.1.%{dist}.nr
License:	GPL
Group:		Applications/System
URL:		http://dalalven.dtdns.net/linux.html
Source0:	http://dalalven.dtdns.net/linux/%{name}/%{name}-%{version}.tar.gz
BuildRoot:	%{_builddir}/%{name}-%{version}-root
Provides:	gadmin-samba

%description
GADMIN-SAMBA is a fast and easy to use GTK+ administration tool for the SAMBA server.

%prep
%setup -q
%configure SYSINIT_START_CMD="chkconfig smb on" SYSINIT_STOP_CMD="chkconfig smb off"

%build
%{__make}

%install
rm -rf $RPM_BUILD_ROOT
%makeinstall INSTALL_USER=`id -un` INSTALL_GROUP=`id -gn`

# Pam auth
install -d %{buildroot}%{_sysconfdir}/pam.d/
install -d %{buildroot}%{_sysconfdir}/security/console.apps
install -m 644 etc/pam.d/%{name} %{buildroot}%{_sysconfdir}/pam.d/%{name}
install -m 644 etc/security/console.apps/%{name} %{buildroot}%{_sysconfdir}/security/console.apps/%{name}

# Desktop entry
install -d %{buildroot}%{_datadir}/applications
install -m 644 desktop/gadmin-samba.desktop %{buildroot}%{_datadir}/applications/%{name}.desktop

# Doc files
install -d %{buildroot}%{_datadir}/doc/%{name}
install -m 644  README COPYING AUTHORS ChangeLog %{buildroot}%{_datadir}/doc/%{name}

# Logon scripts 
install -d %{buildroot}/var/lib/samba/netlogon
install -m 755 scripts/example.bat %{buildroot}/var/lib/samba/netlogon

# Profiles directory
install -d %{buildroot}/var/lib/samba/profiles

# PDF Printer script
install -d %{buildroot}%{_bindir}
install -m 755 scripts/gadmin-samba-pdf %{buildroot}%{_bindir}

# PDF Printer output directory
install -d %{buildroot}/var/lib/samba/pdf-documents




%find_lang %name

%post 
if test ! -h %{_bindir}/gadmin-samba ; then \
ln -s %{_bindir}/consolehelper %{_bindir}/gadmin-samba ; \
fi;

%clean
rm -rf $RPM_BUILD_ROOT

%files -f %{name}.lang
###%files

%defattr(0755, root, root)
%{_sbindir}/%{name}
%{_bindir}/gadmin-samba-pdf
/var/lib/samba/netlogon/example.bat

%defattr(0644, root, root)
#%doc COPYING AUTHORS ChangeLog
%{_datadir}/doc/%{name}/README
%{_datadir}/doc/%{name}/COPYING 
%{_datadir}/doc/%{name}/AUTHORS 
%{_datadir}/doc/%{name}/ChangeLog

%{_sysconfdir}/pam.d/%{name}
%{_sysconfdir}/security/console.apps/%{name}
%{_datadir}/applications/%{name}.desktop
%{_datadir}/pixmaps/*.png
%dir %{_datadir}/pixmaps/%{name}
%{_datadir}/pixmaps/%{name}/*.png

%defattr(1755, root, root)
%dir /var/lib/samba/profiles
%defattr(1755, root, root)
%dir /var/lib/samba/pdf-documents

%changelog
* Wed Jul 12 2012 Magnus Loef <magnus-swe@telia.com>
- Code cleanup and indent fixes.

* Tue Feb 14 2012 Magnus Loef <magnus-swe@telia.com>
- Sets standard configuration for client signing/schannel,
- server signing/schannel and spnego to yes.
- Fixes a possible startup crash.
- Indent fixes and patch cleanups.
- Combines public and guest ok options.
- Removes the apply server settings button.

* Fri Feb 18 2011 Magnus Loef <magnus-swe@telia.com>
- Allow more then 30 chars for the samba version string.
- Added Valid users = %U as default for the [homes] share.
* Thu Sep 12 2010 Magnus Loef <magnus-swe@telia.com>
- Allows for more dynamic configuration of netlogon, profiles and pdf-documents.
* Thu Sep 9 2010 Magnus Loef <magnus-swe@telia.com>
- Changed paths to /var/lib/samba for netlogon, profiles and pdf-documents.
- All users and All groups macros can now be imported.
- Added and updated Deutsch translation (Thanks to Manfred Mueller).
- Adds better default user profile values.
- Improved handling of directories with spaces.
- A small helpfile change (add user/apply user).

* Sat Mar 13 2010 Magnus Loef <magnus-swe@telia.com>
- Now using pdbedit and the tdbsam files instead
- of the smbpasswd file and binary.

* Wed Nov 18 2009 Magnus-swe <magnus-swe@telia.com>
- Added a share path selector.
- Added a home directory selector.
- Shows status color. Array comparizon fixes.
- Option "share modes" has been deprecated and will
- be removed automatically when a share is applied.
- Options "locking" and "strict locking" can be used instead.

* Tue May 6 2008 Magnus-swe <magnus-swe@telia.com>
- Added "bind interfaces only = yes" to the standard configuration.
- Added French translation.
- wins server = option is not used if theres no ip/dns name.

* Tue May 6 2008 Magnus-swe <magnus-swe@telia.com>
- Fixed cups printing.

* Mon Apr 28 2008 Magnus-swe <magnus-swe@telia.com>
- Added support for continued lines.
- Added better automatic configuration backups.

* Thu Feb 21 2008 Magnus-swe <magnus-swe@telia.com>
- Changed name from gsambad to gadmin-samba.
* Sat Jun 25 2007 Magnus-swe <magnus-swe@telia.com>
- License is now GPL version 3 or later.
- New share access gui layout.
- New user and group per share row splitting functions.
- Lots of other things including coloring/highlighting
- of the status indicator labels.
* Sat Jun 16 2007 Magnus-swe <magnus-swe@telia.com>
- Added RC start and stop commands.
- Updated pam.d authentication specifications.
* Wed Aug 2 2006 Magnus-swe <magnus-swe@telia.com>
- Fixed connections tab and split filenames.
- Fixed add_standard_users() and the NOBODY user
- account addition (Thanks to Andreas)
- Added configure option SAMBA_USERMAP.
- The smbusers map file will now be created if
- its bad or doesnt exist.
- The samba root user is now added as a locked account.
- Users changing to BDC, PDC or ADS are notified to alter
- samba root conditions. The 10 second non-root user dialog
- can now be closed with the close buttons.
* Sun Jul 2 2006 Magnus-swe <magnus-swe@telia.com>
- Added multiconnection killing and old conf backup.
- New detection for no good configurations.
* Sat Jun 17 2006 Magnus-swe <magnus-swe@telia.com>
- Added tabs: domain mgmt, connections and machine accounts. 
* Thu May 25 2006 Magnus-swe <magnus-swe@telia.com>
- Some small typo fixes and updates.
* Mon Mar 27 2006 Magnus-swe <magnus-swe@telia.com>
- Added a virtual pdf printer and output share.
- Added a netlogon example.bat script and alot more.
