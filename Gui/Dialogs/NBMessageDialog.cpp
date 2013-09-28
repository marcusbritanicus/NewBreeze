/*
	*
	* NBMessageDialog.cpp - NBMessageDialog class
	*
*/

#include <NBMessageDialog.hpp>
#include <NBTools.hpp>
#include <NBGuiWidgets.hpp>

NBMessageDialog::NBMessageDialog( QString icon, QString title, QString text, QList<StandardButton> btns, QWidget *info ) : NBDialog() {

	QVBoxLayout *dlgLyt = new QVBoxLayout();
	QHBoxLayout *msgLyt = new QHBoxLayout();
	QHBoxLayout *btnLyt = new QHBoxLayout();

	iconLbl = new QLabel();
	iconLbl->setMaximumSize( QSize( 64, 64 ) );
	iconLbl->setPixmap( QPixmap( icon ).scaled( 64, 64, Qt::KeepAspectRatio, Qt::SmoothTransformation ) );

	textLbl = new QLabel( text );
	textLbl->setWordWrap( true );

	setTitle( title );

	msgLyt->addWidget( iconLbl );
	msgLyt->addWidget( textLbl );

	if ( btns.count() == 0 )
		btns << NBMessageDialog::Ok;

	if ( info ) {
		infoWidget = qobject_cast<QWidget *>( info );
		infoWidget->hide();

		detailsBtn = new QPushButton( QIcon( ":/icons/info.png" ), "&More" );
		connect( detailsBtn, SIGNAL( clicked() ), this, SLOT( toggleInfoWidget() ) );
		btnLyt->addWidget( detailsBtn );
	}

	btnLyt->addStretch( 0 );

	foreach( StandardButton btn, btns ) {
		QPushButton *button;
		switch( btn ) {
			case NBMessageDialog::Ok :
				button = new QPushButton( QIcon( ":/icons/ok.png" ), "&Ok" );
				button->setObjectName( tr( "okBtn" ) );
				connect( button, SIGNAL( clicked() ), this, SLOT( buttonClickHandler() ) );
				btnLyt->addWidget( button );
				break;

			case NBMessageDialog::Yes :
				button = new QPushButton( QIcon( ":/icons/ok.png" ), "&Yes" );
				button->setObjectName( tr( "okBtn" ) );
				connect( button, SIGNAL( clicked() ), this, SLOT( buttonClickHandler() ) );
				btnLyt->addWidget( button );
				break;

			case NBMessageDialog::YesToAll :
				button = new QPushButton( QIcon( ":/icons/ok.png" ), "Yes to &All" );
				button->setObjectName( tr( "okBtn" ) );
				connect( button, SIGNAL( clicked() ), this, SLOT( buttonClickHandler() ) );
				btnLyt->addWidget( button );
				break;

			case NBMessageDialog::No :
				button = new QPushButton( QIcon( ":/icons/delete.png" ), "&No" );
				button->setObjectName( tr( "cancelBtn" ) );
				connect( button, SIGNAL( clicked() ), this, SLOT( buttonClickHandler() ) );
				btnLyt->addWidget( button );
				break;

			case NBMessageDialog::NoToAll :
				button = new QPushButton( QIcon( ":/icons/delete.png" ), "N&o to All" );
				button->setObjectName( tr( "cancelBtn" ) );
				connect( button, SIGNAL( clicked() ), this, SLOT( buttonClickHandler() ) );
				btnLyt->addWidget( button );
				break;

			case NBMessageDialog::Abort :
				button = new QPushButton( QIcon( ":/icons/abort.png" ), "A&bort" );
				button->setObjectName( tr( "abortBtn" ) );
				connect( button, SIGNAL( clicked() ), this, SLOT( buttonClickHandler() ) );
				btnLyt->addWidget( button );
				break;

			case NBMessageDialog::Retry :
				button = new QPushButton( QIcon( ":/icons/refresh.png" ), "&Retry" );
				connect( button, SIGNAL( clicked() ), this, SLOT( buttonClickHandler() ) );
				btnLyt->addWidget( button );
				break;

			case NBMessageDialog::Ignore :
				button = new QPushButton( QIcon( ":/icons/ok.png" ), "&Ignore" );
				connect( button, SIGNAL( clicked() ), this, SLOT( buttonClickHandler() ) );
				btnLyt->addWidget( button );
				break;

			case NBMessageDialog::Cancel :
				button = new QPushButton( QIcon::fromTheme( "dialog-cancel" ), "&Cancel" );
				button->setObjectName( tr( "cancelBtn" ) );
				connect( button, SIGNAL( clicked() ), this, SLOT( buttonClickHandler() ) );
				btnLyt->addWidget( button );
				break;

			default:
				qDebug() << "No Button added!!";
				break;
		}
	}

	dlgLyt->addLayout( msgLyt );
	if ( info ) {
		dlgLyt->addWidget( infoWidget );
	}
	dlgLyt->addWidget( Separator::horizontal() );
	dlgLyt->addLayout( btnLyt );

	NBDialog::setLayout( dlgLyt );
};

