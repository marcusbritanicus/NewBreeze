/*
	*
	* NBSPluginsWidget.cpp - Plugins Manager for NewBreeze
	*
*/

#include "NBSPluginsWidget.hpp"

NBSPluginsWidget::NBSPluginsWidget( QWidget *parent ) : QWidget( parent ) {

	createGUI();
};

void NBSPluginsWidget::createGUI() {

	// ListWidget for Plugins
	pPluginsLW = new QListWidget( this );

	/* Load the available Plugins */
	NBPluginManager *pMgr = NBPluginManager::instance();
	QSettings plugSett( "NewBreeze", "Plugins" );
	Q_FOREACH( NBPluginInterface *plugin, pMgr->allPlugins() ) {
		QListWidgetItem *itm = new QListWidgetItem( QString( "%1" ).arg( plugin->name() ), pPluginsLW );

		if ( getuid() )
			itm->setFlags( Qt::ItemIsEnabled | Qt::ItemIsUserCheckable );

		else
			itm->setFlags( Qt::ItemIsUserCheckable );

		itm->setCheckState( plugSett.value( plugin->name() + "/Enabled", true ).toBool() ? Qt::Checked : Qt::Unchecked );
		pPluginsLW->addItem( itm );
	}

	connect( pPluginsLW, SIGNAL( itemChanged( QListWidgetItem* ) ), this, SLOT( savePluginEnabled( QListWidgetItem* ) ) );

	// ListWidget for Plugin Paths
	pPathsLW = new QListWidget( this );

	QStringList pluginPaths;
	if ( not plugSett.value( "PluginPaths" ).toStringList().count() )
		plugSett.setValue( "PluginPaths", QStringList() << DEFAULT_PLUGIN_PATH << NBXdg::home() + ".config/NewBreeze/plugins/" );
	pPathsLW->addItems( plugSett.value( "PluginPaths" ).toStringList() );

	pathsBtn = new NBSegmentButton( this );
	pathsBtn->setCount( 2 );

	pathsBtn->setSegmentIcon( 0, QIcon::fromTheme( "list-add" ) );
	pathsBtn->setSegmentText( 0, "&Add Path" );

	pathsBtn->setSegmentIcon( 1, QIcon::fromTheme( "list-remove" ) );
	pathsBtn->setSegmentText( 1, "&Remove Path" );

	connect( pathsBtn, SIGNAL( clicked( int ) ), this, SLOT( buttonClick( int ) ) );

	QHBoxLayout *btnLyt = new QHBoxLayout();
	btnLyt->addStretch();
	btnLyt->addWidget( pathsBtn );
	btnLyt->addStretch();

	QVBoxLayout *baseLyt = new QVBoxLayout();
	baseLyt->addWidget( new QLabel( "<p><large>Available plugins:</large></p>", this ) );
	baseLyt->addWidget( pPluginsLW );
	baseLyt->addWidget( new QLabel( "<p><large>Plugin search paths</large><br>NewBreeze will search the following paths for plugins:</p>", this ) );
	baseLyt->addWidget( pPathsLW );
	baseLyt->addLayout( btnLyt );

	setLayout( baseLyt );
};

void NBSPluginsWidget::buttonClick( int b ) {

	if ( b == 0 )
		addPath();

	else
		removePath();
};

void NBSPluginsWidget::addPath() {

	QString path = NBDirectoryDialog::getDirectoryName( this, "Plugin Search Path", "/" );

	if ( path.count() ) {
		pPathsLW->addItem( path );
		updateSettings();
	}
};

void NBSPluginsWidget::removePath() {

	int curRow = pPathsLW->currentRow();
	if ( pPathsLW->selectedItems().contains( pPathsLW->item( curRow ) ) ) {
		pPathsLW->takeItem( curRow );
		updateSettings();
	}
};

void NBSPluginsWidget::updateSettings() {

	QStringList paths;
	for( int i = 0; i < pPathsLW->count(); i++ )
		paths << pPathsLW->item( i )->text();

	QSettings nbsett( "NewBreeze", "Plugins" );

	QStringList pluginPaths;
	if ( not paths.count() )
		nbsett.setValue( "PluginPaths", QStringList() << DEFAULT_PLUGIN_PATH << NBXdg::home() + ".config/NewBreeze/plugins/" );

	pPathsLW->clear();
	pPathsLW->addItems( nbsett.value( "PluginPaths" ).toStringList() );
};

void NBSPluginsWidget::savePluginEnabled( QListWidgetItem *itm ) {

	QSettings nbsett( "NewBreeze", "Plugins" );
	nbsett.setValue( itm->text() + "/Enabled", ( itm->checkState() == Qt::Unchecked ? false : true ) );
	nbsett.sync();
};
