/*
	*
	* NBMarkDownPreview.hpp - NewBreeze MarkDownPreviewWidget Class Header
	*
*/

#pragma once

#include "Global.hpp"

#if QT_VERSION >= 0x050000
	#include <QtWebKitWidgets>
#endif

class NBWebWatch : public QDialog {
	Q_OBJECT

	public :
		NBWebWatch( QString path = QDir::homePath(), QWidget *parent = 0 );

	private :
		void createGUI();
		void setWindowProperties();

		QString path;
		QWebView *peekWidgetBase;

	private slots :
		void loadDocument();
		void openInExternal();

	protected :
		void keyPressEvent( QKeyEvent *keyEvent );
		void changeEvent( QEvent *cEvent );

		void paintEvent( QPaintEvent *pEvent );
};

class NBMarkDownPreview : public QObject, NBPluginInterface {
	Q_OBJECT
	#if QT_VERSION >= 0x050000
		Q_PLUGIN_METADATA( IID "org.NewBreeze.NBPluginInterface" FILE "MarkDownPreview5.json" )
	#endif
	Q_INTERFACES( NBPluginInterface )

	public:
		/* Name of the plugin */
		 QString name();

		/* The plugin version */
		 QString version();

		/* The QAction */
		 QList<QAction*> actions( QStringList );

		/* Interface type: preview, rename etc */
		 Interface interface();

		/* Interface type: preview, rename etc */
		 Type type();

		/* Plugin load context */
		 Contexts contexts();

		/* Mimetypes handled by the plugin */
		 QStringList mimetypes();

		/* Store the called widget pointer */
		 void setCaller( QWidget *caller );
};
