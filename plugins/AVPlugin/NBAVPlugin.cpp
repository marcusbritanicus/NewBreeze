/*
	*
	* NBAVPlugin.cpp - NewBreeze PreviewWidget Class
	*
*/

#include "NBAVPlugin.hpp"

static QMimeDatabase mimeDb;

/* Name of the plugin */
QString NBAVPlugin::name() {

	return "AVPlugin";
};

/* The plugin version */
QString NBAVPlugin::version() {

	return "3.0.0";
};

/* The QAction hooks for menus/toolbars */
QList<QAction*> NBAVPlugin::actions( Interface, QStringList nodes ) {

	QList<QAction*> actList;

	if ( ( nodes.count() == 1 ) and isFile( nodes.at( 0 ) ) ) {

		NBAVPlayer *player = new NBAVPlayer( nodes.at( 0 ) );

		QAction *act = new QAction( QIcon( ":/icons/peek.png" ), "&Peek", this );
		connect( act, SIGNAL( triggered() ), player, SLOT( exec() ) );
		actList << act;
	}

	return actList;
};

/* Interface type: preview, rename etc */
NBPluginInterface::Interfaces NBAVPlugin::interfaces() {

	return Interfaces() << NBPluginInterface::PreviewInterface;
};

/* Interface type: preview, rename etc */
NBPluginInterface::Type NBAVPlugin::type( Interface ) {

	return NBPluginInterface::Enhancement;
};

/* Plugin load contexts */
NBPluginInterface::Contexts NBAVPlugin::contexts( Interface ) {

	return Contexts() << NBPluginInterface::File;
};

/* Mimetypes handled by the plugin */
QStringList NBAVPlugin::mimetypes() {

	QStringList mimeList;
	Q_FOREACH( QMimeType type, mimeDb.allMimeTypes() ) {

		if ( type.name().startsWith( "audio" ) )
			mimeList << type.name();

		else if ( type.name().startsWith( "video" ) )
			mimeList << type.name();
	}

	mimeList.removeDuplicates();
	mimeList.removeAll( "video/mng" );

	return mimeList;
};

/* Invoke slots called called by triggering the actions */
void NBAVPlugin::actionTrigger( Interface, QString, QStringList nodes ) {

	if ( ( nodes.count() == 1 ) and isFile( nodes.at( 0 ) ) ) {

		NBAVPlayer *player = new NBAVPlayer( nodes.at( 0 ) );
		player->exec();
	}
};

void NBAVPlugin::setCaller( QWidget *caller ) {

	mParent = caller;
};

#if QT_VERSION < 0x050000
	Q_EXPORT_PLUGIN2( AVPlugin, NBAVPlugin );
#endif
