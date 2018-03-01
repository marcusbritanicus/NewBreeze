/*
	*
	* AboutNB.cpp - AboutNB Class for NewBreeze
	*
*/

#include "AboutNB.hpp"

AboutNB::AboutNB( QWidget *parent ) : QDialog( parent ) {

	createGui();
	setWindowProperties();
};

void AboutNB::createGui() {

	QWidget *aboutnb = createAboutNB();
	QWidget *license = createLicense();
	QWidget *credits = createCredits();

	QTabWidget *tabs = new QTabWidget( this );
	tabs->setFocusPolicy( Qt::NoFocus );
	tabs->addTab( aboutnb, QIcon( ":/icons/info.png" ), "&About" );
	tabs->addTab( license, QIcon( ":/icons/license.png" ), "&License" );
	tabs->addTab( credits, QIcon( ":/icons/credits.png" ), "Cre&dits" );

	NBButton *okBtn = new NBButton( QIcon::fromTheme( "dialog-ok" ), "&Close", this );
	okBtn->setObjectName( "okBtn" );
	connect( okBtn, SIGNAL( clicked() ), this, SLOT( close() ) );

	QHBoxLayout *bLyt = new QHBoxLayout();
	bLyt->addStretch( 0 );
	bLyt->addWidget( okBtn );
	bLyt->addStretch( 0 );

	QLabel *iconLbl = new QLabel( this );
	iconLbl->setWordWrap( true );
	iconLbl->setPixmap( QIcon( ":/icons/newbreeze.png" ).pixmap( 48 ) );

	QLabel *textLbl = new QLabel( this );
	textLbl->setText( QString(
			"<H2>NewBreeze v3.0.0</H2>"
			"<H4>Fast and light-weight File Manager</H4>"
		)
	);

	QHBoxLayout *tLyt = new QHBoxLayout();
	tLyt->addStretch( 10 );
	tLyt->addWidget( iconLbl );
	tLyt->addStretch( 1 );
	tLyt->addWidget( textLbl );
	tLyt->addStretch( 10 );

	QVBoxLayout *dlgLyt = new QVBoxLayout();
	dlgLyt->addLayout( tLyt );
	dlgLyt->addWidget( tabs );
	dlgLyt->addLayout( bLyt );

	setLayout( dlgLyt );
};

QWidget* AboutNB::createAboutNB() {

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
		"	<li>Full drag and drop support</li>"								\
		"	<li>Applications and Catalog View</li>"								\
		"   <li>Custom Folder Icons and Thumbnail Preview</li>"					\
		"   <li>Mime-Icon Support</li>"											\
		"   <li>BreadCrumbs Navigation</li>"									\
		"   <li>Custom Actions</li>"											\
		"   <li>Inline terminal</li>"											\
		"</ul>"																	\
	);

	QLabel *bodyLbl = new QLabel( this );
	bodyLbl->setWordWrap( true );
	bodyLbl->setText( text );

	QVBoxLayout *baseLyt = new QVBoxLayout();
	baseLyt->addWidget( bodyLbl, 0, Qt::AlignCenter );

	QWidget *base = new QWidget( this );
	base->setLayout( baseLyt );

	return base;
};

QWidget* AboutNB::createLicense() {

	QString license = QString(
		"<center><h3>NewBreeze v3.0.0</h3>"
		"<h4>Copyright &copy; Marcus Britanicus</h4></center>"

		"<p>This program is free software: you can redistribute it and/or modify<br>"
		"it under the terms of the GNU General Public License as published by<br>"
		"the Free Software Foundation, either version 3 of the License, or<br>"
		"(at your option) any later version.</p>"

		"<p>This program is distributed in the hope that it will be useful,<br>"
		"but WITHOUT ANY WARRANTY; without even the implied warranty of<br>"
		"MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the<br>"
		"GNU General Public License for more details.</p>"

		"<p>Press Ctrl+Shift+L while running NewBreeze to see the full license.<br>"
		"Also the LICENSE file is available with the source for your convenience.<br>"
		"Alternatively, you may visit http://www.gnu.org/licenses/gpl.html</p>"
	);

	QLabel *bodyLbl = new QLabel( this );
	bodyLbl->setWordWrap( true );
	bodyLbl->setText( license );

	QVBoxLayout *baseLyt = new QVBoxLayout();
	baseLyt->addWidget( bodyLbl, 0, Qt::AlignCenter );

	QWidget *base = new QWidget( this );
	base->setLayout( baseLyt );

	return base;
};

QWidget* AboutNB::createCredits() {

	QTextBrowser *browser = new QTextBrowser( this );
	browser->setOpenLinks( true );

	/* Load GPL3 Text/Html */
	QFile licFile( ":/Data/Credits.html" );
	licFile.open( QFile::ReadOnly | QFile::Text );
	browser->setHtml( licFile.readAll() );
	licFile.close();

	QVBoxLayout *baseLyt = new QVBoxLayout();
	baseLyt->addWidget( browser );

	QWidget *base = new QWidget( this );
	base->setLayout( baseLyt );

	return base;
};

void AboutNB::setWindowProperties() {

	/* Dialogs without close button */
	setWindowFlags( Qt::CustomizeWindowHint | Qt::Dialog | Qt::WindowSystemMenuHint | Qt::WindowTitleHint );
	setWindowModality( Qt::ApplicationModal);

	setWindowTitle( "About NewBreeze v3.0.0" );
	setWindowIcon( QIcon( ":/icons/newbreeze.png" ) );

	setMinimumWidth( 576 );
};


License::License( QWidget *parent ) : QDialog( parent ) {

	QTextBrowser *browser = new QTextBrowser( this );
	browser->setOpenLinks( false );

	/* Load GPL3 Text/Html */
	QFile licFile( ":/Data/GPL3.html" );
	licFile.open( QFile::ReadOnly | QFile::Text );
	browser->setHtml( licFile.readAll() );
	licFile.close();

	NBButton *okBtn = new NBButton( QIcon::fromTheme( "dialog-ok" ), "&Close", this );
	okBtn->setObjectName( "okBtn" );
	connect( okBtn, SIGNAL( clicked() ), this, SLOT( close() ) );

	QHBoxLayout *bLyt = new QHBoxLayout();
	bLyt->addStretch( 0 );
	bLyt->addWidget( okBtn );
	bLyt->addStretch( 0 );

	QVBoxLayout *dlgLyt = new QVBoxLayout();
	dlgLyt->addWidget( browser );
	dlgLyt->addLayout( bLyt );

	setLayout( dlgLyt );

	/* Dialogs without close button */
	setWindowFlags( Qt::CustomizeWindowHint | Qt::Dialog | Qt::WindowSystemMenuHint | Qt::WindowTitleHint );
	setWindowModality( Qt::ApplicationModal);

	setWindowTitle( "NewBreeze v3.0.0 License" );
	setWindowIcon( QIcon( ":/icons/newbreeze.png" ) );

	setMinimumSize( QSize( 630, 630 ) );
};
