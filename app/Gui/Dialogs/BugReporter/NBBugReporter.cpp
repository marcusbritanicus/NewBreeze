/*
	*
	* NBBugReporter.cpp - Report issues in Github using their REST api
	*
*/

#include "NBBugReporter.hpp"

NBBugReporter::NBBugReporter() : NBDialog() {

	createGUI();
	setWindowProperties();
};

void NBBugReporter::createGUI() {

	QLabel *titleLbl = new QLabel( "&Title:" );
	titleLE = new QLineEdit();
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
	lyt->addWidget( titleLbl, 0, 0 );
	lyt->addWidget( titleLE, 0, 1 );
	lyt->addWidget( issueLbl, 1, 0 );
	lyt->addWidget( issueTE, 2, 0, 1, 2 );
	lyt->addLayout( btnLyt, 3, 0, 1, 2, Qt::AlignCenter );

	lyt->addWidget( minTtlTxtLbl, 0, 1, Qt::AlignRight | Qt::AlignVCenter );
	lyt->addWidget( minIssTxtLbl, 2, 0, 1, 2, Qt::AlignRight | Qt::AlignBottom );

	setLayout( lyt );
};

void NBBugReporter::setWindowProperties() {

	setWindowTitle( "NewBreeze3 - Bug Reporter" );
	setWindowIcon( QIcon::fromTheme( "newbreeze" ) );

	QRect scrnSize = QDesktopWidget().screenGeometry();
	int hpos = ( int )( ( scrnSize.width() - 700 ) / 2 );
	int vpos = ( int )( ( scrnSize.height() - 400 ) / 2 );
	move( hpos, vpos );

	setFixedSize( 700, 400 );
};

void NBBugReporter::sendReport() {

	QStringList data;
	data << titleLE->text();
	data << issueTE->toPlainText();

	NBGitHubAuthDialog *authDlg = new NBGitHubAuthDialog( data, this );
	if( authDlg->exec() )
		accept();

	else
		reject();
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

NBGitHubAuthDialog::NBGitHubAuthDialog( QStringList data, QWidget *parent ) : NBDialog( parent ) {

	mData << data;

	setWindowFlags( Qt::Dialog | Qt::FramelessWindowHint );
	setFixedWidth( 270 );

	QLabel *title = new QLabel( "<h3>GitHub Authentication</h3>" );

	QLabel *nameLbl = new QLabel( "User&name:" );
	nameLE = new QLineEdit();
	nameLbl->setBuddy( nameLE );

	QLabel *passLbl = new QLabel( "User&pass:" );
	passLE = new QLineEdit();
	passLE->setEchoMode( QLineEdit::Password );
	passLbl->setBuddy( passLE );

	QPushButton *cancelBtn = new QPushButton( QIcon::fromTheme( "dialog-cancel" ), "&Cancel" );
	connect( cancelBtn, SIGNAL( clicked() ), this, SLOT( reject() ) );

	QPushButton *authBtn = new QPushButton( "&Authenticate" );
	connect( authBtn, SIGNAL( clicked() ), this, SLOT( authenticate() ) );

	QHBoxLayout *btnLyt = new QHBoxLayout();
	btnLyt->addWidget( cancelBtn );
	btnLyt->addStretch();
	btnLyt->addWidget( authBtn );

	QGridLayout *lyt = new QGridLayout();
	lyt->addWidget( title, 0, 0, 1, 2, Qt::AlignCenter );
	lyt->addWidget( nameLbl, 1, 0 );
	lyt->addWidget( nameLE, 1, 1 );
	lyt->addWidget( passLbl, 2, 0 );
	lyt->addWidget( passLE, 2, 1 );
	lyt->addLayout( btnLyt, 3, 0, 1, 2, Qt::AlignCenter );

	setLayout( lyt );
};

void NBGitHubAuthDialog::authenticate() {

	QProcess proc;
	proc.setProcessChannelMode( QProcess::MergedChannels );

	QStringList args;
	args << "-u";
	args << nameLE->text() + ":" + passLE->text();
	args << "https://api.github.com/repos/marcusbritanicus/NewBreeze/issues";
	args << "-XPOST";
	args << "-H";
	args << "Content-Type: application/json";
	args << QString( "-d{\"title\": \"%1\", \"body\": \"%2\", \"assignees\": [ \"marcusbritanicus\" ]}" ).arg( mData.at( 0 ) ).arg( mData.at( 1 ) );

	proc.start( "curl", args );
	proc.waitForFinished();
	QString output = QString::fromUtf8( proc.readAll() );

	if ( output.contains( "https://api.github.com/repos/marcusbritanicus/NewBreeze/issues/" ) ) {
		NBMessageDialog::information(
			this,
			"NewBreeze3 - BugReporter",
			"<p>You have successfully submitted a bug report. You may check the status of your "
			"bug report at https://github.com/marcusbritanicus/NewBreeze/issues/</p>"
			"<p>Thank you for submitting the bug report...! You just helped maked NewBreeze3 better.</p>"
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
