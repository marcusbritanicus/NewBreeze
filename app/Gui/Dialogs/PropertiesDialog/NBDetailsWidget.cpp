/*
	*
	* NBDetailsWidget.cpp - Properties of the selected object
	*
*/

#include "NBDetailsWidget.hpp"
#include "NBMediaInfo.hpp"

NBDetailsWidget::NBDetailsWidget( QStringList paths, QWidget *parent ) : QWidget( parent ) {

	if ( paths.count() > 1 ) {
		QLabel *lbl = new QLabel( "Cannot show details of more than one node at once. Please select a single node." );
		lbl->setWordWrap( true);
		lbl->setAlignment( Qt::AlignCenter );

		QVBoxLayout *lyt = new QVBoxLayout();
		lyt->addStretch();
		lyt->addWidget( lbl );
		lyt->addStretch();

		setLayout( lyt );

		return;
	}

	QVBoxLayout *lyt = new QVBoxLayout();

	QLabel *lbl = new QLabel( this );
	lbl->setWordWrap( true);

	QString mType = mimeDb.mimeTypeForFile( paths.at( 0 ) ).name();
	bool okay = mType.startsWith( "image" );
	okay |= mType.startsWith( "audio" );
	okay |= mType.startsWith( "video" );
	okay &= not mType.contains( "djv" );

	if ( not okay ) {
		lbl->setAlignment( Qt::AlignCenter );
		lbl->setText( "There are no details about this file which can be shown." );

		lyt->addWidget( lbl );
		setLayout( lyt );

		return;
	}

	#ifdef USE_MEDIAINFO
		NBMediaInfoModel *media = new NBMediaInfoModel( paths.at( 0 ) );
		lbl->setText( media->quickInfo() );
	#else
		lbl->setAlignment( Qt::AlignCenter );
		lbl->setText(
			"There are no details about this file which can be shown. "
			"It looks like NewBreeze3 was compiled without libmediainfo support."
			"Please compile NewBreeze3 with mediainfo support to see datails of files of this kind."
		);
	#endif

	lyt->addWidget( lbl );
	setLayout( lyt );
};
