/*
	*
	* NBGuiWidgets.hpp - NBGuiWidgets.cpp header
	*
*/

#pragma once

#include "Global.hpp"
#include "NBLabels.hpp"

class NBLineEdit : public QLineEdit {
	Q_OBJECT

	public:
		NBLineEdit( QWidget* );
};

class NBDriveLabel : public QWidget {
	Q_OBJECT

	public:
		NBDriveLabel( const QString path, QWidget * );
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
		NBDriveInfo( qint64 used, qint64 total, QWidget * );
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
