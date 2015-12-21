/*
	*
	* NBGuiWidgets.cpp - The GUI classes for NewBreeze
	*
*/

#include <NBTools.hpp>
#include <NBSettingsWidgets.hpp>

QSettings NBShortcutsWidget::shortcutSettings( "NewBreeze", "Shortcuts" );
inline QStringList getShortcutList( QString actionName ) {

	if ( not NBShortcutsWidget::shortcutSettings.value( actionName ).isNull() )
		return NBShortcutsWidget::shortcutSettings.value( actionName ).toStringList();

	else
		return QStringList();
};

NBShortcutsWidget::NBShortcutsWidget( QString name ) : QWidget() {

	action = QString( name );

	setKeyBindingBtn = new QPushButton( getShortcutList( name ).value( 0 ) );
	setKeyBindingBtn->setToolTip( "Click to set a new key binding" );
	connect( setKeyBindingBtn, SIGNAL( clicked() ), this, SLOT( changeKeySequence() ) );

	clearBindingBtn = new QToolButton();
	clearBindingBtn->setIcon( QIcon( ":/icons/edit-clear.png" ) );
	clearBindingBtn->setToolTip( "Click to clear the binding" );
	connect( clearBindingBtn, SIGNAL( clicked() ), this, SLOT( clearBinding() ) );

	resetBindingBtn = new QToolButton();
	resetBindingBtn->setIcon( QIcon( ":/icons/reload.png" ) );
	resetBindingBtn->setToolTip( "Click to restore the default key binding" );
	connect( resetBindingBtn, SIGNAL( clicked() ), this, SLOT( resetBinding() ) );

	QHBoxLayout *lyt = new QHBoxLayout();
	lyt->addWidget( setKeyBindingBtn );
	lyt->addWidget( clearBindingBtn );
	lyt->addWidget( resetBindingBtn );

	setLayout( lyt );
};

void NBShortcutsWidget::changeKeySequence() {
};

void NBShortcutsWidget::clearBinding() {

	setKeyBindingBtn->setText( "None" );
	shortcutSettings.setValue( action , QVariant() );
	shortcutSettings.sync();

	Settings->reload();
};

void NBShortcutsWidget::resetBinding() {

	QSettings defaultSettings( ":/Shortcuts.conf", QSettings::NativeFormat );

	setKeyBindingBtn->setText( defaultSettings.value( action ).toString() );
	shortcutSettings.remove( action );
	shortcutSettings.sync();

	Settings->reload();
};
