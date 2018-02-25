/*
	*
	* NBMarkDownPreview.hpp - NewBreeze MarkDownPreviewWidget Class Header
	*
*/

#pragma once

#include "Global.hpp"

class Document : public QObject {
	Q_OBJECT

	public:
		Document( QString path ) {

			QFile f( path );
			f.open( QFile::ReadOnly );

			mText = QString::fromLocal8Bit( f.readAll() );
			f.close();
		};

		Q_INVOKABLE QString text() {

			return mText;
		};

	private:
		QString mText;
};

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
	Q_INTERFACES( NBPluginInterface )

	public:
		/* Name of the plugin */
		 QString name();

		/* The plugin version */
		 QString version();

		/* The QAction */
		 QList<QAction*> actions( Interface, QStringList );

		/* Interface type: preview, rename etc */
		 Interfaces interfaces();

		/* Interface type: preview, rename etc */
		 Type type( Interface );

		/* Plugin load context */
		 Contexts contexts( Interface );

		/* Mimetypes handled by the plugin */
		 QStringList mimetypes();

		/* Invoke slots called called by triggering the actions */
		void actionTrigger( Interface, QString, QStringList nodes );

		/* Store the called widget pointer */
		 void setCaller( QWidget *caller );
};
