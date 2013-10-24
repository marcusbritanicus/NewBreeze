/*
	*
	* NBCustomActions.cpp - CustomActions class for NewBreeze
	*
*/

#include <NBCustomActions.hpp>

NBCustomActions::NBCustomActions() : NBDialog() {

	createGUI();
	setWidgetProperties();
};

void NBCustomActions::createGUI() {

	QVBoxLayout *layout = new QVBoxLayout();
	QHBoxLayout *btnsLyt = new QHBoxLayout();

	actionsTable = new QTableWidget();
	setupTable();

	addBtn = new NBToolButton( ":/icons/list-add.png" );
	addBtn->setShortcut( Settings->Shortcuts.AddCustomAction.at( 0 ) );
	connect( addBtn, SIGNAL( clicked() ), this, SLOT( addCustomAction() ) );

	delBtn = new NBToolButton( ":/icons/list-remove.png" );
	connect( delBtn, SIGNAL( clicked() ), this, SLOT( removeCustomActions() ) );

	quitBtn = new QPushButton( QIcon::fromTheme( "dialog-close" ), "&Close" );
	quitBtn->setObjectName( "cancelBtn" );
	connect( quitBtn, SIGNAL( clicked() ), this, SLOT( reject() ) );

	btnsLyt->addWidget( addBtn );
	btnsLyt->addWidget( delBtn );
	btnsLyt->addStretch( 0 );
	btnsLyt->addWidget( quitBtn );

	layout->addWidget( Separator::horizontal() );
	layout->addWidget( actionsTable );
	layout->addWidget( Separator::horizontal() );
	layout->addLayout( btnsLyt );

	// Final Layout
	setLayout( layout );
};

void NBCustomActions::setupTable() {

	actionsTable->horizontalHeader()->setResizeMode( QHeaderView::Fixed );
	actionsTable->horizontalHeader()->setStretchLastSection( true );
	actionsTable->verticalHeader()->setResizeMode( QHeaderView::Fixed );
	actionsTable->verticalHeader()->hide();

	actionsTable->setShowGrid( false );
	actionsTable->setSelectionBehavior( QAbstractItemView::SelectRows );

	actionsTable->setColumnCount( 4 );
	actionsTable->setRowCount( 0 );
	actionsTable->hideColumn( 3 );

	actionsTable->setColumnWidth( 0, 150 );
	actionsTable->setColumnWidth( 1, 150 );
	actionsTable->setHorizontalHeaderLabels( QStringList() << "Action Name" << "Filters" << "Action Command" );

	foreach( QString group, actionSettings.childGroups() ) {
		actionSettings.beginGroup( group );

		QString name = actionSettings.value( "Name" ).toString();
		QStringList glob = actionSettings.value( "Glob" ).toStringList();
		QString exec = actionSettings.value( "Exec" ).toString();

		QTableWidgetItem *item1 = new QTableWidgetItem( name );
		item1->setFlags( item1->flags() ^ Qt::ItemIsEditable );

		QTableWidgetItem *item2 = new QTableWidgetItem( glob.join( ", " ) );
		item2->setFlags( item2->flags() ^ Qt::ItemIsEditable );

		QTableWidgetItem *item3 = new QTableWidgetItem( exec );
		item3->setFlags( item3->flags() ^ Qt::ItemIsEditable );

		QTableWidgetItem *item4 = new QTableWidgetItem( group );
		item4->setFlags( item4->flags() ^ Qt::ItemIsEditable );

		actionsTable->insertRow( actionsTable->rowCount() );
		actionsTable->setItem( actionsTable->rowCount() - 1, 0, item1 );
		actionsTable->setItem( actionsTable->rowCount() - 1, 1, item2 );
		actionsTable->setItem( actionsTable->rowCount() - 1, 2, item3 );
		actionsTable->setItem( actionsTable->rowCount() - 1, 3, item4 );

		actionSettings.endGroup();
	}

	connect( actionsTable, SIGNAL( itemSelectionChanged() ), this, SLOT( updateButtons() ) );
	connect( actionsTable, SIGNAL( cellDoubleClicked( int, int ) ), this, SLOT( editCustomAction( int, int ) ) );
};

