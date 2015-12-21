/*
	*
	* NBCrumbsBar.hpp - NBCrumbsBar.cpp Header
	*
*/

#pragma once

#include <Global.hpp>
#include <NBTools.hpp>
#include <NBCrumb.hpp>
#include <FlowLayout.hpp>
#include <NBExtraCrumb.hpp>
#include <NBGuiWidgets.hpp>

/* Menu Popup to show the full path */
class NBCrumbsBarX : public QWidget {
	Q_OBJECT

	public:
		NBCrumbsBarX( QString path, QString cPath, QWidget *parent );

		int heightForWidth( int );

	private:
		FlowLayout *lyt;

	protected:
		void paintEvent( QPaintEvent *pEvent );

	Q_SIGNALS:
		void loadPath( QString );
};

class NBCrumbsBar : public QWidget {
	Q_OBJECT

	public:
		NBCrumbsBar( QWidget *parent );
		void setCurrentDirectory( QString );

	private:
		QString mPath;
		QString mCurrentPath;

		int m_CurrentCrumb;

		QWidget *crumbsHolder;
		NBExtraCrumb *xCrumb;

		/* Ugly hack to handle sidepanel animation */
		QTimer *resizeTimer;

	private slots:
		void doLayout();

		void showCrumbsBarX();

	protected:
		void resizeEvent( QResizeEvent *rEvent );

	Q_SIGNALS:
		void openLocation( QString );
};
