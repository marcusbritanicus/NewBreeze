/*
	*
	* NBButton.hpp - The NBButton Class, a simple replacement for QPushButton and QToolButton
	*
*/

#pragma once

#include <Global.hpp>
#include <NBGuiWidgets.hpp>

class NBButton : public QAbstractButton {
	Q_OBJECT

	public:
		/* A button */
		NBButton( QWidget *parent = NULL );

		/* ToolButton */
		NBButton( QIcon, QWidget *parent = NULL  );

		/* PushButton */
		NBButton( QString, QWidget *parent = NULL  );

		/* PushButton */
		NBButton( QIcon, QString, QWidget *parent = NULL  );

		/* Get/Set text */
		QString text();
		void setText( QString );

		/* Get/Set icon */
		QIcon icon();
		void setIcon( QIcon );

		/* Get/Set icon size */
		QSize iconSize();
		void setIconSize( QSize );

		/* Get/Set a menu */
		QMenu *menu();
		void setMenu( QMenu* );

		/* Shortcut handling */
		QKeySequence shortcut();
		void setShortcut( QKeySequence );

		/* Button Size */
		QSize size();
		void resize( QSize );
		void resize( int, int );

		/* Size hint */
		QSize sizeHint() const;

	public slots:
		void showMenu();

	private:
		void initPrivateVariables();

		void paintToolButton( QPainter* );
		void paintPushButton( QPainter* );

		/* Icon/Text */
		QIcon mIcon;
		QString mText;

		/* Icon Size */
		QSize mIconSize;

		/* Button Size */
		int mHeight, mWidth;

		/* Button type: 0 tool, 1 push */
		int m_ButtonType;

		/* Menu */
		QMenu *mMenu;
		bool m_HasMenu;

		/* Shortcut */
		QKeySequence mShortcut;
		int mShortcutId;

		/* Menu button was pressed */
		bool m_MenuButtonPressed;

		/* This button was pressed and held */
		bool m_Pressed;

		/* Animtaion timer */
		QBasicTimer animateTimer;

	private slots:
		void animateClick();

	protected:
		bool event( QEvent* );
		void timerEvent( QTimerEvent* );

		void mousePressEvent( QMouseEvent* );
		void mouseReleaseEvent( QMouseEvent* );

		void paintEvent( QPaintEvent* );

	Q_SIGNALS:
		void pressed();
		void released();

		void clicked();
};
