/*
	*
	* NBSKeyBindingsWidget.cpp - The GUI classes for NewBreeze
	*
*/

#include "NBSKeyBindingsWidget.hpp"

NBKeyBindingsWidget::NBKeyBindingsWidget( QWidget *parent ) : QWidget( parent ) {

	QScrollArea *scroller = new QScrollArea();
	scroller->setStyleSheet( scroller->styleSheet() + "\nQScrollArea { border: none; }" );
	scroller->setWidgetResizable( true );

	QGridLayout *keysLyt = new QGridLayout();
	keysLyt->setSpacing( 0 );

	NBKeySequenceButton *aboutNBKS = new NBKeySequenceButton( Settings->Shortcuts.AboutNB.at( 0 ), NBSettings::defaultInstance()->Shortcuts.AboutNB.at( 0 ), "AboutNB", this );
	NBKeySequenceButton *toggleCrumbLEKS = new NBKeySequenceButton( Settings->Shortcuts.ToggleCrumbLE.at( 0 ), NBSettings::defaultInstance()->Shortcuts.ToggleCrumbLE.at( 0 ), "ToggleCrumbLE", this );
	NBKeySequenceButton *viewModeKS = new NBKeySequenceButton( Settings->Shortcuts.ViewMode.at( 0 ), NBSettings::defaultInstance()->Shortcuts.ViewMode.at( 0 ), "ViewMode", this );
	NBKeySequenceButton *addCustomActionKS = new NBKeySequenceButton( Settings->Shortcuts.AddCustomAction.at( 0 ), NBSettings::defaultInstance()->Shortcuts.AddCustomAction.at( 0 ), "AddCustomAction", this );
	NBKeySequenceButton *goHomeKS = new NBKeySequenceButton( Settings->Shortcuts.GoHome.at( 0 ), NBSettings::defaultInstance()->Shortcuts.GoHome.at( 0 ), "GoHome", this );
	NBKeySequenceButton *goUpKS = new NBKeySequenceButton( Settings->Shortcuts.GoUp.at( 0 ), NBSettings::defaultInstance()->Shortcuts.GoUp.at( 0 ), "GoUp", this );
	NBKeySequenceButton *goLeftKS = new NBKeySequenceButton( Settings->Shortcuts.GoLeft.at( 0 ), NBSettings::defaultInstance()->Shortcuts.GoLeft.at( 0 ), "GoLeft", this );
	NBKeySequenceButton *goRightKS = new NBKeySequenceButton( Settings->Shortcuts.GoRight.at( 0 ), NBSettings::defaultInstance()->Shortcuts.GoRight.at( 0 ), "GoRight", this );
	NBKeySequenceButton *newFolderKS = new NBKeySequenceButton( Settings->Shortcuts.NewFolder.at( 0 ), NBSettings::defaultInstance()->Shortcuts.NewFolder.at( 0 ), "NewFolder", this );
	NBKeySequenceButton *newFileKS = new NBKeySequenceButton( Settings->Shortcuts.NewFile.at( 0 ), NBSettings::defaultInstance()->Shortcuts.NewFile.at( 0 ), "NewFile", this );
	NBKeySequenceButton *peekKS = new NBKeySequenceButton( Settings->Shortcuts.Peek.at( 0 ), NBSettings::defaultInstance()->Shortcuts.Peek.at( 0 ), "Peek", this );
	NBKeySequenceButton *reloadKS = new NBKeySequenceButton( Settings->Shortcuts.Reload.at( 0 ), NBSettings::defaultInstance()->Shortcuts.Reload.at( 0 ), "Reload", this );
	NBKeySequenceButton *toggleHiddenKS = new NBKeySequenceButton( Settings->Shortcuts.ToggleHidden.at( 0 ), NBSettings::defaultInstance()->Shortcuts.ToggleHidden.at( 0 ), "ToggleHidden", this );
	NBKeySequenceButton *selectAllKS = new NBKeySequenceButton( Settings->Shortcuts.SelectAll.at( 0 ), NBSettings::defaultInstance()->Shortcuts.SelectAll.at( 0 ), "SelectAll", this );
	NBKeySequenceButton *toggleSideBarKS = new NBKeySequenceButton( Settings->Shortcuts.ToggleSideBar.at( 0 ), NBSettings::defaultInstance()->Shortcuts.ToggleSideBar.at( 0 ), "ToggleSideBar", this );
	NBKeySequenceButton *cutKS = new NBKeySequenceButton( Settings->Shortcuts.Cut.at( 0 ), NBSettings::defaultInstance()->Shortcuts.Cut.at( 0 ), "Cut", this );
	NBKeySequenceButton *copyKS = new NBKeySequenceButton( Settings->Shortcuts.Copy.at( 0 ), NBSettings::defaultInstance()->Shortcuts.Copy.at( 0 ), "Copy", this );
	NBKeySequenceButton *pasteKS = new NBKeySequenceButton( Settings->Shortcuts.Paste.at( 0 ), NBSettings::defaultInstance()->Shortcuts.Paste.at( 0 ), "Paste", this );
	NBKeySequenceButton *renameKS = new NBKeySequenceButton( Settings->Shortcuts.Rename.at( 0 ), NBSettings::defaultInstance()->Shortcuts.Rename.at( 0 ), "Rename", this );
	NBKeySequenceButton *deleteKS = new NBKeySequenceButton( Settings->Shortcuts.Delete.at( 0 ), NBSettings::defaultInstance()->Shortcuts.Delete.at( 0 ), "Delete", this );
	NBKeySequenceButton *trashKS = new NBKeySequenceButton( Settings->Shortcuts.Trash.at( 0 ), NBSettings::defaultInstance()->Shortcuts.Trash.at( 0 ), "Trash", this );
	NBKeySequenceButton *propertiesKS = new NBKeySequenceButton( Settings->Shortcuts.Properties.at( 0 ), NBSettings::defaultInstance()->Shortcuts.Properties.at( 0 ), "Properties", this );
	NBKeySequenceButton *terminalKS = new NBKeySequenceButton( Settings->Shortcuts.Terminal.at( 0 ), NBSettings::defaultInstance()->Shortcuts.Terminal.at( 0 ), "Terminal", this );
	NBKeySequenceButton *inlineTerminalKS = new NBKeySequenceButton( Settings->Shortcuts.InlineTerminal.at( 0 ), NBSettings::defaultInstance()->Shortcuts.InlineTerminal.at( 0 ), "InlineTerminal", this );
	NBKeySequenceButton *nbInfoKS = new NBKeySequenceButton( Settings->Shortcuts.NBInfo.at( 0 ), NBSettings::defaultInstance()->Shortcuts.NBInfo.at( 0 ), "NBInfo", this );
	NBKeySequenceButton *settingsKS = new NBKeySequenceButton( Settings->Shortcuts.Settings.at( 0 ), NBSettings::defaultInstance()->Shortcuts.Settings.at( 0 ), "Settings", this );
	NBKeySequenceButton *customActionsKS = new NBKeySequenceButton( Settings->Shortcuts.CustomActions.at( 0 ), NBSettings::defaultInstance()->Shortcuts.CustomActions.at( 0 ), "CustomActions", this );
	NBKeySequenceButton *focusAddressBarKS = new NBKeySequenceButton( Settings->Shortcuts.FocusAddressBar.at( 0 ), NBSettings::defaultInstance()->Shortcuts.FocusAddressBar.at( 0 ), "FocusAddressBar", this );
	NBKeySequenceButton *newWindowKS = new NBKeySequenceButton( Settings->Shortcuts.NewWindow.at( 0 ), NBSettings::defaultInstance()->Shortcuts.NewWindow.at( 0 ), "NewWindow", this );
	NBKeySequenceButton *addBookmarkKS = new NBKeySequenceButton( Settings->Shortcuts.AddBookmark.at( 0 ), NBSettings::defaultInstance()->Shortcuts.AddBookmark.at( 0 ), "AddBookmark", this );
	NBKeySequenceButton *focusSearchBarKS = new NBKeySequenceButton( Settings->Shortcuts.FocusSearchBar.at( 0 ), NBSettings::defaultInstance()->Shortcuts.FocusSearchBar.at( 0 ), "FocusSearchBar", this );
	NBKeySequenceButton *clearSearchBarKS = new NBKeySequenceButton( Settings->Shortcuts.ClearSearchBar.at( 0 ), NBSettings::defaultInstance()->Shortcuts.ClearSearchBar.at( 0 ), "ClearSearchBar", this );

	keysLyt->addWidget( new QLabel( "AboutNB" ), 0, 0 );
	keysLyt->addWidget( new QLabel( "Toggle CrumbsBar" ), 1, 0 );
	keysLyt->addWidget( new QLabel( "ViewMode" ), 2, 0 );
	keysLyt->addWidget( new QLabel( "Add Custom Action" ), 3, 0 );
	keysLyt->addWidget( new QLabel( "Go Home" ), 4, 0 );
	keysLyt->addWidget( new QLabel( "Go Up" ), 5, 0 );
	keysLyt->addWidget( new QLabel( "Go Prev Dir" ), 6, 0 );
	keysLyt->addWidget( new QLabel( "Go Next Dir" ), 7, 0 );
	keysLyt->addWidget( new QLabel( "New Folder" ), 8, 0 );
	keysLyt->addWidget( new QLabel( "New File" ), 9, 0 );
	keysLyt->addWidget( new QLabel( "Peek" ), 10, 0 );
	keysLyt->addWidget( new QLabel( "Reload" ), 11, 0 );
	keysLyt->addWidget( new QLabel( "Toggle Hidden Files" ), 12, 0 );
	keysLyt->addWidget( new QLabel( "Select All" ), 13, 0 );
	keysLyt->addWidget( new QLabel( "Toggle SideBar" ), 14, 0 );
	keysLyt->addWidget( new QLabel( "Cut" ), 15, 0 );
	keysLyt->addWidget( new QLabel( "Copy" ), 16, 0 );
	keysLyt->addWidget( new QLabel( "Paste" ), 17, 0 );
	keysLyt->addWidget( new QLabel( "Rename" ), 18, 0 );
	keysLyt->addWidget( new QLabel( "Delete" ), 19, 0 );
	keysLyt->addWidget( new QLabel( "Trash" ), 20, 0 );
	keysLyt->addWidget( new QLabel( "Properties" ), 21, 0 );
	keysLyt->addWidget( new QLabel( "Terminal" ), 22, 0 );
	keysLyt->addWidget( new QLabel( "Show Inline Terminal" ), 23, 0 );
	keysLyt->addWidget( new QLabel( "Show NBInfo" ), 24, 0 );
	keysLyt->addWidget( new QLabel( "Settings" ), 25, 0 );
	keysLyt->addWidget( new QLabel( "Custom Actions" ), 26, 0 );
	keysLyt->addWidget( new QLabel( "Focus AddressBar" ), 27, 0 );
	keysLyt->addWidget( new QLabel( "New Window" ), 28, 0 );
	keysLyt->addWidget( new QLabel( "Add Bookmark" ), 29, 0 );
	keysLyt->addWidget( new QLabel( "Focus SearchBar" ), 30, 0 );
	keysLyt->addWidget( new QLabel( "Clear SearchBar" ), 31, 0 );

	keysLyt->addWidget( aboutNBKS, 0, 1 );
	keysLyt->addWidget( toggleCrumbLEKS, 1, 1 );
	keysLyt->addWidget( viewModeKS, 2, 1 );
	keysLyt->addWidget( addCustomActionKS, 3, 1 );
	keysLyt->addWidget( goHomeKS, 4, 1 );
	keysLyt->addWidget( goUpKS, 5, 1 );
	keysLyt->addWidget( goLeftKS, 6, 1 );
	keysLyt->addWidget( goRightKS, 7, 1 );
	keysLyt->addWidget( newFolderKS, 8, 1 );
	keysLyt->addWidget( newFileKS, 9, 1 );
	keysLyt->addWidget( peekKS, 10, 1 );
	keysLyt->addWidget( reloadKS, 11, 1 );
	keysLyt->addWidget( toggleHiddenKS, 12, 1 );
	keysLyt->addWidget( selectAllKS, 13, 1 );
	keysLyt->addWidget( toggleSideBarKS, 14, 1 );
	keysLyt->addWidget( cutKS, 15, 1 );
	keysLyt->addWidget( copyKS, 16, 1 );
	keysLyt->addWidget( pasteKS, 17, 1 );
	keysLyt->addWidget( renameKS, 18, 1 );
	keysLyt->addWidget( deleteKS, 19, 1 );
	keysLyt->addWidget( trashKS, 20, 1 );
	keysLyt->addWidget( propertiesKS, 21, 1 );
	keysLyt->addWidget( terminalKS, 22, 1 );
	keysLyt->addWidget( inlineTerminalKS, 23, 1 );
	keysLyt->addWidget( nbInfoKS, 24, 1 );
	keysLyt->addWidget( settingsKS, 25, 1 );
	keysLyt->addWidget( customActionsKS, 26, 1 );
	keysLyt->addWidget( focusAddressBarKS, 27, 1 );
	keysLyt->addWidget( newWindowKS, 28, 1 );
	keysLyt->addWidget( addBookmarkKS, 29, 1 );
	keysLyt->addWidget( focusSearchBarKS, 30, 1 );
	keysLyt->addWidget( clearSearchBarKS, 31, 1 );

	QWidget *scrollWidget = new QWidget( this );
	scrollWidget->setLayout( keysLyt );

	scroller->setWidget( scrollWidget );

	QHBoxLayout *baseLyt = new QHBoxLayout();
	baseLyt->setContentsMargins( QMargins() );
	baseLyt->addWidget( scroller );

	setLayout( baseLyt );
};