void NBCustomActions::setWidgetProperties() {

	setDialogTitle( "Custom Actions" );

	setFixedSize( 720, 540 );
	setWindowFlags( Qt::Dialog | Qt::FramelessWindowHint );
	setWindowModality( Qt::ApplicationModal );

	if ( ( Settings->General.Style == QString( "TransDark" ) ) or ( Settings->General.Style == QString( "TransLight" ) ) )
		setAttribute( Qt::WA_TranslucentBackground );
};

void NBCustomActions::addCustomAction() {

	NBNewAction *newAction = new NBNewAction();
	newAction->exec();

	QString group = QString( newAction->actionName() ).replace( " ", "" );

	if ( actionSettings.childGroups().contains( group, Qt::CaseInsensitive ) ) {
		QRegExp rx( "([0-9]+)$" );
		if ( rx.indexIn( group ) ) {
			group.chop( rx.cap( 1 ).size() );
			group += QString::number( rx.cap( 1 ).toInt() + 1 );
		}

		else {
			group += '0';
		}
	}

	if ( newAction->result() ) {
		actionSettings.beginGroup( group );

		QString name = newAction->actionName();
		QString exec = newAction->actionCommand();
		QString glob = newAction->actionFilters();

		actionSettings.setValue( "Name", name );
		actionSettings.setValue( "Glob", glob );
		actionSettings.setValue( "Exec", exec );
		actionSettings.sync();

		QTableWidgetItem *item1 = new QTableWidgetItem( name );
		item1->setFlags( item1->flags() ^ Qt::ItemIsEditable );

		QTableWidgetItem *item2 = new QTableWidgetItem( glob );
		item2->setFlags( item2->flags() ^ Qt::ItemIsEditable );

		QTableWidgetItem *item3 = new QTableWidgetItem( exec );
		item3->setFlags( item3->flags() ^ Qt::ItemIsEditable );

		QTableWidgetItem *item4 = new QTableWidgetItem( group );
		item4->setFlags( item4->flags() ^ Qt::ItemIsEditable );

		actionsTable->insertRow( actionsTable->rowCount() );
		actionsTable->setItem( actionsTable->rowCount() - 1, 0, item1 );
		actionsTable->setItem( actionsTable->rowCount() - 1, 1, item2 );
		actionsTable->setItem( actionsTable->rowCount() - 1, 2, item3 );
		actionsTable->setItem( actionsTable->rowCount() - 1, 3, item4 );

		actionSettings.endGroup();
	}
};

void NBCustomActions::editCustomAction( int row, int col ) {

	Q_UNUSED( col );

	QString name = actionsTable->item( row, 0 )->text();
	QString glob = actionsTable->item( row, 1 )->text();
	QString exec = actionsTable->item( row, 2 )->text();
	QString group = actionsTable->item( row, 3 )->text();

	NBNewAction *newAction = new NBNewAction();
	newAction->setActionName( name );
	newAction->setActionFilters( glob );
	newAction->setActionCommand( exec );
	newAction->exec();

	if ( newAction->result() ) {
		actionSettings.beginGroup( group );

		QString name = newAction->actionName();
		QString exec = newAction->actionCommand();
		QString glob = newAction->actionFilters();

		actionSettings.setValue( "Name", name );
		actionSettings.setValue( "Glob", glob );
		actionSettings.setValue( "Exec", exec );
		actionSettings.sync();

		actionsTable->item( row, 0 )->setText( name );
		actionsTable->item( row, 1 )->setText( glob );
		actionsTable->item( row, 2 )->setText( exec );

		actionSettings.endGroup();
	}
};

