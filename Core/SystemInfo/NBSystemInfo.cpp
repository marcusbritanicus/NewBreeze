/*
	*
	* NBSystemInfo.cpp - SystemInfo class for NewBreeze
	*
*/

#include <NBSystemInfo.hpp>

DesktopSession::Session DesktopSession::activeSession() {

	/*
		*
		* Desktop Detection
		*
		* KDE           : DESKTOP_SESSION=*kde*;       KDE_FULL_SESSION=true;
		* XFCE          : DESKTOP_SESSION=xfce;
		* GNOME         : DESKTOP_SESSION=gnome;
		* RAZOR         : DESKTOP_SESSION=razor
		* LXDE          : DESKTOP_SESSION=LXDE;        XDG_CURRENT_DESKTOP=LXDE;       _LXSESSION_PID=<INT>;
		* OBOX          : DESKTOP_SESSION=openbox;
		* TWM           : DESKTOP_SESSION=twm
		*
	*/

	QString DS1 = qgetenv( "DESKTOP_SESSION" );
	QString DS2 = qgetenv( "KDE_FULL_SESSION" );
	QString DS3 = qgetenv( "XDG_CURRENT_DESKTOP" );

	// A kde session, e.g. kde-plasma, openbox-kde
	if ( DS1.toLower().contains( QString( "kde" ) ) or DS3.toLower().contains( QString( "kde" ) ) or DS2 == QString( "true" ) )
		return DesktopSession::KDE;

	// XFCE Session, e.g. xfce, xfce4
	else if ( DS1.toLower().contains( QString( "xfce" ) ) or DS3.toLower().contains( QString( "xfce" ) ) )
		return DesktopSession::XFCE;

	// GNOME Session, e.g. gnome-shell, gnome-fallback
	else if ( DS1.toLower().contains( QString( "gnome" ) ) or DS3.toLower().contains( QString( "gnome" ) ) )
		return DesktopSession::GNOME;

	// RazorQt Session, e.g. razor, razor-openbox
	else if ( DS1.toLower().contains( QString( "razor" ) ) or  DS3.toLower().contains( QString( "razor" ) ) )
		return DesktopSession::RAZOR;

	// LXDE Session: LXDE
	else if ( DS1.toLower().contains( QString( "lxde" ) ) or DS3.toLower().contains( QString( "lxde" ) ) )
		return DesktopSession::LXDE;

	// I have no information about this desktop type
	else
		return DesktopSession::UNKNOWN;
};

QString NBSystemIconTheme() {
	/*
		*
		* DefaultThemeDetection
		*     kde:		<QSettings> .kde/share/config/kdeglobals/[Icons]/Theme
		*     gnome:	$ gconftool --get /desktop/gnome/interface/icon_theme
		*     xfce:		$ xfconf-query -c xsettings -p /Net/IconThemeName
		*     razorqt:	<QSettings> .razor/razor.conf/[General]/icon_theme
		*     lxde:     ??
		*
	*/

	switch( DesktopSession::activeSession() ) {
		case DesktopSession::KDE :
			return QSettings( qgetenv( "HOME" ) + "/.kde/share/config/kdeglobals", QSettings::NativeFormat ).value( "Icons/Theme" ).toString();

		case DesktopSession::GNOME : {
			QStringList args = QStringList() << "--get" << "/desktop/gnome/interface/icon_theme";
			QProcess proc;
			proc.start(  "gconftool-2", args, QProcess::ReadOnly );
			proc.waitForFinished( -1 );

			return QString( proc.readAllStandardOutput() ).simplified();
		}

		case DesktopSession::XFCE : {
			QStringList args = QStringList() << "-c" << "xsettings" << "-p" << "/Net/IconTheme/Name";
			QProcess proc;
			proc.start(  "xfconf-query", args, QProcess::ReadOnly );
			proc.waitForFinished( -1 );

			return QString( proc.readAllStandardOutput() ).simplified();
		}

		case DesktopSession::RAZOR :
			return QSettings( qgetenv( "HOME" ) + "/.razor/razor.conf", QSettings::NativeFormat ).value( "icon_theme" ).toString();

		case DesktopSession::LXDE:
			// Right now I have no idea where the hell it stores the data. So we return the same as QIcon::themeName()

		default :
			return QString( "hicolor" );
	}

	return QString();
};