NBKeyCaptureWidget::NBKeyCaptureWidget( QKeySequence old, QKeySequence defKey, QWidget *parent ) : QDialog( parent ) {

	newKey = old;
	defaultKey = defKey;

	setWindowModality( Qt::WindowModal );

	setFixedSize( QSize( 500, 150 ) );
	setFocusPolicy( Qt::StrongFocus );

	QLabel *lbl = new QLabel( "Press in the key combination of your choice, BackSpace clears the key combo." );
	lbl->setWordWrap( true );

	QPushButton *okBtn = new QPushButton( QIcon::fromTheme( "dialog-ok-apply", QIcon( ":/icons/ok.png" ) ), "Apply" );
	okBtn->setFocusPolicy( Qt::NoFocus );
	connect( okBtn, SIGNAL( clicked() ), this, SLOT( apply() ) );

	QPushButton *cancelBtn = new QPushButton( QIcon::fromTheme( "edit-delete" ), "Cancel" );
	cancelBtn->setDefault( false );
	cancelBtn->setFocusPolicy( Qt::NoFocus );
	connect( cancelBtn, SIGNAL( clicked() ), this, SLOT( cancel() ) );

	QPushButton *clearBtn = new QPushButton( QIcon::fromTheme( "edit-clear" ), "Clear" );
	clearBtn->setDefault( false );
	clearBtn->setFocusPolicy( Qt::NoFocus );
	connect( clearBtn, SIGNAL( clicked() ), this, SLOT( clear() ) );

	QPushButton *resetBtn = new QPushButton( QIcon::fromTheme( "view-refresh", QIcon( ":/icons/reload.png" ) ), "Reset" );
	resetBtn->setDefault( false );
	resetBtn->setFocusPolicy( Qt::NoFocus );
	connect( resetBtn, SIGNAL( clicked() ), this, SLOT( reset() ) );

	QVBoxLayout *lyt = new QVBoxLayout();
	QHBoxLayout *bLyt = new QHBoxLayout();

	bLyt->addWidget( clearBtn );
	bLyt->addWidget( resetBtn );
	bLyt->addStretch();
	bLyt->addWidget( cancelBtn );
	bLyt->addWidget( okBtn );

	lyt->addWidget( lbl );
	lyt->addStretch();
	lyt->addLayout( bLyt );

	setLayout( lyt );
};

