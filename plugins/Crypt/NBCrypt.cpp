/*
	*
	* NBCrypt.cpp - NewBreeze File Folder Encryption Class
	*
*/

#include <NBCrypt.hpp>

/* Name of the plugin */
QString NBCrypt::name() {

	return "Crypt";
};

/* The plugin version */
QString NBCrypt::version() {

	return "3.0.0";
};

/* The QAction hooks for menus/toolbars */
QList<QAction*> NBCrypt::actions( QStringList nodes ) {

	if ( nodes.count() > 1 ) {
		/* Create a encrypted folder with multiple input folders */
		return QList<QAction*>(  );
	}

	else if ( nodes.count() == 1 ) {

		QString path = nodes.at( 0 );
		if ( isDir( path ) ) {

			/* Parent Settings object */
			QSettings sett( QDir( dirName( path ) ).filePath( ".directory" ), QSettings::NativeFormat );
			sett.beginGroup( "EncFS" );

			/* Check if the current directory is an EncFS source */
			if ( sett.childKeys().contains( baseName( path ) ) ) {

				/* If this source is mounted we need to unmount it */
				Q_FOREACH( QStorageInfo info, QStorageInfo::mountedVolumes() ) {
					if ( info.rootPath() == path ) {

						/* Unmount Action */
						QAction *act = new QAction( QIcon( ":/icons/emblem-unmounted.png" ), "&Unmount EncFS Volume", this );

						NBEncFS *encfs = new NBEncFS( path, dirName( path ) + sett.value( baseName( path ) ).toString(), mParent );
						connect( act, SIGNAL( triggered() ), encfs, SLOT( unmountDir() ) );

						return QList<QAction*>() << act;
					}
				}

				/* Mount Action */
				QAction *act = new QAction( QIcon( ":/icons/emblem-mounted.png" ), "&Mount EncFS Volume", this );

				NBEncFS *encfs = new NBEncFS( path, dirName( path ) + sett.value( baseName( path ) ).toString(), mParent );
				connect( act, SIGNAL( triggered() ), encfs, SLOT( mountDir() ) );

				return QList<QAction*>() << act;
			}

			/* Otherwise it might be EncFS target */
			Q_FOREACH( QString key, sett.childKeys() ) {
				if ( sett.value( key ).toString() == baseName( path ) ) {
					/* If it is mounted, we return unmount action */
					Q_FOREACH( QStorageInfo info, QStorageInfo::mountedVolumes() ) {
						if ( info.rootPath() == path ) {

							/* Unmount Action */
							QAction *act = new QAction( QIcon( ":/icons/emblem-unmounted.png" ), "&Unmount EncFS Volume", this );

							NBEncFS *encfs = new NBEncFS( dirName( path ) + key, path, mParent );
							connect( act, SIGNAL( triggered() ), encfs, SLOT( unmountDir() ) );

							return QList<QAction*>() << act;
						}
					}

					/* Otherwise we return mount action */
					QAction *act = new QAction( QIcon( ":/icons/emblem-mounted.png" ), "&Mount EncFS Volume", this );

					NBEncFS *encfs = new NBEncFS( dirName( path ) +  key, path, mParent );
					connect( act, SIGNAL( triggered() ), encfs, SLOT( mountDir() ) );

					return QList<QAction*>() << act;
				}
			}

			/* So, its neither encrypted nor mounted volume */
			QAction *act = new QAction( QIcon( ":/icons/encrypt.png" ), "&Create EncFS Volume", this );

			NBEncFS *encfs = new NBEncFS( path, QString(), mParent );
			connect( act, SIGNAL( triggered() ), encfs, SLOT( createEncFS() ) );

			return QList<QAction*>() << act;
		}

		/* We have no way of telling which is an encrypted file, which is not, except by extension */
		else if ( isFile( nodes.at( 0 ) ) ) {
			/* If the extension is .s20, we decrypt it */
			if ( nodes.at( 0 ).endsWith( ".s20" ) ) {
				QAction *act1 = new QAction( QIcon::fromTheme( "document-decrypt" ), "&Decrypt File", this );
				QAction *act2 = new QAction( QIcon( ":/icons/key.png" ), "Change &Password", this );

				NBSalsa20 *s20 = new NBSalsa20( path, mParent );
				connect( act1, SIGNAL( triggered() ), s20, SLOT( decrypt() ) );
				connect( act2, SIGNAL( triggered() ), s20, SLOT( changePass() ) );

				return QList<QAction*>() << act1 << act2;
			}

			/* Otherwise, we encrypt */
			else {
				QAction *act = new QAction( QIcon::fromTheme( "document-encrypt" ), "&Encrypt File", this );

				NBSalsa20 *s20 = new NBSalsa20( path, mParent );
				connect( act, SIGNAL( triggered() ), s20, SLOT( encrypt() ) );

				return QList<QAction*>() << act;
			}
		}

		else {
			/* Cannot encrypt this type of node */
			qDebug() << "Unable to encrypt/decrypt this node:" << path;
			return QList<QAction*>();
		}
	}

	/* Otherwise we return create encfs action */
	QAction *act = new QAction( QIcon( ":/icons/encrypt.png" ), "&Create EncFS Volume", this );

	NBEncFS *encfs = new NBEncFS( QString(), QString(), mParent );
	connect( act, SIGNAL( triggered() ), encfs, SLOT( createEncFS() ) );

	return QList<QAction*>() << act;
};

/* Interface type: preview, rename etc */
NBPluginInterface::Interface NBCrypt::interface() {

	return NBPluginInterface::ActionInterface;
};

/* Interface type: preview, rename etc */
NBPluginInterface::Type NBCrypt::type() {

	return NBPluginInterface::Enhancement;
};

/* Plugin load contexts */
NBPluginInterface::Contexts NBCrypt::contexts() {

	return Contexts() << NBPluginInterface::File << NBPluginInterface::Dir << NBPluginInterface::Files << NBPluginInterface::Dirs << NBPluginInterface::None;
};

/* Mimetypes handled by the plugin */
QStringList NBCrypt::mimetypes() {

	return QStringList() << "*";
};

void NBCrypt::setCaller( QWidget *caller ) {

	mParent = caller;
};

#if QT_VERSION < 0x050000
	Q_EXPORT_PLUGIN2( NBCrypt, NBCrypt );
#endif
