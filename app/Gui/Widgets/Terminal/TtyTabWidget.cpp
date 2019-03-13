/*
    *
    * * TtyTabWidget.cpp - TabWidget to hold Pty for NBTerminal
    *
*/

#include <TtyTabWidget.hpp>
#include <NBTermWidget.hpp>

TtyTabWidget::TtyTabWidget( QWidget *parent, bool mode ) : QTabWidget( parent ) {

	/* DropDownMode */
	ddMode = mode;

	setMovable( true );
	setTabsClosable( true );
	setDocumentMode( true );

	setFocusPolicy( Qt::NoFocus );

	connect( this, SIGNAL( tabCloseRequested( int ) ), this, SLOT( closeTab( int ) ) );

	setTabPosition( QTabWidget::North );
};

int TtyTabWidget::newTerminal() {

	NBTermWidget *widget = new NBTermWidget( this );
	widget->setFocusPolicy( Qt::StrongFocus );

	int idx = addTab( widget, QIcon::fromTheme( "utilities-terminal" ), "" );
	setTabText( idx, QString( "Terminal %1" ).arg( idx + 1 ) );
	connect( widget, SIGNAL( finished() ), this, SLOT( closeTab() ) );

	setCurrentIndex( idx );

	widget->setFocus();

	return idx;
};

int TtyTabWidget::newTerminal( QString wDir, QString cmd ) {

	NBTermWidget *widget = new NBTermWidget( wDir, cmd, this );
	widget->setFocusPolicy( Qt::StrongFocus );

	int idx = addTab( widget, QIcon::fromTheme( "utilities-terminal" ), "" );
	setTabText( idx, QString( "Terminal %1" ).arg( idx + 1 ) );

	connect( widget, SIGNAL( finished() ), this, SLOT( closeTab() ) );

	setCurrentIndex( idx );

	widget->setFocus();

	return idx;
};

int TtyTabWidget::newTerminalCWD() {

	NBTermWidget *curTerm = qobject_cast<NBTermWidget *>( currentWidget() );
	QString cwd = curTerm->currentWorkingDirectory();

	return newTerminal( cwd, "" );
};

void TtyTabWidget::clearTerminal() {

	NBTermWidget *widget = qobject_cast<NBTermWidget *>( currentWidget() );
	widget->clear();
};

void TtyTabWidget::copyToClipboard() {

	NBTermWidget *widget = qobject_cast<NBTermWidget *>( currentWidget() );
	widget->copyClipboard();
};

void TtyTabWidget::pasteClipboard() {

	NBTermWidget *widget = qobject_cast<NBTermWidget *>( currentWidget() );
	widget->pasteClipboard();
};

void TtyTabWidget::closeTab( int tabIndex ) {

	removeTab( tabIndex );

	if ( count() == 0 ) {
		if ( ddMode )
			newTerminal();

		else
			emit close();
	}

	else {
		for( int idx = 0; idx < count(); idx++ )
			setTabText( idx, QString( "Terminal %1" ).arg( idx + 1 ) );
	}
};

void TtyTabWidget::prevTerminal() {

	int idx = currentIndex();
	if ( idx == 0 )
		setCurrentIndex( count() - 1 );

	else
		setCurrentIndex( idx - 1 );
};

void TtyTabWidget::nextTerminal() {

	int idx = currentIndex();
	if ( idx == count() - 1 )
		setCurrentIndex( 0 );

	else
		setCurrentIndex( idx + 1 );
};

void TtyTabWidget::closeTab() {

	if ( qobject_cast<NBTermWidget*>( sender() ) )
		removeTab( indexOf( qobject_cast<NBTermWidget*>( sender() ) ) );

	else
		removeTab( currentIndex() );

	if ( count() == 0 ) {
		if ( ddMode )
			newTerminal();

		else
			emit close();
	}

	else {
		for( int idx = 0; idx < count(); idx++ )
			setTabText( idx, QString( "Terminal %1" ).arg( idx + 1 ) );
	}
};

void TtyTabWidget::printSelection( bool selection ) {

	NBTermWidget *widget = qobject_cast<NBTermWidget *>( currentWidget() );
	widget->copyClipboard();

	QClipboard *klipper = qApp->clipboard();
	qDebug() << klipper->text();

	if ( selection ) {
		widget->pasteSelection();
	}
};
