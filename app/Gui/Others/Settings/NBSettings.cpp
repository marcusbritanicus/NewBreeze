/*
	*
	* NBSettings.hpp - Settings Module for NewBreeze
	*
*/

#include "NBSettings.hpp"

QSettings *NBSettings::nb3settings = NULL;
QSettings *NBSettings::defSett = NULL;

NBSettings *NBSettings::mSett = NULL;
bool NBSettings::ClosingDown = false;

NBSettings* NBSettings::instance() {

	if ( mSett )
		return mSett;

	mSett = new NBSettings();

	return mSett;
}

NBSettings::NBSettings() {

	nb3settings = new QSettings( "NewBreeze", "NewBreeze" );
	defSett = new QSettings( ":/data/NewBreeze.conf", QSettings::NativeFormat );
};

NBSettings::proxy NBSettings::value( QString key, NBSettings::Scope scope ) {

	nb3settings->sync();

	switch ( scope ) {
		case NBSettings::GlobalScope: {

			proxy p{ key, QString() };
			return p;
		}

		case NBSettings::LocalScope: {

			proxy p{ key, QDir::current().filePath( ".directory" ) };
			return p;
		}

		case NBSettings::SuperStart: {

			QSettings sett( "NewBreeze", "SuperStart" );

			proxy p{ key, sett.fileName() };
			return p;
		}

		case NBSettings::Catalogs: {

			QSettings sett( "NewBreeze", "Catalogs" );

			proxy p{ key, sett.fileName() };
			return p;
		}
	}

	return proxy{};
};

void NBSettings::setValue( QString key, QVariant value, NBSettings::Scope scope ) {

	if ( scope == NBSettings::GlobalScope ) {
		nb3settings->setValue( key, value );
		nb3settings->sync();
		return;
	}

	QSettings localSett( QDir::current().filePath( ".directory" ), QSettings::NativeFormat );
	localSett.setValue( "NewBreeze/" + key, value );
	localSett.sync();
};

QList<QKeySequence> NBSettings::shortcuts( QString action ) {

	QStringList keys = nb3settings->value( "Shortcuts/" + action, defSett->value( "Shortcuts/" + action ).toStringList() ).toStringList();
	QList<QKeySequence> shortcuts;

	if ( not keys.count() )
		qWarning() << "No key bindings found for" << action;

	foreach( QString key, keys )
		shortcuts << QKeySequence( key );

	return shortcuts;
};

QKeySequence NBSettings::shortcut( QString action ) {

	return shortcuts( action ).value( 0 );
};
