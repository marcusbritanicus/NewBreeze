/*
	*
	* NBPropertiesDialog.cpp - Properties of the selected object
	*
*/

#include "NBPropertiesDialog.hpp"

NBPropertiesDialog::NBPropertiesDialog( QStringList paths, PropertiesTab tab, QWidget *parent ) : NBDialog( parent ) {

	mTerminate = false;
	pathsList << paths;

	tabs = new NBPSideBar( this );

	propsW = new NBPropertiesWidget( paths, &mTerminate, this );
	permsW = new NBPermissionsWidget( paths, this );
	defaultsW = new NBDefaultsWidget( paths, this );
	previewW = new NBPreviewWidget( paths, this );
	detailsW = new NBDetailsWidget( paths, this );

	stack = new QStackedWidget( this );
	stack->addWidget( propsW );
	stack->addWidget( permsW );
	stack->addWidget( defaultsW );
	stack->addWidget( previewW );
	stack->addWidget( detailsW );

	connect( tabs, SIGNAL( clicked( int ) ), stack, SLOT( setCurrentIndex( int ) ) );

	QHBoxLayout *baseLyt = new QHBoxLayout();
	baseLyt->addWidget( tabs );
	baseLyt->addWidget( stack );

	setLayout( baseLyt );

	tabs->setChecked( tab );
	stack->setCurrentIndex( tab );

	setFixedSize( 480, 330 );
};

NBPropertiesDialog::~NBPropertiesDialog() {

	mTerminate = true;
}
