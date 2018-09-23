/*
	*
	* NBDefaultsWidget.cpp - Properties of the selected object
	*
*/

#include "NBPreviewWidget.hpp"

NBPreviewWidget::NBPreviewWidget( QStringList paths, QWidget *parent ) : QWidget( parent ) {

	/* Set movie to NULL */
	movie = 0;

	if ( paths.count() > 1 ) {
		QLabel *lbl = new QLabel( "Cannot preview more than one node at a time. Please select a single node." );
		lbl->setWordWrap( true);

		QVBoxLayout *lyt = new QVBoxLayout();
		lyt->addStretch();
		lyt->addWidget( lbl );
		lyt->addStretch();

		setLayout( lyt );

		return;
	}

	QMimeType mType = mimeDb.mimeTypeForFile( paths.at( 0 ) );
	QVBoxLayout *lyt = new QVBoxLayout();

	/* GIF/MNG */
	if ( mType.name().contains( "gif" ) or mType.name().contains( "mng" ) ) {
		QLabel *lbl = new QLabel();
		lbl->setAlignment( Qt::AlignCenter );

		movie = new QMovie( paths.at( 0 ) );
		lbl->setMovie( movie );
		movie->start();

		lyt->addStretch();
		lyt->addWidget( lbl );
		lyt->addStretch();
	}

	/* Plain Images */
	else if ( mType.name().startsWith( "image" ) and not mType.name().contains( "djv" ) ) {
		QLabel *lbl = new QLabel();
		lbl->setFixedSize( QSize( 380, 310 ) );
		lbl->setAlignment( Qt::AlignCenter );

		QPixmap pix( paths.at( 0 ) );
		if ( pix.width() > 380 or pix.height() > 310 )
			pix = pix.scaled( QSize( 380, 310 ), Qt::KeepAspectRatio, Qt::SmoothTransformation );

		lbl->setPixmap( pix );

		lyt->addStretch();
		lyt->addWidget( lbl );
		lyt->addStretch();
	}

	/* Text */
	else if ( mType.name().startsWith( "text" ) or mType.allAncestors().contains( "text/plain" ) ) {
		QTextBrowser *txt= new QTextBrowser();
		QFile f( paths.at( 0 ) );
		f.open( QFile::ReadOnly );
		txt->setText( QString::fromUtf8( f.readAll() ) );
		f.close();

		lyt->addWidget( txt );
	}

	/* Folder */
	else if ( mType.name() == "inode/directory" ) {
		QListView *view = new QListView();
		view->setIconSize( QSize( 24, 24 ) );
		view->setViewMode( QListView::ListMode );
		view->setFlow( QListView::TopToBottom );

		QFileSystemModel *fsm = new QFileSystemModel( view );
		view->setModel( fsm );
		view->setRootIndex( fsm->setRootPath( paths.at( 0 ) ) );

		QLabel *lbl = new QLabel( "Contents of <b>" + baseName( paths.at( 0 ) ) + "</b>:" );
		lbl->setWordWrap( true );

		lyt->addWidget( lbl );
		lyt->addWidget( view );
	}

	/* Not possible */
	else {
		QLabel *lbl = new QLabel( "Cannot preview this filetype natively." );
		lbl->setWordWrap( true);
		lbl->setAlignment( Qt::AlignCenter );

		lyt->addStretch();
		lyt->addWidget( lbl );
		lyt->addStretch();
	}

	setLayout( lyt );
};

void NBPreviewWidget::resizeEvent( QResizeEvent *rEvent ) {

	if ( movie and movie->fileName().count() ) {
		QPixmap pix( movie->fileName() );
		QSize pixSize( pix.size() );
		movie->setScaledSize( pixSize.scaled( rEvent->size(), Qt::KeepAspectRatio ) );
	}

	rEvent->accept();
};
