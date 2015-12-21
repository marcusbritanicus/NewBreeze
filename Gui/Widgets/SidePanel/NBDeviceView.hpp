/*
	*
	* NBDeviceView.hpp - NBDeviceView.cpp header file
	*
*/

#pragma once

#include <Global.hpp>
#include <NBDeviceInfo.hpp>
#include <NBDeviceManager.hpp>
#include <NBAnimations.hpp>

class NBDeviceView;

class NBDevicesIcon : public QWidget {
	Q_OBJECT

	/* Color properties */
	Q_PROPERTY( QColor flashColor READ flashColor WRITE setFlashColor )
	Q_PROPERTY( qint64 alphaDelta READ alphaDelta WRITE setAlphaDelta )

	/* Flash properties */
	Q_PROPERTY( int flashDuration READ flashDuration WRITE setFlashDuration )
	Q_PROPERTY( int flashesNumber READ numberOfFlashes WRITE setNumberOfFlashes )

	/* Flash Frames */
	Q_PROPERTY( int flashFrames READ flashFrames WRITE setFlashFrames )

	public:
		NBDevicesIcon( QWidget* );

		/* Override the QLabel pixmap property handlers */
		QPixmap pixmap();
		void setPixmap( QPixmap );

		/* Color property handlers */
		QColor flashColor();
		void setFlashColor( QColor );

		/* Alpha property handlers */
		qint64 alphaDelta();
		void setAlphaDelta( qint64 );

		/* Duration property handlers */
		int flashDuration();
		void setFlashDuration( int );

		/* FlashesNumber property handlers */
		int numberOfFlashes();
		void setNumberOfFlashes( int );

		/* FlashesNumber property handlers */
		int flashFrames();
		void setFlashFrames( int );

	private :
		QBasicTimer delayTimer;
		QTimer timer;

		QPixmap mPixmap;

		qint64 alpha;
		qint64 mAlphaDelta;
		QColor color;

		int currentStep;
		int flashSteps;
		int mFlashDuration;;

		bool flash;
		int flashesCompleted;
		int maxFlashes;

		NBWidthAnimation *anim2;

		/* should the flash be colored */
		// bool colorFlash;

		NBDeviceView *devView;

	private slots:
		void showDevices();
		void hideDevices();

	protected :
		void paintEvent( QPaintEvent* );
		void mousePressEvent( QMouseEvent* );
		void mouseMoveEvent( QMouseEvent* );

		void enterEvent( QEvent* );		// => Expand
		void leaveEvent( QEvent* );		// => Contract

		void timerEvent( QTimerEvent * );

	public slots:
		void flashLabel();
		void flashLabel( QColor );

	Q_SIGNALS:
		void driveClicked( QString );
};

class NBDeviceView : public QWidget {
	Q_OBJECT

	public:
		NBDeviceView( QWidget* );

		/* Repopulate on new device add */
		void repopulate();

		/* Ideal width for the deviceView */
		int idealWidth();

	private :
		void computeIdealWidth();

		QBasicTimer closeTimer;

		/* Label, MountPoint piar for each device */
		QMap<int, QString> devRectMap;
		mutable QPoint cursor;

		int mIdealWidth;

	protected :
		void paintEvent( QPaintEvent* );

		void mousePressEvent( QMouseEvent* );
		void mouseMoveEvent( QMouseEvent* );

		void keyPressEvent( QKeyEvent * );

		void enterEvent( QEvent* );		// => Expand
		void leaveEvent( QEvent* );		// => Contract

		void timerEvent( QTimerEvent * );

	Q_SIGNALS:
		void driveClicked( QString );
		void close();
};
