%define version 1.1.0b
%define release 3

Summary: deeangband %{version}
Name: deeangband
Version: %{version}
Release: %{release}
Copyright: unknown
Group: Amusements/Games
Packager: Takahiro MIZUNO <tow@plum.freemail.ne.jp>
Url: http://echizen.s5.xrea.com/heng/index.html
Source: deeangband-%{version}.tar.bz2
Buildroot: %{_tmppath}/%{name}-%{version}-root

%description
Deeangband is a variant of deeangband.

Official page is thish
http://echizen.s5.xrea.com/heng/eng-deeangband/index.html

More infomation is /usr/doc/deeangband-hoge/readme_eng.txt

Summary(ja): Deeangband %{version}

%description -l ja
D'angbandは変愚蛮怒のバリアントです。

本ソフトウェアの最新版は以下の場所から入手できます。
http://echizen.s5.xrea.com/heng/index.html

詳しくは /usr/doc/deeangband-hoge/readme.txt を参照。

%prep
rm -rf $RPM_BUILD_ROOT

%setup -n %{name}-%{version}

%build
./configure --prefix=%{_prefix} --bindir=%{_bindir} --with-libpath=%{_datadir}/games/deengband/lib
make

%install
mkdir -p $RPM_BUILD_ROOT/%{_bindir}
mkdir -p $RPM_BUILD_ROOT/%{_datadir}/games/deeangband
cp src/deeangband $RPM_BUILD_ROOT/%{_bindir}
cp -R lib/ -p $RPM_BUILD_ROOT/%{_datadir}/games/deeangband/
touch $RPM_BUILD_ROOT/%{_datadir}/games/deeangband/lib/apex/scores.raw

%clean
rm -rf $RPM_BUILD_ROOT

%preun
if [ -e %{_datadir}/games/deeangband/lib/data/f_info_j.raw ]
then
rm -rf %{_datadir}/games/deeangband/lib/data/*.raw
fi
exit 0

%files
%defattr(-,root,root)
%attr(2755,root,games) %{_bindir}/deeangband
%dir %{_datadir}/games/deeangband/lib
%attr(775,root,games) %dir %{_datadir}/games/deeangband/lib/apex
%attr(775,root,games) %dir %{_datadir}/games/deeangband/lib/bone
%attr(775,root,games) %dir %{_datadir}/games/deeangband/lib/data
%dir %{_datadir}/games/deeangband/lib/edit
%dir %{_datadir}/games/deeangband/lib/file
%dir %{_datadir}/games/deeangband/lib/help
%dir %{_datadir}/games/deeangband/lib/info
%dir %{_datadir}/games/deeangband/lib/pref
%attr(775,root,games) %dir %{_datadir}/games/deeangband/lib/save
%dir %{_datadir}/games/deeangband/lib/script
%dir %{_datadir}/games/deeangband/lib/user
%dir %{_datadir}/games/deeangband/lib/xtra
%dir %{_datadir}/games/deeangband/lib/xtra/graf
%{_datadir}/games/deeangband/lib/apex/h_scores.raw
%{_datadir}/games/deeangband/lib/apex/readme.txt
%attr(664 root,games) %config(noreplace) %{_datadir}/games/deeangband/lib/apex/scores.raw
%{_datadir}/games/deeangband/lib/bone/delete.me
%{_datadir}/games/deeangband/lib/data/delete.me
%{_datadir}/games/deeangband/lib/edit/*.txt
%{_datadir}/games/deeangband/lib/file/*.txt
%{_datadir}/games/deeangband/lib/help/*.hlp
%{_datadir}/games/deeangband/lib/help/*.txt
%{_datadir}/games/deeangband/lib/info/delete.me
%{_datadir}/games/deeangband/lib/pref/*.prf
%{_datadir}/games/deeangband/lib/save/delete.me
%{_datadir}/games/deeangband/lib/script/delete.me
%{_datadir}/games/deeangband/lib/user/delete.me
%{_datadir}/games/deeangband/lib/xtra/graf/8x8.bmp
%doc readme.txt readme_angband readme_eng.txt


%changelog


