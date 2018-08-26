/*
	*
	* NBDirectoryView.cpp - NewBreeze DirectoryView class
	*
*/

#include "NBDirectoryView.hpp"

void NBDirViewDelegate::paint( QPainter* painter, const QStyleOptionViewItem &styleOpts, const QModelIndex &idx ) const {

	if ( not idx.isValid() )
		QStyledItemDelegate::paint( painter, styleOpts, idx );

	#if QT_VERSION > 0x050000
		QStyleOptionViewItem newOpts( styleOpts );
	#else
		QStyleOptionViewItemV4 newOpts( styleOpts );
	#endif

	QString path( idx.data( Qt::UserRole +1 ).toString() );
	if ( isReadable( path ) and isWritable( path ) ) {
		newOpts.palette.setColor( QPalette::WindowText, Qt::black );
		newOpts.palette.setColor( QPalette::Text, Qt::black );
	}

	else if ( isReadable( path ) ) {
		newOpts.palette.setColor( QPalette::WindowText, Qt::gray );
		newOpts.palette.setColor( QPalette::Text, Qt::gray );
	}

	else {
		newOpts.palette.setColor( QPalette::WindowText, Qt::darkRed );
		newOpts.palette.setColor( QPalette::Text, Qt::darkRed );
	}

	QStyledItemDelegate::paint( painter, newOpts, idx );
};

NBDirectoryView::NBDirectoryView( QWidget *parent ) : QTreeView( parent ) {

	// Model
	mdl = new QFileSystemModel();
	mdl->setFilter( QDir::Dirs | QDir::NoDotAndDotDot );
	mdl->sort( 0, Qt::AscendingOrder );

	setCurrentIndex( mdl->setRootPath( "/home/cosmos/" ) );

	setModel( mdl );

	// Hide Columns
	for( int c = 1; c < mdl->columnCount(); c++ )
		setColumnHidden( c, true );

	// Internal Object Name
	setObjectName( "mainList" );

	/* Styling */
	setStyleSheet( "#mainList{ border:none; }" );

	/* ItemDelegate */
	setItemDelegate( new NBDirViewDelegate() );
};

QString NBDirectoryView::currentBranch() const {

	return currentIndex().data( Qt::UserRole + 1 ).toString();
};

void NBDirectoryView::setCurrentBranch( QString path ) {

	if ( not exists( path ) or not isReadable( path ) )
		return;

	path += "/";

	setCurrentIndex( mdl->setRootPath( path ) );
	expand( currentIndex() );
};
