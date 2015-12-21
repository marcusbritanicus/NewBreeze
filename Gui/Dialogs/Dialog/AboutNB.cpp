/*
	*
	* NBDialog.cpp - NewBreeze Dialog Class
	*
*/

#include <AboutNB.hpp>

AboutNB::AboutNB( QWidget *parent ) : NBDialog( parent ) {

	setDialogTitle( "About NewBreeze v3.0.0" );
	setDialogIcon( QIcon( ":/icons/newbreeze2.png" ) );

	setFixedWidth( 540 );

	QString title = QString( "About NewBreeze v3.0.0" );
	QString text = QString(
		"<p>Built on top of the Powerful Qt4 GUI Toolkit, this file manager "	\
		"is sleek, stylish and fast. Some of its features are</p>"				\
		"<ul>"																	\
		"	<li>Fast startup and browsing</li>"									\
		"	<li>Folder, text, html, image and odt quick preview</li>"			\
		"	<li>File Association support</li>"									\
		"	<li>Multiple Views - Tiles, Icons and Details</li>"					\
		"	<li>Sleek side panel showing drives and boomarks</li>"				\
		"	<li>Full drag and drop support</li>"								\
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
	iconLbl->setPixmap( QIcon( ":/icons/newbreeze2.png" ).pixmap( 48 ) );

	QLabel *textLbl = new QLabel();
	textLbl->setText( QString(
			"<H2>NewBreeze v3.0.0</H2>"
			"<H4>Fast and light-weight File Manager</H4>"
		)
	);

	QLabel *bodyLbl = new QLabel();
	bodyLbl->setWordWrap( true );
	bodyLbl->setText( text );

	NBButton *okBtn = new NBButton( QIcon::fromTheme( "dialog-ok" ), "&Close" );
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
