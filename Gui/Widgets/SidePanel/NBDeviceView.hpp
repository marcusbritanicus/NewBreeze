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

class NBDeviceAction;
class NBDeviceMenu;

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
		QBasicTimer closeTimer;
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

		NBDeviceMenu *devView;

	private slots:
		void showDevices();
		void clickDrive( QAction* );

	protected :
		void paintEvent( QPaintEvent* );
		void mousePressEvent( QMouseEvent* );
		void mouseMoveEvent( QMouseEvent* );

		void enterEvent( QEvent* );

		void timerEvent( QTimerEvent * );

	public slots:
		void flashLabel();
		void flashLabel( QColor );

	Q_SIGNALS:
		void driveClicked( QString );
};

class NBDeviceMenu : public QMenu {
	Q_OBJECT

	public:
		NBDeviceMenu( QWidget* );

	protected:
		void changeEvent( QEvent* );
};

class NBDeviceAction : public QWidget {
	Q_OBJECT

	public:
		NBDeviceAction( NBDeviceInfo );
		QString mountPoint();

	private:
		QString mMountPoint;
		quint64 percentUsed;
		QIcon icon;
		QString mDeviceLabel;

		bool select;

	protected :
		void paintEvent( QPaintEvent* );
		void enterEvent( QEvent* );
		void leaveEvent( QEvent* );
};
