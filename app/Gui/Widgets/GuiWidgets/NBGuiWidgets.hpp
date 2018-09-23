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

	protected:
		void paintEvent( QPaintEvent * );
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
