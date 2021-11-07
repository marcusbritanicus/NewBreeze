/*
	*
	* NBBugReporter.cpp - Report issues in GitLab using their REST api
	*
*/

#include "NBBugReporter.hpp"

NBBugReporter::NBBugReporter() : NBDialog() {

	createGUI();
	setWindowProperties();
};

void NBBugReporter::createGUI() {

	QLabel *tokenLbl = new QLabel( "&Authorization Token:" );
	tokenLE = new QLineEdit( "" );
	tokenLE->setFont( QFont( "DejaVu Sans Mono", 10 ) );
	tokenLE->setAlignment( Qt::AlignCenter );
	tokenLE->setInputMask( "NNNNNNNNNNNNNNNNNNNN" );
	tokenLbl->setBuddy( tokenLE );
	connect( tokenLE, SIGNAL( textChanged( QString ) ), this, SLOT( enableSend() ) );

	QLabel *titleLbl = new QLabel( "&Title:" );
	titleLE = new QLineEdit( "Testing the BugReporter" );
	titleLE->setFont( QFont( "DejaVu Sans Mono", 10 ) );
	titleLbl->setBuddy( titleLE );
	connect( titleLE, SIGNAL( textChanged( QString ) ), this, SLOT( enableSend() ) );

	QLabel *issueLbl = new QLabel( "&Issue:" );
	issueTE = new QPlainTextEdit();
	issueTE->setFont( QFont( "DejaVu Sans Mono", 10 ) );
	connect( issueTE, SIGNAL( textChanged() ), this, SLOT( enableSend() ) );

	issueLbl->setBuddy( issueTE );

	cancelBtn = new QPushButton( QIcon::fromTheme( "dialog-cancel" ), "&Cancel" );
	connect( cancelBtn, SIGNAL( clicked() ), this, SLOT( reject() ) );

	sendBtn = new QPushButton( QIcon::fromTheme( "mail-send" ), "&Send" );
	sendBtn->setDisabled( true );
	connect( sendBtn, SIGNAL( clicked() ), this, SLOT( sendReport() ) );

	QHBoxLayout *btnLyt = new QHBoxLayout();
	btnLyt->addWidget( cancelBtn );
	btnLyt->addStretch();
	btnLyt->addWidget( sendBtn );

	minTtlTxtLbl = new QLabel( "<font color='darkred'>Minimum 20 chars remain    </font>" );
	minTtlTxtLbl->setStyleSheet( "background: transparent;" );
	minIssTxtLbl = new QLabel( "<font color='darkred'>Minimum 150 chars remain    </font>" );
	minIssTxtLbl->setStyleSheet( "background: transparent;" );

	QGridLayout *lyt = new QGridLayout();
	lyt->addWidget( tokenLbl, 0, 0 );
	lyt->addWidget( tokenLE, 0, 1 );
	lyt->addWidget( titleLbl, 1, 0 );
	lyt->addWidget( titleLE, 1, 1 );
	lyt->addWidget( issueLbl, 2, 0 );
	lyt->addWidget( issueTE, 3, 0, 1, 2 );
	lyt->addLayout( btnLyt, 4, 0, 1, 2, Qt::AlignCenter );

	lyt->addWidget( minTtlTxtLbl, 1, 1, Qt::AlignRight | Qt::AlignVCenter );
	lyt->addWidget( minIssTxtLbl, 3, 0, 1, 2, Qt::AlignRight | Qt::AlignBottom );

	setLayout( lyt );
};

void NBBugReporter::setWindowProperties() {

	setWindowTitle( "NewBreeze3 - Bug Reporter" );
	setWindowIcon( QIcon::fromTheme( "newbreeze" ) );

	QRect scrnSize = qApp->primaryScreen()->geometry();
	int hpos = ( int )( ( scrnSize.width() - 700 ) / 2 );
	int vpos = ( int )( ( scrnSize.height() - 400 ) / 2 );
	move( hpos, vpos );

	setFixedSize( 700, 400 );
};

void NBBugReporter::sendReport() {

	QString token = tokenLE->text();
	QString title = QString::fromLocal8Bit( QUrl::toPercentEncoding( titleLE->text() ) );
	QString issue = QString::fromLocal8Bit( QUrl::toPercentEncoding( issueTE->toPlainText() ) );

	QString url( "https://gitlab.com/api/v4/projects/6894571/issues?title=%2&description=%3&assignee_ids=2417559&private_token=%1" );
	QStringList args = QStringList() << "--request" << "POST" << url.arg( token ).arg( title ).arg( issue );

	QProcess proc;
	proc.start( "curl", args );
	proc.waitForFinished( -1 );
	QString output = QString::fromUtf8( proc.readAll() );

	QRegExp rx( "https://gitlab.com/marcusbritanicus/NewBreeze/issues/[0-9]+" );
	rx.indexIn( output );

	if ( output.contains( "https://gitlab.com/marcusbritanicus/NewBreeze/issues/" ) ) {
		NBMessageDialog::information(
			this,
			"NewBreeze3 - BugReporter",
			QString( "<p>You have successfully submitted a bug report. You may check the status of your "
			"bug report at %1.</p><p>Thank you for submitting the bug report...! "
			"You just helped make NewBreeze3 better.</p>" ).arg( rx.cap( 0 ) )
		);
		accept();
	}

	else {
		NBMessageDialog::information(
			this,
			"NewBreeze3 - BugReporter",
			"<p>I am very sorry, but the bug report was not submitted properly. Please raise the issue at "
			"https://github.com/marcusbritanicus/NewBreeze/issues/new</p>"
		);
		reject();
	}
};

void NBBugReporter::enableSend() {

	if ( titleLE->text().count() < 20 ) {
		minTtlTxtLbl->show();
		minTtlTxtLbl->setText( QString( "<font color='darkred'>Minimum %1 chars remain    </font>" ).arg( 20 - titleLE->text().count() ) );
	}

	else {
		minTtlTxtLbl->hide();
	}

	if ( issueTE->toPlainText().count() < 150 ) {
		minIssTxtLbl->show();
		minIssTxtLbl->setText( QString( "<font color='darkred'>Minimum %1 chars remain    </font>" ).arg( 150 - issueTE->toPlainText().count() ) );
	}

	else {
		minIssTxtLbl->hide();
	}

	if ( ( titleLE->text().count() >= 15 ) and ( issueTE->toPlainText().count() >= 150 ) )
		sendBtn->setEnabled( true );

	else
		sendBtn->setEnabled( false );
};
