/*
	*
	* NBGuiWidgets.hpp - NBGuiWidgets.cpp header
	*
*/

#ifndef NBGUIWIDGETS_HPP
#define NBGUIWIDGETS_HPP

#include <Global.hpp>

class NBSearchBar : public QWidget {
	Q_OBJECT

	public:
		NBSearchBar();
		QLineEdit *searchLE;

	private slots:
		void searchChanged( QString );

	Q_SIGNALS:
		void searchString( QString );
		void searchCleared();
};

class NBMenu : public QMenu {
	Q_OBJECT

	public:
		NBMenu( QWidget *parent = 0 );
		NBMenu( QString title, QWidget *parent = 0 );
};

class NBToolButton : public QToolButton {
	Q_OBJECT

	public:
		NBToolButton( QString themeIcon = QString(), QString customIcon = QString() );
};

class NBPushButton : public QPushButton {
	Q_OBJECT

	public:
		NBPushButton( QIcon icon, QString text );
};

class NBTitleIcon : public QLabel {
	Q_OBJECT

	public:
		NBTitleIcon( QString icon );

	private :
		QMenu menu;

	protected:
		virtual void mousePressEvent( QMouseEvent * );

	signals:
		void aboutNB();
		void aboutQt4();
};

class NBPicLabel : public QLabel {
	Q_OBJECT

	public:
		NBPicLabel( QPixmap, QString );
		QString Style;

	protected:
		virtual void mousePressEvent( QMouseEvent * );

	signals:
		void clicked();
};

class NBClickLabel : public QLabel {
	Q_OBJECT

	public:
		NBClickLabel( QPixmap icon = QPixmap() );
		NBClickLabel( QString icon = QString() );
		void setClickable( bool );
		QString Style;

	private :
		bool clickEnabled;

	protected:
		virtual void mousePressEvent( QMouseEvent * );

	signals:
		void clicked();
};

class Separator {

	public:
		static QWidget* vertical();
		static QWidget* horizontal();
};

class NBViewModeButton: public QToolButton {
	Q_OBJECT

	public:
		NBViewModeButton();
		QRadioButton *smallListBtn, *mediumListBtn, *tilesBtn, *smallIconsBtn, *normalIconsBtn, *largeIconsBtn, *hugeIconsBtn;
		QRadioButton *smallDetailsBtn, *normalDetailsBtn;
		QWidgetAction *smallListAct, *mediumListAct, *tilesAct, *smallIconsAct, *normalIconsAct, *largeIconsAct, *hugeIconsAct;
		QWidgetAction *smallDetailsAct, *normalDetailsAct;

	// private slots:
		// void actionHandler();

	Q_SIGNALS:
		void changeViewMode();
		void switchToNextView();
};

class NBInfoLabel : public QLabel {
	Q_OBJECT

	public:
		NBInfoLabel();
		void setText( QString, QString, QString );
};

class NBDriveLabel : public QWidget {
	Q_OBJECT

	public:
		NBDriveLabel( const QString path );
		~NBDriveLabel();

	private:
		QPainter *painter;
		QString label;

	protected:
		void paintEvent( QPaintEvent * );
};

class NBDriveInfo : public QFrame {
	Q_OBJECT

	public:
		NBDriveInfo( qint64 used, qint64 total );
		~NBDriveInfo();

	private:
		QPainter *painter;
		QString label;
		float percent;

	protected:
		virtual void paintEvent( QPaintEvent * );
};

QDialog* getMessageDialog( QString, QString, QString, QString, QString, QString btns = "O" );

#endif