bool NBKeyCaptureWidget::event( QEvent *e ) {

	if ( e->type() == QEvent::KeyPress ) {

		keyPressEvent( static_cast<QKeyEvent*>( e ) );
		return true;
	}

	if ( e->type() == QEvent::ShortcutOverride ) {

		e->accept();
		return true;
	}

	if ( e->type() == QEvent::FocusIn ) {

		grabKeyboard();
		return true;
	}

	if ( e->type() == QEvent::FocusOut ) {

		releaseKeyboard();
		return true;
	}

	return QWidget::event( e );
};

void NBKeyCaptureWidget::keyPressEvent( QKeyEvent *keyEvent ) {

	newKey = QKeySequence();
	repaint();

	int keyQt =  keyEvent->key();

	/*
		*
		* Qt sometimes returns garbage keycodes if it doesn't know a key.
		* We cannot do anything useful with those ( several keys have -1, indistinguishable )
		* and QKeySequence.toString() will also yield a garbage string.
		*
	*/
	if ( keyQt == -1 ) {

		// keu moy supported in Qt
		keyEvent->ignore();
		return;
	}

	//get modifiers key
	modifierKeys = keyEvent->modifiers() & ( Qt::SHIFT | Qt::CTRL | Qt::ALT | Qt::META );

	// switching key type
	switch( keyQt ) {

		case Qt::Key_AltGr: //or else we get unicode salad
		case Qt::Key_Shift:
		case Qt::Key_Control:
		case Qt::Key_Alt:
		case Qt::Key_Meta:
		case Qt::Key_Menu: {
			break;
		}

		case Qt::Key_Backspace: {

			// Modifiers + BackSpace
			if ( modifierKeys ) {
				newKey = QKeySequence( keyQt |= modifierKeys );
				break;
			}

			// This is our reset
			else {
				newKey = QKeySequence( "");
				repaint();
				break;
			}
		}

		default: {
			// We now have a valid key press.
			if ( keyQt ) {
				if ( ( keyQt == Qt::Key_Backtab ) && ( modifierKeys & Qt::SHIFT ) ) {

					keyQt = Qt::Key_Tab | modifierKeys;
				}

				else {

					keyQt |= modifierKeys;
				}

				newKey = QKeySequence( keyQt );
			}
		}
	}

	repaint();
	keyEvent->accept();
};

