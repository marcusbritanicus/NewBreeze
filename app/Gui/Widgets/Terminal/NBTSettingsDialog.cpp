/*
    *
    * * NBTSettingsDialogDialog.cpp - QTermWidget reimplementation for NBTerminal
    *
*/

#include <NBTSettingsDialog.hpp>
#include <qtermwidget.h>

QSettings settings( "NewBreeze", "NBTerminal" );

NBTSettingsDialog::NBTSettingsDialog() : QDialog() {

	createGUI();
};

void NBTSettingsDialog::createGUI() {

	QLabel *shellLabel = new QLabel( "She&ll program" );
	QLabel *comboLabel = new QLabel( "Color &Scheme" );
	QLabel *fontLabel = new QLabel( "&Font" );

	shellEdit = new QLineEdit( this );
	shellEdit->setText( settings.value( "Shell", "/bin/bash" ).toString() );
	shellLabel->setBuddy( shellEdit );
	connect( shellEdit, SIGNAL( textEdited( QString ) ), this, SLOT( saveShell( QString ) ) );

	colorSchemesCombo = new QComboBox();
	colorSchemesCombo->addItems( QTermWidget::availableColorSchemes() );
	colorSchemesCombo->setCurrentIndex( QTermWidget::availableColorSchemes().indexOf( settings.value( "ColorScheme" ).toString() ) );
	connect( colorSchemesCombo, SIGNAL( currentIndexChanged( int ) ), this, SLOT( setColorScheme() ) );

	fontCombo = new QFontComboBox();
	QFont font = settings.value( "Font" ).value<QFont>();
	fontCombo->setCurrentFont( font );
	fontCombo->setFontFilters( QFontComboBox::MonospacedFonts );
	connect( fontCombo, SIGNAL( currentFontChanged( const QFont& ) ), this, SLOT( setFont( QFont ) ) );

	fontSizeSpin = new QSpinBox();
	fontSizeSpin->setRange( 0, 72 );
	fontSizeSpin->setSingleStep( 1 );
	fontSizeSpin->setValue( settings.value( "Font" ).value<QFont>().pointSize() );
	connect( fontSizeSpin, SIGNAL( valueChanged( int ) ), this, SLOT( setFont( int ) ) );

	fontLabel->setBuddy( fontCombo );

	QPushButton *okBtn = new QPushButton( QIcon::fromTheme( "dialog-close" ), tr( "&Close" ) );
	connect( okBtn, SIGNAL( clicked() ), this, SLOT( close() ) );

	QGridLayout *lyt = new QGridLayout();

	lyt->addWidget( shellLabel, 0, 0 );
	lyt->addWidget( shellEdit, 0, 1 );

	lyt->addWidget( comboLabel, 1, 0 );
	lyt->addWidget( colorSchemesCombo, 2, 0 );

	QHBoxLayout *fLyt = new QHBoxLayout();
	fLyt->addWidget( fontLabel );
	fLyt->addStretch();
	fLyt->addWidget( fontCombo );
	fLyt->addWidget( fontSizeSpin );
	lyt->addLayout( fLyt, 3, 0, 1, 2 );

	lyt->addWidget( okBtn, 4, 1, Qt::AlignRight );

	setLayout( lyt );

	setWindowTitle( "NBTerminal - Settings" );
	setWindowIcon( QIcon( ":/icons/qterminal.png" ) );
};

void NBTSettingsDialog::saveShell( QString sh ) {

	if ( exists( sh ) and isExecutable( sh ) ) {
		settings.setValue( "Shell", sh );
		settings.sync();
	}
};

void NBTSettingsDialog::setColorScheme() {

	QString colorScheme = colorSchemesCombo->currentText();
	settings.setValue( "ColorScheme", colorScheme );
	settings.sync();
};

void NBTSettingsDialog::setFont( QFont font ) {

	font.setPointSize( fontSizeSpin->value() );

	settings.setValue( "Font", font );
	settings.sync();
};

void NBTSettingsDialog::setFont( int size ) {

	QFont font( fontCombo->currentFont() );
	font.setPointSize( size );

	settings.setValue( "Font", font );
	settings.sync();
};
