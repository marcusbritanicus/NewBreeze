/*
	*
	* AboutNB.cpp - AboutNB Class for NewBreeze
	*
*/

#include <AboutNB.hpp>

AboutNB::AboutNB( QWidget *parent ) : QDialog( parent ) {

	createGui();
	setWindowProperties();
};

void AboutNB::createGui() {
	QString title = QString( "About NewBreeze v3.0.0" );
	QString text = QString(
		"<p>Built on top of the Powerful Qt GUI Toolkit, this file manager "	\
		"is sleek, stylish and fast. Some of its features are</p>"				\
		"<ul>"																	\
		"	<li>Complete Qt4 and Qt5 Support</li>"								\
		"	<li>Fast startup and browsing</li>"									\
		"	<li>Folder, text, html, image and odt quick preview</li>"			\
		"	<li>Plugin support to extend quick preview interface</li>"			\
		"	<li>File Association support</li>"									\
		"	<li>Multiple Views - Tiles, Icons and Details</li>"					\
		"	<li>Sleek side panel showing drives and boomarks</li>"				\
		"	<li><s>Full drag and drop support</s></li>"				\
		"	<li>Applications and Catalog View</li>"								\
		"   <li>Custom Folder Icons and Thumbnail Preview</li>"					\
		"   <li>Mime-Icon Support</li>"											\
		"   <li>BreadCrumbs Navigation</li>"									\
		"   <li>Custom Actions</li>"											\
		"   <li>Inline terminal</li>"											\
		"</ul>"																	\
		"<p><small>The credit for the inline terminal goes to "					\
		"Christian Surlykke, the author of QTermWidget. "						\
		"I have used the QTermWidget to dispaly the "							\
		"inline terminal.</small></p>"											\
	);

	QLabel *iconLbl = new QLabel();
	iconLbl->setWordWrap( true );
	iconLbl->setPixmap( QIcon( ":/icons/newbreeze.png" ).pixmap( 48 ) );

	QLabel *textLbl = new QLabel();
	textLbl->setText( QString(
			"<H2>NewBreeze v3.0.0</H2>"
			"<H4>Fast and light-weight File Manager</H4>"
		)
	);

	QLabel *bodyLbl = new QLabel();
	bodyLbl->setWordWrap( true );
	bodyLbl->setText( text );

	NBButton *okBtn = new NBButton( QIcon::fromTheme( "dialog-ok" ), "&Close", this );
	okBtn->setObjectName( "okBtn" );
	connect( okBtn, SIGNAL( clicked() ), this, SLOT( close() ) );

	QHBoxLayout *tLyt = new QHBoxLayout();
	tLyt->addStretch( 10 );
	tLyt->addWidget( iconLbl );
	tLyt->addStretch( 1 );
	tLyt->addWidget( textLbl );
	tLyt->addStretch( 10 );

	QHBoxLayout *bLyt = new QHBoxLayout();
	bLyt->addStretch( 0 );
	bLyt->addWidget( okBtn );
	bLyt->addStretch( 0 );

	QVBoxLayout *dlgLyt = new QVBoxLayout();
	dlgLyt->addLayout( tLyt );
	dlgLyt->addWidget( bodyLbl );
	dlgLyt->addWidget( Separator::horizontal( this ) );
	dlgLyt->addLayout( bLyt );

	setLayout( dlgLyt );
};

void AboutNB::setWindowProperties() {

	/* Dialogs without close button */
	setWindowFlags( Qt::CustomizeWindowHint | Qt::Dialog | Qt::WindowSystemMenuHint | Qt::WindowTitleHint );
	setWindowModality( Qt::ApplicationModal);

	setWindowTitle( "About NewBreeze v3.0.0" );
	setWindowIcon( QIcon( ":/icons/newbreeze.png" ) );

	setFixedWidth( 540 );
};
