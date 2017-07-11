#
# spec file for package loopidity
#
# Copyright (c) 2012-2107 bill-auger bill-auger@programmer.net
#
# All modifications and additions to the file contributed by third parties
# remain the property of their copyright owners, unless otherwise agreed
# upon. The license for this file, and modifications and additions to the
# file, is the same license as for the pristine package itself (unless the
# license for the pristine package is not an Open Source License, in which
# case the license is the MIT License). An "Open Source License" is a
# license that conforms to the Open Source Definition (Version 1.9)
# published by the Open Source Initiative.

# Please submit bugfixes or comments via http://bugs.opensuse.org/
#
# NOTE: hooks/pre-commit will clobber 'Version'


Name:          loopidity
Version:       0.14.005
Release:       1%{?dist}
Summary:       Multi-track multi-channel looping audio recorder designed for live hands-free use
License:       GPL-3.0
URL:           https://github.com/bill-auger/%{name}/
Source0:       https://github.com/bill-auger/%{name}/archive/v%{version}.tar.gz
BuildRequires: gcc-c++
Requires:      SDL_gfx SDL_ttf
%if 0%{?suse_version}
BuildRequires: jack-devel SDL_gfx-devel SDL_ttf-devel libX11-devel update-desktop-files
Requires:      jack
%endif
%if 0%{?fedora_version}
BuildRequires: jack-audio-connection-kit-devel SDL_gfx-devel SDL_ttf-devel libX11-devel
Requires:      jack-audio-connection-kit
%endif
# %if 0%{?mageia_version} || 0%{?mdkversion} # NFG
# BuildRequires: libjack-devel libSDL_gfx-devel libSDL_ttf-devel libx11-devel
#              lib64jack-devel lib64SDL_gfx-devel lib64SDL_ttf-devel lib64x11-devel
# Requires:      jack lib64SDL_gfx15 lib64SDL_ttf2.0_0
# %endif

%description
  Loopidity is a native GNU/Linux and Windows application built with
  the SDL framework and using the JACK Connection Kit for audio I/O.
  Loopidity is designed so that the most useful functions can be controlled
  using as few buttons as possible (a small number of foot pedals for example).
  The Loopidity state graph is kept as small and linear as possible
  so that it can be easily memorized; as not to hinder the creative workflow,
  but to quickly become an intuitive activity.


%global debug_package %{nil}

%prep
%autosetup

%build
cd build/
make %{?_smp_mflags}

%install
cd build/
make DESTDIR="%{buildroot}/" PREFIX=/usr install

%files
%doc
%{_bindir}/%{name}
%_datadir/%{name}

%post
%if ! 0%{?suse_version}
  xdg-desktop-menu forceupdate
%endif

%postun
%if ! 0%{?suse_version}
  xdg-desktop-menu forceupdate
%endif


# GIT-HOOK-SENTINEL: DO NOT DELETE - hooks/pre-commit will clobber below this line

%changelog
* Tue Jul 18 2017 bill-auger
- v0.14.005