void NBMessageDialog::setIcon( QString icon ) {

	iconLbl->setPixmap( QPixmap( icon ).scaled( 64, 64, Qt::KeepAspectRatio, Qt::SmoothTransformation ) );
};

void NBMessageDialog::setTitle( QString title ) {

	NBDialog::setDialogTitle( title );
};

void NBMessageDialog::setText( QString text ) {

	textLbl->setText( text );
};

void NBMessageDialog::keyPressEvent( QKeyEvent *keyEvent ) {

	QWidget::keyPressEvent( keyEvent );
};

void NBMessageDialog::close() {

	NBDialog::close();
	emit closed();
};

void NBMessageDialog::show() {

	QWidget::show();
};

void NBMessageDialog::exec() {

	QEventLoop loop;
	connect( this, SIGNAL( closed() ), &loop, SLOT( quit() ) );

	QWidget::show();
	loop.exec();
};

void NBMessageDialog::toggleInfoWidget() {

	if ( infoWidget->isVisible() ) {
		detailsBtn->setText( "&More..." );
		infoWidget->hide();

		restoreGeometry( origGeometry );
	}

	else {
		origGeometry = saveGeometry();

		detailsBtn->setText( "&Less..." );
		infoWidget->show();

		adjustSize();
	}
};

void NBMessageDialog::buttonClickHandler() {

	QString text = qobject_cast<QPushButton*>( sender() )->text();

	if ( text == QString( "&Ok" ) ) {
		retValue = NBMessageDialog::Ok;
		close();
	}

	else if ( text == QString( "&Yes" ) ) {
		retValue = NBMessageDialog::Yes;
		close();
	}

	else if ( text == QString( "Yes to &All" ) ) {
		retValue = NBMessageDialog::YesToAll;
		close();
	}

	else if ( text == QString( "&No" ) ) {
		retValue = NBMessageDialog::No;
		close();
	}

	else if ( text == QString( "N&o to All" ) ) {
		retValue = NBMessageDialog::NoToAll;
		close();
	}

	else if ( text == QString( "A&bort" ) ) {
		retValue = NBMessageDialog::Abort;
		close();
	}

	else if ( text == QString( "&Retry" ) ) {
		retValue = NBMessageDialog::Retry;
		close();
	}

	else if ( text == QString( "&Ignore" ) ) {
		retValue = NBMessageDialog::Ignore;
		close();
	}

	else if ( text == QString( "&Cancel" ) ) {
		retValue = NBMessageDialog::Cancel;
		close();
	}
};

void NBMessageDialog::aboutNewBreeze() {

	QString title = QString( "About NewBreeze v2.0.0-preview" );
	QString text = QString(
		"<p><b><center>Fast and light-weight File Manager</center></b></p>"		\
		"<p>Built on top of the Powerful Qt4 GUI Toolkit, this file manager "	\
		"is sleek, stylish and fast. Some of its features are</p>"				\
		"<ul>"																	\
		"	<li>Fast startup and browsing</li>"									\
		"	<li>Folder, text, html and image quick preview</li>"				\
		"	<li>File Association support</li>"									\
		"	<li>Multiple Views</li>"											\
		"	<li>Lists the dives in the sidebar</li>"							\
		"	<li>Full drag and drop support</li>"								\
		"	<li>Bookmarks</li>"													\
		"   <li>Custom Folder Icons and Thumbnail Preview</li>"					\
		"   <li>Mime-Icon Support</li>"											\
		"   <li>BreadCrumbs Navigation</li>"									\
		"   <li>Custom Actions</li>"											\
		"   <li>Inline terminal</li>"											\
		"   <li>Set Custom Folder Icons</li>"									\
		"</ul>"																	\
		"<p><small>The credit for the inline terminal goes to "					\
		"Christian Surlykke, the author of QTermWidget. "						\
		"I have used the QTermWidget to dispaly the "							\
		"inline terminal.</small></p>"											\
	);
	QList<StandardButton> btns = QList<StandardButton>() << NBMessageDialog::Ok;

	NBMessageDialog *dlg = new NBMessageDialog( ":/icons/newbreeze.png", title, text, btns );
	dlg->setFixedSize( 540, 390 );
	dlg->exec();
};

