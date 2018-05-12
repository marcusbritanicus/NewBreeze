/*
	*
	* NBTermPlugin.cpp - NewBreeze MarkDownPreviewWidget Class
	*
*/

#include "NBTermPlugin.hpp"
#include "NBTerminal.hpp"

/* Name of the plugin */
QString NBTermPlugin::name() {

	return "TermPlugin";
};

/* The plugin version */
QString NBTermPlugin::version() {

	return "1.0.0";
};

/* The QAction hooks for menus/toolbars */
QList<QAction*> NBTermPlugin::actions( Interface, QStringList nodes ) {

	if ( ( nodes.count() == 1 ) and isDir( nodes.at( 0 ) ) ) {
		QAction *act = new QAction( QIcon( ":/icons/terminal.png" ), "&Peek", this );

		NBTerminal *term = new NBTerminal( nodes.at( 0 ) );
		connect( act, SIGNAL( triggered() ), term, SLOT( showMaximized() ) );

		return QList<QAction*>() << act;
	}

	return QList<QAction*>();
};

/* Interface type: preview, rename etc */
NBPluginInterface::Interfaces NBTermPlugin::interfaces() {

	return  NBPluginInterface::Interfaces() << NBPluginInterface::TerminalInterface;
};

/* Interface type: preview, rename etc */
NBPluginInterface::Type NBTermPlugin::type( Interface ) {

	return NBPluginInterface::Enhancement;
};

/* Plugin load contexts */
NBPluginInterface::Contexts NBTermPlugin::contexts( Interface ) {

	return Contexts() << NBPluginInterface::Dir;
};

/* Mimetypes handled by the plugin */
QStringList NBTermPlugin::mimetypes() {

	return QStringList() << "inode/directory";
};

/* Invoke slots called called by triggering the actions */
void NBTermPlugin::actionTrigger( Interface, QString, QStringList nodes ) {

	/* Just open a terminal in a given directory */
	if ( ( nodes.count() == 1 ) and isDir( nodes.at( 0 ) ) ) {
		NBTerminal *term = new NBTerminal( nodes.at( 0 ) );
		term->showMaximized();
	}

	/* Execute a command */
	else {
		NBTerminal *term = new NBTerminal( nodes.at( 0 ), nodes.at( 1 ) );
		term->showMaximized();
	}

};

void NBTermPlugin::setCaller( QWidget *caller ) {

	mParent = caller;
};

#if QT_VERSION < 0x050000
	Q_EXPORT_PLUGIN2( TermPlugin, NBTermPlugin );
#endif
