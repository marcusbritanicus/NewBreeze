/*
	*
	* NBGuiWidgets.hpp - NBGuiWidgets.cpp header
	*
*/

#pragma once
#ifndef NBGUIWIDGETS_HPP
#define NBGUIWIDGETS_HPP

#include <Global.hpp>
#include <NBTools.hpp>
#include <NBLabels.hpp>

class NBLineEdit : public QLineEdit {
	Q_OBJECT

	public:
		NBLineEdit( QWidget* );

	protected:
		void paintEvent( QPaintEvent* );
};

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

class Separator : public QWidget {
	Q_OBJECT

	public:
		static QWidget* vertical( QWidget *parent = 0 );
		static QWidget* horizontal( QWidget *parent = 0 );

	private:
		enum Mode {
			Horizontal,
			Vertical
		};

		Separator( Separator::Mode mode, QWidget *parent = 0 );

		QLinearGradient vGrad, hGrad;
		Separator::Mode mMode;

	protected:
		void paintEvent( QPaintEvent * );
		void resizeEvent( QResizeEvent * );
};

class NBSpacer : public QWidget {
	Q_OBJECT

	public:
		static QWidget* vertical( int spacing = 1, QWidget *parent = 0 );
		static QWidget* horizontal( int spacing = 1, QWidget *parent = 0 );

	private:
		enum Mode {
			Horizontal,
			Vertical
		};

		NBSpacer( NBSpacer::Mode mode, int spacing = 1, QWidget *parent = 0 );
		NBSpacer::Mode mMode;

	protected:
		void paintEvent( QPaintEvent * );
		void resizeEvent( QResizeEvent * );
};

#endif
