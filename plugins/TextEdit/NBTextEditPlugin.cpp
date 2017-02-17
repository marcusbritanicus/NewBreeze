/*
	*
	* TextEditorPlugin.cpp - NewBreeze TextEditorPlugin Class
	*
*/

#include "NBTextEditPlugin.hpp"

/* Name of the plugin */
QString TextEditorPlugin::name() {

	return "TextEditor";
};

/* The plugin version */
QString TextEditorPlugin::version() {

	return "3.0.0";
};

/* The QAction hooks for menus/toolbars */
QList<QAction*> TextEditorPlugin::actions( Interface iface, QStringList nodes ) {

	if ( not nodes.count() )
		return QList<QAction*>();

	Editor = new TextEditor( nodes.at( 0 ), mParent );

	if ( iface == NBPluginInterface::PreviewInterface ) {
		Editor->setPreviewMode();
		QAction *act = new QAction( QIcon( ":/icons/peek.png" ), "Pee&k", this );
		connect( act, SIGNAL( triggered() ), Editor, SLOT( show() ) );

		return QList<QAction *>() << act;
	}

	else if ( iface == NBPluginInterface::ActionInterface ) {
		QAction *act = new QAction( QIcon::fromTheme( "document-edit" ), "&Edit file", this );
		connect( act, SIGNAL( triggered() ), Editor, SLOT( show() ) );

		return QList<QAction *>() << act;
	}

	else {
		return QList<QAction*>();
	}

	return QList<QAction*>();
};

/* Interface type: preview, rename etc */
NBPluginInterface::Interfaces TextEditorPlugin::interfaces() {

	return NBPluginInterface::Interfaces() << NBPluginInterface::ActionInterface << NBPluginInterface::PreviewInterface;
};

/* Interface type: preview, rename etc */
NBPluginInterface::Type TextEditorPlugin::type( Interface ) {

	return NBPluginInterface::Enhancement;
};

/* Plugin load contexts */
NBPluginInterface::Contexts TextEditorPlugin::contexts( Interface ) {

	return Contexts() << NBPluginInterface::File;
};

/* Mimetypes handled by the plugin */
QStringList TextEditorPlugin::mimetypes() {

	QStringList mimeList;
	Q_FOREACH( QMimeType type, mimeDb.allMimeTypes() ) {

		if ( type.name().startsWith( "text" ) )
			mimeList << type.name();

		else if ( type.allAncestors().contains( "text/plain" ) )
			mimeList << type.name();
	}

	/* No HTML and Markdown */
	mimeList.removeAll( "text/html" );
	mimeList.removeAll( "text/x-extension-html" );
	mimeList.removeAll( "application/xml" );
	mimeList.removeAll( "text/xhtml+xml" );
	mimeList.removeAll( "text/markdown" );
	mimeList.removeAll( "text/x-markdown" );

	return mimeList;
};

/* Invoke slots called called by triggering the actions */
void TextEditorPlugin::actionTrigger( Interface iface, QString, QStringList nodes ) {

	if ( not nodes.count() )
		return;

	Editor = new TextEditor( nodes.at( 0 ), mParent );
	QAction *act = new QAction( QIcon::fromTheme( "document-edit" ), "&Edit file", this );
	connect( act, SIGNAL( triggered() ), Editor, SLOT( show() ) );

	if ( iface == NBPluginInterface::PreviewInterface )
		Editor->setPreviewMode();

	Editor->show();
};

void TextEditorPlugin::setCaller( QWidget *caller ) {

	mParent = caller;
};

#if QT_VERSION < 0x050000
	Q_EXPORT_PLUGIN2( TextEditor, TextEditorPlugin );
#endif