void NBCustomActions::removeCustomActions() {

	foreach( QModelIndex idx, actionsTable->selectionModel()->selectedRows() ) {

		actionsTable->removeRow( idx.row() );
		actionSettings.remove( idx.data().toString() );
	}
};

void NBCustomActions::updateButtons() {

	if( actionsTable->selectionModel()->hasSelection() )
		delBtn->setEnabled( true );

	else
		delBtn->setDisabled( true );
};

QString NBCustomActions::currentAction() {

	return actionsTable->item( actionsTable->currentRow(), 1 )->text();
};

NBNewAction::NBNewAction() {

	QHBoxLayout *lblBtnLyt = new QHBoxLayout();
	QVBoxLayout *layout = new QVBoxLayout();
	QHBoxLayout *btnsLyt = new QHBoxLayout();
	QHBoxLayout *widgetLyt = new QHBoxLayout();

	QWidget *baseWidget = new QWidget();
	baseWidget->setObjectName( tr( "guiBase" ) );

	QLabel *lbl = new QLabel( tr( "Add Custom Action" ) );

	QToolButton *closeBtn = new QToolButton();
	closeBtn->setText( trUtf8( "\u2718" ) );
	closeBtn->setAutoRaise( true );
	closeBtn->setFocusPolicy( Qt::NoFocus );

	connect( closeBtn, SIGNAL( clicked() ), this, SLOT( close() ) );

	lblBtnLyt->addWidget( lbl );
	lblBtnLyt->addStretch( 0 );
	lblBtnLyt->addWidget( closeBtn );

	QLabel *actionNameLbl = new QLabel( "Action &Name" );
	QLabel *actionGlobLbl = new QLabel ( "Action &Filters" );
	QLabel *actionCmdLbl = new QLabel( "Action &Command" );

	actionNameLE = new QLineEdit();
	actionGlobLE = new QLineEdit();
	actionCmdLE = new QLineEdit();

	actionNameLbl->setBuddy( actionNameLE );
	actionGlobLbl->setBuddy( actionGlobLE );
	actionCmdLbl->setBuddy( actionCmdLE );

	addBtn = new NBToolButton( QString( ":/icons/list-add.png" ) );
	addBtn->setObjectName( "okBtn" );

	cancelBtn = new NBToolButton( QString( ":/icons/delete.png" ) );
	cancelBtn->setObjectName( "cancelBtn" );

	connect( addBtn, SIGNAL( clicked() ), this, SLOT( accept() ) );
	connect( cancelBtn, SIGNAL( clicked() ), this, SLOT( reject() ) );

	btnsLyt->addStretch( 0 );
	btnsLyt->addWidget( addBtn );
	btnsLyt->addWidget( cancelBtn );

	layout->addWidget( actionNameLbl );
	layout->addWidget( actionNameLE );
	layout->addWidget( actionGlobLbl );
	layout->addWidget( actionGlobLE );
	layout->addWidget( actionCmdLbl );
	layout->addWidget( actionCmdLE );
	layout->addLayout( btnsLyt );

	baseWidget->setLayout( layout );
	widgetLyt->addWidget( baseWidget );
	widgetLyt->setContentsMargins( QMargins() );
	setLayout( widgetLyt );

	if ( ( Settings->General.Style == QString( "TransDark" ) ) or ( Settings->General.Style == QString( "TransLight" ) ) )
		setAttribute( Qt::WA_TranslucentBackground );
	setWindowFlags( Qt::Dialog | Qt::FramelessWindowHint );

	setFixedWidth( 360 );
};

QString NBNewAction::actionName() {

	return actionNameLE->text();
};

QString NBNewAction::actionCommand() {

	return actionCmdLE->text();
};

QString NBNewAction::actionFilters() {

	return actionGlobLE->text();
};

void NBNewAction::setActionName( QString str ) {

	actionNameLE->setText( str );;
};

void NBNewAction::setActionCommand( QString str ) {

	actionCmdLE->setText( str );;
};

void NBNewAction::setActionFilters( QString str ) {

	actionGlobLE->setText( str );;
};
