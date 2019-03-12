/*
	*
	* NBSettings.hpp - Settings Module for NewBreeze
	*
*/

#pragma once

#include <QtCore>
#include <QKeySequence>

class NBSettings {

	public:
		enum Scope{
			GlobalScope = 0x28d1e9,				// Values stored in ~/.config/NewBreeze/NewBreeze.conf
			LocalScope,							// Values stored in $(PWD)/.directory if it exists, GlobalScope
			SuperStart,
			Catalogs
		};

		static NBSettings *instance();

		void reload() {

			nb3settings->sync();
		};

		struct proxy {
			QString data;
			QString filename;

			operator bool() const {

				if ( filename.count() ) {
					QSettings localSettings( filename, QSettings::NativeFormat );
					return localSettings.value( "NewBreeze/" + data, nb3settings->value( data ).toBool() ).toBool();
				}

				return nb3settings->value( data ).toBool();
			};

			operator int() const {

				if ( filename.count() ) {
					QSettings localSettings( filename, QSettings::NativeFormat );
					return localSettings.value( "NewBreeze/" + data, nb3settings->value( data ).toInt() ).toInt();
				}

				return nb3settings->value( data ).toInt();
			};

			operator QString() const {

				if ( filename.count() ) {
					QSettings localSettings( filename, QSettings::NativeFormat );
					return localSettings.value( "NewBreeze/" + data, nb3settings->value( data ).toString() ).toString();
				}

				return nb3settings->value( data ).toString();
			};

			operator QStringList() const {

				if ( filename.count() ) {
					QSettings localSettings( filename, QSettings::NativeFormat );
					return localSettings.value( "NewBreeze/" + data, nb3settings->value( data ).toStringList() ).toStringList();
				}

				return nb3settings->value( data ).toStringList();
			};

			operator QSize() const {

				if ( filename.count() ) {
					QSettings localSettings( filename, QSettings::NativeFormat );
					return localSettings.value( "NewBreeze/" + data, nb3settings->value( data ).toSize() ).toSize();
				}

				return nb3settings->value( data ).toSize();
			};

			operator QRect() const {

				if ( filename.count() ) {
					QSettings localSettings( filename, QSettings::NativeFormat );
					return localSettings.value( "NewBreeze/" + data, nb3settings->value( data ).toRect() ).toRect();
				}

				return nb3settings->value( data ).toRect();
			};
		};

		proxy value( QString key, NBSettings::Scope scope = NBSettings::LocalScope );
		void setValue( QString key, QVariant value, NBSettings::Scope scope = NBSettings::LocalScope );

		static QList<QKeySequence> shortcuts( QString );
		static QKeySequence shortcut( QString );

		static bool ClosingDown;

		private:
			static QSettings *nb3settings;
			static QSettings *defSett;

			NBSettings();

			static NBSettings *mSett;
};
