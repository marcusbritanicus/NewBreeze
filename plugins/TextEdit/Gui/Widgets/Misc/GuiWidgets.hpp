/*
    *
    * GuiWidgets.cpp -  TextEditor Gui Widgets Header
    *
*/

#pragma once

#include <Global.hpp>

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

class Spacer {
	public:
		static QWidget *vertical( int space, QWidget *parent = 0 );
		static QWidget* horizontal( int space, QWidget *parent = 0 );
};

class LedLight : public QWidget {
	Q_OBJECT

	public:
		LedLight( quint64 radius, QWidget *parent = NULL );

		bool isSafe();
		void setSafe( bool );

	private:
		bool safety;
		quint64 mRadius;

	protected:
		void paintEvent( QPaintEvent * );
};
