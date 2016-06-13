/*
	*
	* NBExtraCrumb.hpp - The Segmented Buttons Class Header
	*
*/

#pragma once

#include "Global.hpp"
#include "NBGuiWidgets.hpp"

class NBExtraCrumb : public QWidget {
	Q_OBJECT

	public:
		NBExtraCrumb( QWidget *parent );

		QSize sizeHint();
		QSize minimumSizeHint();

		void setRequired( bool );

	private:
		/* Menu button was pressed */
		bool m_Pressed;
		bool m_Required;

		/* FontMetrics */
		QFontMetrics *fm;

	protected:
		void mousePressEvent( QMouseEvent* );
		void mouseReleaseEvent( QMouseEvent* );

		void enterEvent( QEvent* );
		void leaveEvent( QEvent* );

		void paintEvent( QPaintEvent* );

	Q_SIGNALS:
		void clicked();
};