void NBKeyCaptureWidget::keyReleaseEvent( QKeyEvent *keyEvent ) {

	if ( keyEvent->key() == -1 ) {

		keyEvent->ignore();
		return;
	}

	keyEvent->accept();
};

void NBKeyCaptureWidget::paintEvent( QPaintEvent *pEvent ) {

	QWidget::paintEvent( pEvent );

	QPainter *painter = new QPainter( this );
	painter->setRenderHints( QPainter::Antialiasing );

	QString str = newKey.toString();

	if ( not str.isEmpty() )
		painter->drawText( QRect( 0, 65, width(), 20 ), Qt::AlignCenter, str );

	else {
		if ( modifierKeys ) {
			if ( ( modifierKeys & Qt::META ) )
				str += "Meta+";

			if ( ( modifierKeys & Qt::CTRL ) )
				str += "Ctrl+";

			if ( ( modifierKeys & Qt::ALT ) )
				str += "Alt+";

			if ( ( modifierKeys & Qt::SHIFT ) )
				str += "Shift+";
		}

		painter->drawText( QRect( 0, 65, width(), 20 ), Qt::AlignCenter, str );
	}

	painter->end();
};

void NBKeyCaptureWidget::show() {

	QWidget::show();
	grabKeyboard();
};

void NBKeyCaptureWidget::apply() {

	emit keySequenceChanged( newKey );
	close();
};

