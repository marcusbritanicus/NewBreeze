/*
	*
	* TextEditor.cpp - NewBreeze File Folder Encryption Class
	*
*/

#include <NBTextEditPlugin.hpp>

/* Name of the plugin */
QString TextEditor::name() {

	return "TextEditor";
};

/* The plugin version */
QString TextEditor::version() {

	return "3.0.0";
};

/* The QAction hooks for menus/toolbars */
QList<QAction*> TextEditor::actions( QStringList nodes ) {

	if ( not nodes.count() )
		return QList<QAction*>();

	QStringList mimes;
	mimes << mimeDb.mimeTypeForFile( nodes.at( 0 ) ).name() << mimeDb.mimeTypeForFile( nodes.at( 0 ) ).allAncestors();
	if ( mimes.contains( "text/plain" ) ) {
		wordy = new Wordy( nodes.at( 0 ), mParent );
		QAction *act = new QAction( QIcon::fromTheme( "document-edit" ), "&Edit file", this );
		connect( act, SIGNAL( triggered() ), wordy, SLOT( show() ) );

		return QList<QAction*>() << act;
	}

	return QList<QAction*>();
};

/* Interface type: preview, rename etc */
NBPluginInterface::Interface TextEditor::interface() {

	return NBPluginInterface::ActionInterface;
};

/* Interface type: preview, rename etc */
NBPluginInterface::Type TextEditor::type() {

	return NBPluginInterface::Enhancement;
};

/* Plugin load contexts */
NBPluginInterface::Contexts TextEditor::contexts() {

	return Contexts() << NBPluginInterface::File;
};

/* Mimetypes handled by the plugin */
QStringList TextEditor::mimetypes() {

	return QStringList() << "*";
};

void TextEditor::setCaller( QWidget *caller ) {

	mParent = caller;
};

#if QT_VERSION < 0x050000
	Q_EXPORT_PLUGIN2( TextEditor, TextEditor );
#endif
