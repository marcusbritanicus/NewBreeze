/*
	*
	* NBMarkDownPreview.hpp - NewBreeze MarkDownPreviewWidget Class Header
	*
*/

#ifndef NBPREVIEWWIDGET_HPP
#define NBPREVIEWWIDGET_HPP

#include <NBPreviewInterface.hpp>
#include <Global.hpp>
#include <QtWebKit>

QT_BEGIN_NAMESPACE
class QString;
class QStringList;
class QWidget;
QT_END_NAMESPACE

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

class NBMarkDownPreview : public QObject, public NBPreviewInterface {

	Q_OBJECT
	Q_INTERFACES( NBPreviewInterface )

	public:
		QStringList mimeTypesHandled() const;
		QDialog* getPreviewWidget( const QString & );

};

#endif
