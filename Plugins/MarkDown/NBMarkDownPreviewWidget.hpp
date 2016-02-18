/*
	*
	* NBMarkDownPreview.hpp - NewBreeze MarkDownPreviewWidget Class Header
	*
*/

#pragma once

#include <NBPreviewInterface.hpp>

#include <Global.hpp>
#include <QtWebKit>

#if QT_VERSION >= 0x050000
	#include <QtWebKitWidgets>
#endif

class NBWebWatch : public QDialog {
	Q_OBJECT

	public :
		NBWebWatch( QString path = QDir::homePath() );
		QString path;
		QWebView *peekWidgetBase;

	private :
		void createGUI();
		void setWindowProperties();

	private slots :
		void openInExternal();

	protected :
		void keyPressEvent( QKeyEvent *keyEvent );
		void changeEvent( QEvent *cEvent );

		void paintEvent( QPaintEvent *pEvent );
};

class NBMarkDownPreview : public QObject, NBPreviewInterface {
	Q_OBJECT
	#if QT_VERSION >= 0x050000
		Q_PLUGIN_METADATA( IID "org.NewBreeze.NBPreviewInterface" FILE "MarkDownPreview5.json" )
	#endif
	Q_INTERFACES( NBPreviewInterface )

	public:
		QStringList mimeTypesHandled() const;
		QDialog* getPreviewWidget( const QString & );
};
