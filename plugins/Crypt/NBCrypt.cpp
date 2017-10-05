/*
	*
	* NBCrypt.cpp - NewBreeze File Folder Encryption Class DUMMY
	*
*/

#include "NBCrypt.hpp"

#warning "This is a dummy implementation to replace the existing plugin. "				\
         "All the NBCrypt Plugin functionality has been merged with the main app."

/* Name of the plugin */
QString NBCrypt::name() {

	return "Crypt";
};

/* The plugin version */
QString NBCrypt::version() {

	return "3.0.0-dummy";
};

/* The QAction hooks for menus/toolbars */
QList<QAction*> NBCrypt::actions( Interface, QStringList ) {

	return QList<QAction*>();
};

/* Interface type: preview, rename etc */
NBPluginInterface::Interfaces NBCrypt::interfaces() {

	return NBPluginInterface::Interfaces();
};

/* Interface type: preview, rename etc */
NBPluginInterface::Type NBCrypt::type( NBPluginInterface::Interface ) {

	return NBPluginInterface::Enhancement;
};

/* Plugin load contexts */
NBPluginInterface::Contexts NBCrypt::contexts( NBPluginInterface::Interface ) {

	return Contexts() << NBPluginInterface::None;
};

/* Mimetypes handled by the plugin */
QStringList NBCrypt::mimetypes() {

	return QStringList() << "invalid/none";
};

/* Invoke slots called called by triggering the actions */
void NBCrypt::actionTrigger( Interface, QString, QStringList ) {

};

void NBCrypt::setCaller( QWidget *caller ) {

	mParent = caller;
};

#if QT_VERSION < 0x050000
	Q_EXPORT_PLUGIN2( NBCrypt, NBCrypt );
#endif