void NBMessageDialog::aboutQt() {

	QString title = QObject::tr(
		"About Qt %1"
	).arg( QLatin1String( QT_VERSION_STR ) );

	QString text  = QObject::tr(
		"<p>Qt is a C++ toolkit for cross-platform application development.</p>"

		"<p>Qt provides single-source portability across MS&nbsp;Windows, Mac&nbsp;OS&nbsp;X, Linux, and all major commercial Unix"
		"variants. Qt is also available for embedded devices as Qt for Embedded Linux and Qt for Windows CE.</p>"

		"<p>Qt is available under three different licensing options designed to accommodate the needs of our various users.</p>"

		"<p>Qt licensed under our commercial license agreement is appropriate for development of proprietary/commercial software where you do "
		"not want to share any source code with third parties or otherwise cannot comply with the terms of the GNU LGPL version 2.1 or GNU "
		"GPL version 3.0.</p>"

		"<p>Qt licensed under the GNU LGPL version 2.1 is appropriate for the development of Qt applications (proprietary or open source) "
		"provided you can comply with the terms and conditions of the GNU LGPL version 2.1.</p>"

		"<p>Qt licensed under the GNU General Public License version 3.0 is appropriate for the development of Qt applications where you wish "
		"to use such applications in combination with software subject to the terms of the GNU GPL version 3.0 or where you are otherwise "
		"willing to comply with the terms of the GNU GPL version 3.0.</p>"

		"<p>Please see <a href='http://qt.digia.com/product/licensing'>qt.digia.com/product/licensing</a> for an overview of Qt licensing.</p>"

		"<p>Copyright (C) 2012 Digia Plc and/or its subsidiary(-ies).</p>"

		"<p>Qt is a Digia product. See <a href='http://qt.digia.com/'>qt.digia.com</a> for more information.</p>"
	);

	QList<StandardButton> btns = QList<StandardButton>() << NBMessageDialog::Ok;

	NBMessageDialog *dlg = new NBMessageDialog( ":/icons/QtLogo.png", title, text, btns );
	dlg->setMinimumSize( 630, 450 );
	dlg->exec();
};

int NBMessageDialog::information( QString title, QString text, QList<StandardButton> btns, QWidget *infoWidget ) {

	NBMessageDialog *dlg = new NBMessageDialog( ":/icons/info.png", title, text, btns, infoWidget );
	dlg->exec();

	return dlg->retValue;
}

int NBMessageDialog::question( QString title, QString text, QList<StandardButton> btns, QWidget *infoWidget ) {

	NBMessageDialog *dlg = new NBMessageDialog( ":/icons/question.png", title, text, btns, infoWidget );
	dlg->exec();

	return dlg->retValue;
}

int NBMessageDialog::warning( QString title, QString text, QList<StandardButton> btns, QWidget *infoWidget ) {

	NBMessageDialog *dlg = new NBMessageDialog( ":/icons/warning.png", title, text, btns, infoWidget );
	dlg->exec();

	return dlg->retValue;
}

int NBMessageDialog::error( QString title, QString text, QList<StandardButton> btns, QWidget *infoWidget ) {

	NBMessageDialog *dlg = new NBMessageDialog( ":/icons/error.png", title, text, btns, infoWidget );
	dlg->exec();

	return dlg->retValue;
}

int NBMessageDialog::critical( QString title, QString text, QList<StandardButton> btns, QWidget *infoWidget ) {

	NBMessageDialog *dlg = new NBMessageDialog( ":/icons/stop.png", title, text, btns, infoWidget );
	dlg->exec();

	return dlg->retValue;
}
