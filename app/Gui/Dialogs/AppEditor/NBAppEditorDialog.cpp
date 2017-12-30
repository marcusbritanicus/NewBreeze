/*
	*
	* NBAppEditorDialog.cpp - AppEditor class for NewBreeze
	*
*/

#include "NBAppEditorDialog.hpp"

void NBDesktopFileEditor::createGUI() {

};

NBMimeTypeSelector::NBMimeTypeSelector( QWidget *parent ) : QDialog( parent ) {

	populateGUI();

	setWindowTitle( "MimeType Selector" );
	setWindowIcon( QIcon::fromTheme( "application-x-executable" ) );

	QRect scrnSize = QDesktopWidget().screenGeometry();
	int hpos = ( int )( ( scrnSize.width() - 700 ) / 2 );
	int vpos = ( int )( ( scrnSize.height() - 400 ) / 2 );
	move( hpos, vpos );

	setFixedSize( 700, 400 );
};

QStringList NBMimeTypeSelector::selectedItems() {

	return checked;
};

void NBMimeTypeSelector::setChecked( QString mimeName, bool chkd ) {

	int sep = mimeName.indexOf( "/" );
	QString cls = mimeName.left( sep );
	QString elm = mimeName.right( sep );

	QList<QTreeWidgetItem*> items = mimeTree->findItems( elm, Qt::MatchContains | Qt::MatchRecursive, 0 );
	Q_FOREACH( QTreeWidgetItem *itm, items ) {
		if ( not itm->parent() )
			continue;

		if ( itm->parent()->text( 0 ) == cls ) {
			itm->setCheckState( 0, chkd ? Qt::Checked : Qt::Unchecked );
			return;
		}
	}
};

bool NBMimeTypeSelector::isChecked( QString mimeName ) {

	int sep = mimeName.indexOf( "/" );
	QString cls = mimeName.left( sep );
	QString elm = mimeName.right( sep );

	QList<QTreeWidgetItem*> items = mimeTree->findItems( elm, Qt::MatchContains | Qt::MatchRecursive, 0 );
	Q_FOREACH( QTreeWidgetItem *itm, items ) {
		if ( not itm->parent() )
			continue;

		if ( itm->parent()->text( 0 ) == cls )
			return ( itm->checkState( 0 ) == Qt::Checked ? true : false );
	}

	return false;
};

void NBMimeTypeSelector::populateGUI() {

	mimeTree = new QTreeWidget( this );
	mimeTree->setHeaderHidden( true );
	mdict.clear();

	// Create our database
	Q_FOREACH( QMimeType mtype, mimeDb.allMimeTypes() ) {
		QStringList l = mtype.name().split( "/" );
		if ( not mdict.contains( l[ 0 ] ) )
			mdict[ l[ 0 ] ] = QStringList();

		mdict[ l[ 0 ] ] << l[ 1 ];
	}

	// Populate our tree
	Q_FOREACH( QString key, mdict.keys() ) {
		QTreeWidgetItem *twip = new QTreeWidgetItem( mimeTree, QStringList() << key );
		twip->setFlags( Qt::ItemIsEnabled | Qt::ItemIsUserCheckable );
		twip->setCheckState( 0, Qt::Unchecked );
		mimeTree->addTopLevelItem( twip );
		Q_FOREACH( QString elm, mdict[ key ] ) {
			QTreeWidgetItem *twi = new QTreeWidgetItem( twip, QStringList() << elm );
			twi->setFlags( Qt::ItemIsEnabled | Qt::ItemIsUserCheckable );
			twi->setCheckState( 0, Qt::Unchecked );
			twip->addChild( twi );
		}

		twip->sortChildren( 0, Qt::AscendingOrder );
	}

	mimeTree->expandAll();

	QVBoxLayout *lyt = new QVBoxLayout();
	lyt->addWidget( new QLabel( "Select the mimetypes" ) );
	lyt->addWidget( mimeTree );

	setLayout( lyt );

	connect( mimeTree, SIGNAL( itemChanged( QTreeWidgetItem*, int ) ), this, SLOT( changeSelection( QTreeWidgetItem*, int ) ) );
};

void NBMimeTypeSelector::changeSelection( QTreeWidgetItem *itm, int col ) {

	if ( not itm->parent() ) {
		for( int c = 0; c < itm->childCount(); c++ )
			itm->child( c )->setCheckState( col, itm->checkState( col ) );
	}

	else {
		QString txt = itm->parent()->text( col ) + "/" + itm->text( col );
		if ( itm->checkState( col ) == Qt::Unchecked )
			checked.removeAll( txt );

		else
			checked << txt;
	}
};

#ifdef STANDALONE
int main( int argc, char **argv ) {

	QApplication app( argc, argv );
	NBMimeTypeSelector Gui;
	Gui.show();

	app.exec();

	NBAppEngine *ae = NBAppEngine::instance();

	NBAppFile geany = ae->application( "okteta" );
	qWarning() << geany.value( NBAppFile::MimeTypes ).toStringList();

	return 0;
};
#endif
