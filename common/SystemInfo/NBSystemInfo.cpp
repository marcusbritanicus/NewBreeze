/*
	*
	* NBSystemInfo.cpp - SystemInfo class for NewBreeze
	*
*/

#include "NBSystemInfo.hpp"

DesktopSession::Session DesktopSession::activeSession() {

	/*
		*
		* Desktop Detection
		*
		* KDE           : DESKTOP_SESSION=kde;       KDE_FULL_SESSION=true;      KDE_SESSION_VERSION
		* XFCE          : DESKTOP_SESSION=xfce;
		* GNOME         : DESKTOP_SESSION=gnome;
		* RAZOR         : DESKTOP_SESSION=razor
		* LXDE          : DESKTOP_SESSION=LXDE;        XDG_CURRENT_DESKTOP=LXDE;       _LXSESSION_PID=<INT>;
		* OBOX          : DESKTOP_SESSION=openbox;
		* TWM           : DESKTOP_SESSION=twm
		*
	*/

	QString DS1 = qgetenv( "DESKTOP_SESSION" );
	QString DS2 = qgetenv( "KDE_SESSION_VERSION" );
	QString DS3 = qgetenv( "XDG_CURRENT_DESKTOP" );

	// A kde session, e.g. kde-plasma-5
	if ( ( DS1.toLower().contains( QString( "kde" ) ) or DS3.toLower().contains( QString( "kde" ) ) )and DS2 == "5" )
		return DesktopSession::KDE5;

	// A kde session, e.g. kde-plasma, openbox-kde
	if ( DS1.toLower().contains( QString( "kde" ) ) or DS3.toLower().contains( QString( "kde" ) ) )
		return DesktopSession::KDE4;

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

	// LXDE Session: LXQT
	else if ( DS1.toLower().contains( QString( "lxqt" ) ) or DS3.toLower().contains( QString( "lxqt" ) ) )
		return DesktopSession::LXDE;

	// LXDE Session: Cinnamon
	else if ( DS1.toLower().contains( QString( "cinnamon" ) ) or DS3.toLower().contains( QString( "cinnamon" ) ) )
		return DesktopSession::CINNAMON;

	// LXDE Session: MATE
	else if ( DS1.toLower().contains( QString( "mate" ) ) or DS3.toLower().contains( QString( "mate" ) ) )
		return DesktopSession::MATE;

	// LXDE Session: Enlightenment
	else if ( DS1.toLower().contains( QString( "enlightenment" ) ) or DS3.toLower().contains( QString( "enlightenment" ) ) )
		return DesktopSession::ENLIGHTENMENT;

	// I have no information about this desktop type
	else
		return DesktopSession::UNKNOWN;
};

QString NBSystemIconTheme() {
	/*
		*
		* DefaultThemeDetection
		*     kde4:		      <QSettings> ~/.kde/share/config/kdeglobals/[Icons]/Theme
		*     kde5:		      <QSettings> ~/.config/kdeglobals/[Icons]/Theme
		*     gnome:	      $ gconftool --get /desktop/gnome/interface/icon_theme
		*     xfce:		      $ xfconf-query -c xsettings -p /Net/IconThemeName
		*     razorqt:	      <QSettings> ~/.razor/razor.conf/[General]/icon_theme
		*     lxde:           <QSettings> ~/.config/lxsession/LXDE/desktop.conf/[GTK/sNet/IconThemeName
		*     lxqt:           <QSettings> ~/.config/lxqt/lxqt.conf/[General/icon_theme
		*     cinnamon:       gsettings get org.cinnamon.desktop.interface icon-theme
		*     MATE:           gsettings get org.mate.interface icon-theme
		*     Enlightenment:  $E_ICON_THEME
		*
	*/

	switch( DesktopSession::activeSession() ) {
		case DesktopSession::KDE4 :
			return QSettings( qgetenv( "HOME" ) + "/.kde/share/config/kdeglobals", QSettings::NativeFormat ).value( "Icons/Theme" ).toString();

		case DesktopSession::KDE5 :
			return QSettings( qgetenv( "HOME" ) + "/.config/kdeglobals", QSettings::NativeFormat ).value( "Icons/Theme" ).toString();

		case DesktopSession::GNOME : {
			QStringList args = QStringList() << "--get" << "/desktop/gnome/interface/icon_theme";
			QProcess proc;
			proc.start(  "gconftool-2", args, QProcess::ReadOnly );
			proc.waitForFinished( -1 );

			return QString( proc.readAllStandardOutput() ).simplified();
		}

		case DesktopSession::XFCE : {
			QStringList args = QStringList() << "-c" << "xsettings" << "-p" << "/Net/IconThemeName";
			QProcess proc;
			proc.start(  "xfconf-query", args, QProcess::ReadOnly );
			proc.waitForFinished( -1 );

			return QString( proc.readAllStandardOutput() ).simplified();
		}

		case DesktopSession::RAZOR :
			return QSettings( qgetenv( "HOME" ) + "/.razor/razor.conf", QSettings::NativeFormat ).value( "icon_theme" ).toString();

		case DesktopSession::LXDE:
			return QSettings( qgetenv( "HOME" ) + "/.config/lxsession/LXDE/desktop.conf", QSettings::NativeFormat ).value( "GTK/sNet/IconThemeName" ).toString();

		case DesktopSession::LXQT:
			return QSettings( qgetenv( "HOME" ) + "/.config/lxqt/lxqt.conf", QSettings::NativeFormat ).value( "icon_theme" ).toString();

		case DesktopSession::CINNAMON : {
			QStringList args = QStringList() << "get" << "org.cinnamon.desktop.interface" << "icon-theme";
			QProcess proc;
			proc.start(  "gsettings", args, QProcess::ReadOnly );
			proc.waitForFinished( -1 );

			return QString( proc.readAllStandardOutput() ).simplified().replace( "'", "" ).replace( "\"", "" );
		}

		case DesktopSession::MATE : {
			QStringList args = QStringList() << "get" << "org.mate.interface" << "icon-theme";
			QProcess proc;
			proc.start(  "gsettings", args, QProcess::ReadOnly );
			proc.waitForFinished( -1 );

			return QString( proc.readAllStandardOutput() ).simplified().replace( "'", "" ).replace( "\"", "" );
		}

		case DesktopSession::ENLIGHTENMENT : {

			return qgetenv( "E_ICON_THEME" );
		}

		default :
			return QString( "hicolor" );
	}

	return QString();
};
