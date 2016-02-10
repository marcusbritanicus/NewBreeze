/*
	*
	* NBSStyleWidget.hpp - NBSStyleWidget.cpp header
	*
*/

#pragma once

#include <Global.hpp>
#include <NBButtons.hpp>

class NBSSPreviewWidget;

class NBSStyleWidget : public QWidget {
	Q_OBJECT

	public:
		NBSStyleWidget( QWidget* );

	private:
		QComboBox *styleListCB;
		NBSegmentButton *segBtn;

		NBSSPreviewWidget *preview;

	private slots:
		void stylePreview( const QString );
		void applyStyle();
};

class NBSSPreviewWidget : public QWidget {

	public:
		NBSSPreviewWidget( QWidget* );
		void showStyle( const QString );
};
