/*
	*
	* NBSKeyBindingsWidget.cpp - The GUI classes for NewBreeze
	*
*/

#include "NBSKeyBindingsWidget.hpp"

QSettings *NBKeyBindingsWidget::userSett = NULL;
QSettings *NBKeyBindingsWidget::defaultSett = NULL;

NBKeyBindingsWidget::NBKeyBindingsWidget( QWidget *parent ) : QWidget( parent ) {

	NBKeyBindingsWidget::userSett = new QSettings( "NewBreeze", "NewBreeze" );
	NBKeyBindingsWidget::defaultSett = new QSettings( ":/data/NewBreeze.conf", QSettings::NativeFormat );

	QScrollArea *scroller = new QScrollArea();
	scroller->setStyleSheet( scroller->styleSheet() + "\nQScrollArea { border: none; }" );
	scroller->setWidgetResizable( true );

	QGridLayout *keysLyt = new QGridLayout();
	keysLyt->setSpacing( 0 );

	QSettings xyz( ":/data/NewBreeze.conf", QSettings::NativeFormat );

	NBKeySequenceButton *aboutNBKS = new NBKeySequenceButton( shortcut( "AboutNB" ), shortcut( "AboutNB", true ), "AboutNB", this );
	NBKeySequenceButton *toggleCrumbLEKS = new NBKeySequenceButton( shortcut( "ToggleCrumbLE" ), shortcut( "ToggleCrumbLE", true ), "ToggleCrumbLE", this );
	NBKeySequenceButton *viewModeKS = new NBKeySequenceButton( shortcut( "ViewMode" ), shortcut( "ViewMode", true ), "ViewMode", this );
	NBKeySequenceButton *addCustomActionKS = new NBKeySequenceButton( shortcut( "AddCustomAction" ), shortcut( "AddCustomAction", true ), "AddCustomAction", this );
	NBKeySequenceButton *goHomeKS = new NBKeySequenceButton( shortcut( "GoHome" ), shortcut( "GoHome", true ), "GoHome", this );
	NBKeySequenceButton *goUpKS = new NBKeySequenceButton( shortcut( "GoUp" ), shortcut( "GoUp", true ), "GoUp", this );
	NBKeySequenceButton *goLeftKS = new NBKeySequenceButton( shortcut( "GoLeft" ), shortcut( "GoLeft", true ), "GoLeft", this );
	NBKeySequenceButton *goRightKS = new NBKeySequenceButton( shortcut( "GoRight" ), shortcut( "GoRight", true ), "GoRight", this );
	NBKeySequenceButton *newFolderKS = new NBKeySequenceButton( shortcut( "NewFolder" ), shortcut( "NewFolder", true ), "NewFolder", this );
	NBKeySequenceButton *newFileKS = new NBKeySequenceButton( shortcut( "NewFile" ), shortcut( "NewFile", true ), "NewFile", this );
	NBKeySequenceButton *peekKS = new NBKeySequenceButton( shortcut( "Peek" ), shortcut( "Peek", true ), "Peek", this );
	NBKeySequenceButton *reloadKS = new NBKeySequenceButton( shortcut( "Reload" ), shortcut( "Reload", true ), "Reload", this );
	NBKeySequenceButton *toggleHiddenKS = new NBKeySequenceButton( shortcut( "ToggleHidden" ), shortcut( "ToggleHidden", true ), "ToggleHidden", this );
	NBKeySequenceButton *selectAllKS = new NBKeySequenceButton( shortcut( "SelectAll" ), shortcut( "SelectAll", true ), "SelectAll", this );
	NBKeySequenceButton *toggleSideBarKS = new NBKeySequenceButton( shortcut( "ToggleSideBar" ), shortcut( "ToggleSideBar", true ), "ToggleSideBar", this );
	NBKeySequenceButton *cutKS = new NBKeySequenceButton( shortcut( "Cut" ), shortcut( "Cut", true ), "Cut", this );
	NBKeySequenceButton *copyKS = new NBKeySequenceButton( shortcut( "Copy" ), shortcut( "Copy", true ), "Copy", this );
	NBKeySequenceButton *pasteKS = new NBKeySequenceButton( shortcut( "Paste" ), shortcut( "Paste", true ), "Paste", this );
	NBKeySequenceButton *renameKS = new NBKeySequenceButton( shortcut( "Rename" ), shortcut( "Rename", true ), "Rename", this );
	NBKeySequenceButton *deleteKS = new NBKeySequenceButton( shortcut( "Delete" ), shortcut( "Delete", true ), "Delete", this );
	NBKeySequenceButton *trashKS = new NBKeySequenceButton( shortcut( "Trash" ), shortcut( "Trash", true ), "Trash", this );
	NBKeySequenceButton *propertiesKS = new NBKeySequenceButton( shortcut( "Properties" ), shortcut( "Properties", true ), "Properties", this );
	NBKeySequenceButton *terminalKS = new NBKeySequenceButton( shortcut( "Terminal" ), shortcut( "Terminal", true ), "Terminal", this );
	NBKeySequenceButton *inlineTerminalKS = new NBKeySequenceButton( shortcut( "InlineTerminal" ), shortcut( "InlineTerminal", true ), "InlineTerminal", this );
	NBKeySequenceButton *nbInfoKS = new NBKeySequenceButton( shortcut( "NBInfo" ), shortcut( "NBInfo", true ), "NBInfo", this );
	NBKeySequenceButton *settingsKS = new NBKeySequenceButton( shortcut( "Settings" ), shortcut( "Settings", true ), "Settings", this );
	NBKeySequenceButton *customActionsKS = new NBKeySequenceButton( shortcut( "CustomActions" ), shortcut( "CustomActions", true ), "CustomActions", this );
	NBKeySequenceButton *focusAddressBarKS = new NBKeySequenceButton( shortcut( "FocusAddressBar" ), shortcut( "FocusAddressBar", true ), "FocusAddressBar", this );
	NBKeySequenceButton *newWindowKS = new NBKeySequenceButton( shortcut( "NewWindow" ), shortcut( "NewWindow", true ), "NewWindow", this );
	NBKeySequenceButton *addBookmarkKS = new NBKeySequenceButton( shortcut( "AddBookmark" ), shortcut( "AddBookmark", true ), "AddBookmark", this );
	NBKeySequenceButton *focusSearchBarKS = new NBKeySequenceButton( shortcut( "FocusSearchBar" ), shortcut( "FocusSearchBar", true ), "FocusSearchBar", this );
	NBKeySequenceButton *clearSearchBarKS = new NBKeySequenceButton( shortcut( "ClearSearchBar" ), shortcut( "ClearSearchBar", true ), "ClearSearchBar", this );

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
	keysLyt->addWidget( new QLabel( "Toggle SidePanel" ), 14, 0 );
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

	qDebug() << "Settings shortcuts";
};

QKeySequence NBKeyBindingsWidget::shortcut( QString action, bool nb3 ) {

	/* NB3 Inbuilt default */
	if ( nb3 ) {
		QStringList keys = defaultSett->value( "Shortcuts/" + action ).toStringList();
		return QKeySequence( keys.at( 0 ) );
	}

	else {
		QStringList keys = userSett->value( "Shortcuts/" + action, defaultSett->value( "Shortcuts/" + action ).toStringList() ).toStringList();
		return QKeySequence( keys.at( 0 ) );
	}

	return QKeySequence();
}

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

	QSettings shortcuts( "NewBreeze", "NewBreeze" );
	shortcuts.setValue( "Shortcuts/" + shortcutName, newKey.toString() );
	shortcuts.sync();

	Settings->reload();
};
