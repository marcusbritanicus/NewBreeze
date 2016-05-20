/*
	*
	* TextEditorPlugin.cpp - NewBreeze TextEditorPlugin Class
	*
*/

#include <NBTextEditPlugin.hpp>

/* Name of the plugin */
QString TextEditorPlugin::name() {

	return "TextEditor";
};

/* The plugin version */
QString TextEditorPlugin::version() {

	return "3.0.0";
};

/* The QAction hooks for menus/toolbars */
QList<QAction*> TextEditorPlugin::actions( QStringList nodes ) {

	if ( not nodes.count() )
		return QList<QAction*>();

	QStringList mimes;
	mimes << mimeDb.mimeTypeForFile( nodes.at( 0 ) ).name() << mimeDb.mimeTypeForFile( nodes.at( 0 ) ).allAncestors();
	if ( mimes.contains( "text/plain" ) ) {
		Editor = new TextEditor( nodes.at( 0 ), mParent );
		QAction *act = new QAction( QIcon::fromTheme( "document-edit" ), "&Edit file", this );
		connect( act, SIGNAL( triggered() ), Editor, SLOT( show() ) );

		return QList<QAction*>() << act;
	}

	return QList<QAction*>();
};

/* Interface type: preview, rename etc */
NBPluginInterface::Interface TextEditorPlugin::interface() {

	return NBPluginInterface::ActionInterface;
};

/* Interface type: preview, rename etc */
NBPluginInterface::Type TextEditorPlugin::type() {

	return NBPluginInterface::Enhancement;
};

/* Plugin load contexts */
NBPluginInterface::Contexts TextEditorPlugin::contexts() {

	return Contexts() << NBPluginInterface::File;
};

/* Mimetypes handled by the plugin */
QStringList TextEditorPlugin::mimetypes() {

	return QStringList() << "*";
};

void TextEditorPlugin::setCaller( QWidget *caller ) {

	mParent = caller;
};

#if QT_VERSION < 0x050000
	Q_EXPORT_PLUGIN2( TextEditor, TextEditorPlugin );
#endif