void NBKeyCaptureWidget::cancel() {

	close();
};

void NBKeyCaptureWidget::clear() {

	newKey = QKeySequence();
	repaint();
};

void NBKeyCaptureWidget::reset() {

	newKey = QKeySequence( defaultKey );
	repaint();
};

NBKeySequenceButton::NBKeySequenceButton( QKeySequence key, QKeySequence defaultSeq, QString name, QWidget *parent ) : QPushButton( parent ) {

	defaultSequence = QKeySequence( defaultSeq );
	shortcutName = QString( name );

	setIcon( QIcon( ":/icons/shortcuts.png" ) );
	setText( key.toString() );

	connect( this, SIGNAL( clicked() ), this, SLOT( changeKeySequence() ) );
};

void NBKeySequenceButton::changeKeySequence() {

	NBKeyCaptureWidget *capturer = new NBKeyCaptureWidget( text(), defaultSequence, qobject_cast<QWidget*>( parent() ) );
	connect( capturer, SIGNAL( keySequenceChanged( QKeySequence ) ), this, SLOT( setKeySequence( QKeySequence ) ) );

	capturer->show();
};

void NBKeySequenceButton::setKeySequence( QKeySequence newKey ) {

	setText( newKey.toString() );

	QSettings shortcuts( "NewBreeze", "Shortcuts" );
	shortcuts.setValue( shortcutName, newKey.toString() );
	shortcuts.sync();

	Settings->reload();
};
