/*
	*
	* NBSTerminalWidget.cpp - General Settings Widget for NewBreeze
	*
*/

#include "NBSTerminalWidget.hpp"
#include "NBMessageDialog.hpp"
#include "NBFunctions.hpp"

static QSettings sett( "NewBreeze", "NewBreeze" );

NBSTerminalWidget::NBSTerminalWidget( QWidget *parent ) : QWidget( parent ) {

	/* Terminal */
	termGB = new QGroupBox( "Terminals", this );

	termsLW = new QListWidget( termGB );
	termsLW->setMinimumHeight( 250 );
	QStringList terms = QStringList() << "Inbuilt";
	Q_FOREACH( QString key, sett.allKeys() ) {
		if ( key.startsWith( "Terminals/" ) and ( key != "Terminals/Default" ) ) {
			terms << key.replace( "Terminals/", "" );
		}
	}
	termsLW->addItems( terms );
	termsLW->setCurrentRow( terms.indexOf( sett.value( "Terminals/Default" ).toString() ) );

	defaultBtn = new QPushButton( "Defau&lt", termGB );
	connect( defaultBtn, SIGNAL( clicked() ), this, SLOT( handleDefaultClicked() ) );

	addBtn = new QPushButton( "Add &Terminal", termGB );
	connect( addBtn, SIGNAL( clicked() ), this, SLOT( handleAddTermClicked() ) );

	QVBoxLayout *termBtnLyt = new QVBoxLayout();
	termBtnLyt->addWidget( defaultBtn );
	termBtnLyt->addWidget( addBtn );
	termBtnLyt->addStretch();

	QHBoxLayout *termLyt = new QHBoxLayout();
	termLyt->addWidget( termsLW );
	termLyt->addLayout( termBtnLyt );

	termGB->setLayout( termLyt );

	QVBoxLayout *lyt = new QVBoxLayout();
	lyt->addWidget( termGB );
	lyt->addStretch();

	setLayout( lyt );
};

void NBSTerminalWidget::handleDefaultClicked() {

	QString defTerm = termsLW->currentItem()->text();

	sett.setValue( "Terminals/Default", defTerm );
	sett.sync();
};

void NBSTerminalWidget::handleAddTermClicked() {

	NBAddVTEDialog *vteDlg = new NBAddVTEDialog( this );
	vteDlg->exec();

	/* Reload the detected terminals list */
	termsLW->clear();
	QStringList terms = QStringList() << "Inbuilt";
	Q_FOREACH( QString key, sett.allKeys() ) {
		if ( key.startsWith( "Terminals/" ) and ( key != "Terminals/Default" ) ) {
			terms << key.replace( "Terminals/", "" );
		}
	}
	termsLW->addItems( terms );
	termsLW->setCurrentRow( terms.indexOf( sett.value( "Terminals/Default" ).toString() ) );
};

NBAddVTEDialog::NBAddVTEDialog( QWidget *parent ) : NBDialog( parent ) {

	setFixedWidth( 300 );

	QLabel *execLbl = new QLabel( "&Executable:" );
	QLabel *cmdLbl = new QLabel( "Co&mmand Switch:" );

	execLE = new QLineEdit( this );
	execLbl->setBuddy( execLE );

	wDirCB = new QCheckBox( "&Work Dir Switch:" );
	connect( wDirCB, SIGNAL( clicked() ), this, SLOT( handleCBPressed() ) );

	workLE = new QLineEdit( this );
	workLE->setDisabled( true );

	cmdLE = new QLineEdit( this );
	cmdLbl->setBuddy( cmdLE );

	btns = new NBSegmentButton( this );
	btns->setCount( 2 );

	btns->setSegmentIcon( 0, QIcon( ":/icons/list-add.png" ) );
	btns->setSegmentText( 0, "&Add" );
	btns->segment( 0 )->setObjectName( "okBtn" );
	connect( btns->segment( 0 ), SIGNAL( clicked() ), this, SLOT( addTerminal() ) );

	btns->setSegmentIcon( 1, QIcon( ":/icons/delete.png" ) );
	btns->setSegmentText( 1, "&Cancel" );
	btns->segment( 0 )->setObjectName( "cancelBtn" );
	connect( btns->segment( 1 ), SIGNAL( clicked() ), this, SLOT( reject() ) );

	QHBoxLayout *execLyt = new QHBoxLayout();
	execLyt->addWidget( execLbl );
	execLyt->addWidget( execLE );

	QHBoxLayout *workLyt = new QHBoxLayout();
	workLyt->addWidget( wDirCB );
	workLyt->addWidget( workLE );

	QHBoxLayout *cmdLyt = new QHBoxLayout();
	cmdLyt->addWidget( cmdLbl );
	cmdLyt->addWidget( cmdLE );

	QHBoxLayout *btnLyt = new QHBoxLayout();
	btnLyt->addStretch();
	btnLyt->addWidget( btns );

	QVBoxLayout *dlgLyt = new QVBoxLayout();
	dlgLyt->addLayout( execLyt );
	dlgLyt->addLayout( workLyt );
	dlgLyt->addLayout( cmdLyt );
	dlgLyt->addLayout( btnLyt );

	setLayout( dlgLyt );
};

void NBAddVTEDialog::addTerminal() {

	QString exec = execLE->text().simplified();
	QString wrkSwitch = workLE->text().simplified();
	QString cmdSwitch = cmdLE->text().simplified();

	if ( wDirCB->isChecked() )
		sett.setValue( "Terminals/" + execLE->text(), QStringList() << exec << wrkSwitch << "%1" << cmdSwitch << "%2" );

	else
		sett.setValue( "Terminals/" + execLE->text(), QStringList() << exec << cmdSwitch << "cd %1 && %2" );

	sett.sync();

	accept();
};

void NBAddVTEDialog::handleCBPressed() {

	if ( wDirCB->isChecked() ) {
		workLE->setEnabled( true );
		workLE->setFocus();
	}

	else {
		workLE->setDisabled( true );
		wDirCB->setFocus();
	}
};
