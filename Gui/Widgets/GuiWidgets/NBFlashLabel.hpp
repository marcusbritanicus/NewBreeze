/*
	*
	* NBFlashLabel.hpp - NBFlashLabel.cpp header file
	*
*/

#ifndef NBFLASHLABEL_HPP
#define NBFLASHLABEL_HPP

#include <Global.hpp>
#include <NBDeleteManager.hpp>
#include <NBMessageDialog.hpp>
#include <NBConfirmDeleteDialog.hpp>

class NBFlashLabel : public QLabel {
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
		NBFlashLabel();

		/* Override the QLabel pixmap property handlers */
		QPixmap pixmap();
		void setPixmap( QPixmap );

		/* Override the QLabel text property handlers */
		QString text();
		void setText( QString );

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
		QPixmap mPixmap = QPixmap();
		QString mText = QString();

		QTimer timer;

		qint64 alpha = 0;
		qint64 mAlphaDelta = 30;
		QColor color = QColor( 255, 255, 255, 0 );

		int currentStep = 0;
		int flashSteps = 10;
		int mFlashDuration = 200;

		bool flash = false;
		int flashesCompleted = 0;
		int maxFlashes = 2;

	protected :
		void paintEvent( QPaintEvent* );
		void mousePressEvent( QMouseEvent* );

		void enterEvent( QEvent* );
		void leaveEvent( QEvent* );

	public slots:
		void flashLabel();

	Q_SIGNALS:
		void clicked();
		void entered();
		void exited();
};

class NBDeleteLabel : public QLabel {
	Q_OBJECT

	public:
		NBDeleteLabel();

	private :
		QPixmap mPixmap = QPixmap();

		QTimer timer;

		QColor color = QColor( 255, 0, 0, 150 );

		int currentStep = 0;
		int flashSteps = 50;
		int radius = 0;

		bool flash = false;

	protected :
		void paintEvent( QPaintEvent* );
		void mousePressEvent( QMouseEvent* );

		void dragEnterEvent( QDragEnterEvent* );
		void dropEvent( QDropEvent* );

	private slots:
		void handleDeleteFailure( QStringList, QStringList );
		void flashLabel();

	Q_SIGNALS:
		void showTrash();
		void clicked();
};

#endif
