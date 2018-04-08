/*
	*
	* NBCategoryMenu.cpp - ContextMenu class for NewBreeze
	*
*/

#include "NBCategoryMenu.hpp"
#include "NBPropertiesDialog.hpp"

NBCategoryMenu::NBCategoryMenu( QString catName, QPixmap icon, QString wNode, QModelIndexList nodeList, QWidget *parent ) : QMenu( parent ) {

	mCategory = catName;
	mWorkNode = wNode;
	sNodes << nodeList;

	QWidgetAction *title = new QWidgetAction( this );
	QWidget *actBase = new QWidget();

	QLabel *pix = new QLabel();
	pix->setFixedSize( 20, 20 );
	pix->setPixmap( icon.scaled( 16, 16, Qt::KeepAspectRatio, Qt::SmoothTransformation ) );
	pix->setAlignment( Qt::AlignCenter );

	QLabel *name = new QLabel( "<b>" + catName + "</b>" );
	name->setFixedHeight( 20 );
	name->setAlignment( Qt::AlignVCenter );

	QHBoxLayout *lyt = new QHBoxLayout();
	lyt->setContentsMargins( QMargins( 4, 4, 4, 0 ) );
	lyt->addWidget( pix );
	lyt->addWidget( name );

	actBase->setLayout( lyt );
	title->setDefaultWidget( actBase );
	addAction( title );

	addSeparator();

	populateMenu();
};

void NBCategoryMenu::populateMenu() {

	showAct = new QAction( "Visible", this );
	connect( showAct, SIGNAL( triggered() ), this, SLOT( showCategory() ) );
	addAction( showAct );

	foldAct = new QAction( "Folded", this );
	connect( foldAct, SIGNAL( triggered() ), this, SLOT( foldCategory() ) );
	addAction( foldAct );

	hideAct = new QAction( "Hidden", this );
	connect( hideAct, SIGNAL( triggered() ), this, SLOT( hideCategory() ) );
	addAction( hideAct );

	addSeparator();

	selAct = new QAction( QIcon( ":/icons/select1.png" ), "Select All", this );
	addAction( selAct );
	connect( selAct, SIGNAL( triggered() ), this, SLOT( manageSelection() ) );

	uselAct = new QAction( QIcon(), "Deselect All", this );
	addAction( uselAct );
	connect( uselAct, SIGNAL( triggered() ), this, SLOT( manageSelection() ) );

	addSeparator();

	propAct = new QAction( QIcon( ":/icons/props.png" ), "Properties", this );
	connect( propAct, SIGNAL( triggered() ), this, SLOT( showProperties() ) );
	addAction( propAct );
};

void NBCategoryMenu::showCategory() {

	emit showCategory( mCategory );
};

void NBCategoryMenu::foldCategory() {

	emit foldCategory( mCategory );
};

void NBCategoryMenu::hideCategory() {

	emit hideCategory( mCategory );
};

void NBCategoryMenu::manageSelection() {

	QAction *act = qobject_cast<QAction*>( sender() );
	if ( act->text().replace( "&", "" ).startsWith( "Select" ) )
		emit selection( mCategory, true );

	else
		emit selection( mCategory, false );
};

void NBCategoryMenu::showProperties() {

	QStringList paths;
	foreach( QModelIndex idx, sNodes ) {
		paths << mWorkNode + "/" + idx.data().toString();
		qDebug() << mWorkNode + idx.data().toString();
	}

	bool terminate = false;

	NBPropertiesDialog *propsDlg = new NBPropertiesDialog( paths, NBPropertiesDialog::Properties, &terminate, this );
	propsDlg->show();
};
