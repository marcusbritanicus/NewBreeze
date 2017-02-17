/*
	*
	* NBDirectoryView.cpp - NewBreeze DirectoryView class
	*
*/

#include "NBDirectoryView.hpp"

void NBDirViewDelegate::paint( QPainter* painter, const QStyleOptionViewItem &styleOpts, const QModelIndex &idx ) const {

	if ( not idx.isValid() )
		QStyledItemDelegate::paint( painter, styleOpts, idx );

	QStyleOptionViewItemV4 newOpts( styleOpts );

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
	mdl = new NBDirTreeModel();
	setModel( mdl );

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

	QStringList nodeList;
	nodeList << path.split( "/", QString::SkipEmptyParts );

	/* First Index is the root index, we expand it */
	QModelIndex idx = mdl->index( 0, 0, QModelIndex() );
	expand( idx );

	Q_FOREACH( QString node, nodeList ) {
		idx = mdl->index( node, idx );
		expand( idx );
	}

	setCurrentIndex( idx );
};
