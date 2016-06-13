/*
	*
	* NBSStyleWidget.cpp - Style Settings Widget for NewBreeze
	*
*/

#include "NBSStyleWidget.hpp"

NBSStyleWidget::NBSStyleWidget( QWidget * parent ) : QWidget( parent ) {

	styleListCB = new QComboBox( this );
	styleListCB->addItems( QStyleFactory::keys() );
	styleListCB->setCurrentIndex( QStyleFactory::keys().indexOf( Settings->General.Style ) );
	connect( styleListCB, SIGNAL( currentIndexChanged( const QString ) ), this, SLOT( stylePreview( const QString ) ) );

	QHBoxLayout *sLyt = new QHBoxLayout();
	sLyt->addWidget( new QLabel( "Style:" ) );
	sLyt->addWidget( styleListCB );
	sLyt->addStretch();

	preview = new NBSSPreviewWidget( this );

	segBtn = new NBSegmentButton( this );
	segBtn->setCount( 1 );
	segBtn->setSegmentIcon( 0, QIcon::fromTheme( "dialog-apply-ok" ) );
	segBtn->setSegmentText( 0, "&Apply" );
	segBtn->segment( 0 )->setObjectName( "okBtn" );
	connect( segBtn, SIGNAL( clicked( int ) ), this, SLOT( applyStyle() ) );

	QHBoxLayout *btnLyt = new QHBoxLayout();
	btnLyt->addStretch();
	btnLyt->addWidget( segBtn );

	QVBoxLayout *styleLyt = new QVBoxLayout();
	styleLyt->addLayout( sLyt );
	styleLyt->addStretch();
	styleLyt->addWidget( preview );
	styleLyt->addStretch();
	styleLyt->addLayout( btnLyt );

	QGroupBox *styleGB = new QGroupBox( "Styling", this );
	styleGB->setLayout( styleLyt );

	QVBoxLayout *baseLyt = new QVBoxLayout();
	baseLyt->addWidget( styleGB );

	setLayout( baseLyt );
};

void NBSStyleWidget::stylePreview( const QString style ) {

	preview->showStyle( style );
};

NBSSPreviewWidget::NBSSPreviewWidget( QWidget *parent ) : QWidget( parent ) {

	QTabWidget *tabWidget = new QTabWidget( this );

	/* Tab 1 */
	QComboBox *comboBox1 = new QComboBox( this );
	comboBox1->addItems( QStringList() << "One" << "Two" << "Three" );

	QComboBox *comboBox2 = new QComboBox( this );
	comboBox2->addItems( QStringList() << "One" << "Two" << "Three" );
	comboBox2->setEditable( true );

	QVBoxLayout *comboLyt = new QVBoxLayout();
	comboLyt->addWidget( comboBox1 );
	comboLyt->addWidget( comboBox2 );

	QGroupBox *comboGB = new QGroupBox( this );
	comboGB->setLayout( comboLyt );

	QPushButton *pushButton1 = new QPushButton( this );
	pushButton1->setIcon( QIcon::fromTheme( "application-x-executable" ) );
	pushButton1->setText( "Default Push Button" );

	QPushButton *pushButton2 = new QPushButton( this );
	pushButton2->setIcon( QIcon::fromTheme( "application-x-executable" ) );
	pushButton2->setText( "Button with Menu" );

	QMenu *menu1 = new QMenu( this );
	menu1->addAction( QIcon( ":/icons/exec.png" ), "Action 1" );
	menu1->addAction( QIcon( ":/icons/exec.png" ), "Action 2" );
	menu1->addAction( QIcon( ":/icons/exec.png" ), "Action 3" );

	pushButton2->setMenu( menu1 );

	QToolButton *toolButton1 = new QToolButton( this );
	toolButton1->setIcon( QIcon::fromTheme( "preferences-desktop-gaming" ) );

	QToolButton *toolButton2 = new QToolButton( this );
	toolButton2->setIcon( QIcon::fromTheme( "preferences-desktop-gaming" ) );

	QMenu *menu2 = new QMenu( this );
	menu2->addAction( QIcon( ":/icons/exec.png" ), "Action 1" );
	menu2->addAction( QIcon( ":/icons/exec.png" ), "Action 2" );
	menu2->addAction( QIcon( ":/icons/exec.png" ), "Action 3" );

	toolButton2->setMenu( menu2 );
	toolButton2->setPopupMode( QToolButton::MenuButtonPopup );

	QGridLayout *btnLyt = new QGridLayout();
	btnLyt->addWidget( pushButton1, 0, 0 );
	btnLyt->addWidget( Separator::vertical( this ), 0, 1, 2, 1 );
	btnLyt->addWidget( pushButton2, 1, 0 );
	btnLyt->addWidget( toolButton1, 0, 2 );
	btnLyt->addWidget( toolButton2, 1, 2 );

	QGroupBox *btnGB = new QGroupBox( this );
	btnGB->setLayout( btnLyt );

	QRadioButton *radioButton = new QRadioButton( this );
	radioButton->setText( "Default RadioButton" );
	radioButton->setChecked( true );

	QCheckBox *checkBox = new QCheckBox( this );
	checkBox->setTristate( true );
	checkBox->setCheckState( Qt::PartiallyChecked );
	checkBox->setText( "Default CheckBox");

	QScrollBar *scrollBar = new QScrollBar( this );
	scrollBar->setOrientation( Qt::Vertical );
	scrollBar->setRange( 0, 10 );
	scrollBar->setValue( 5 );

	QGridLayout *t1Lyt = new QGridLayout();
	t1Lyt->addWidget( comboGB, 0, 0 );
	t1Lyt->addWidget( btnGB, 0, 1 );
	t1Lyt->addWidget( radioButton, 1, 0 );
	t1Lyt->addWidget( checkBox, 1, 1 );

	QWidget *t1 = new QWidget ( this );
	t1->setLayout( t1Lyt );
	tabWidget->addTab( t1, "Tab 1" );

	/* Tab 2 */
	QSpinBox *spinBox = new QSpinBox( this );
	spinBox->setRange( -100, 100 );
	spinBox->setSuffix( " pixels" );
	spinBox->setSingleStep( 9 );
	spinBox->setValue( 72 );

	QDoubleSpinBox *doubleSpinBox = new QDoubleSpinBox( this );
	doubleSpinBox->setRange( 0, 100000.00 );
	doubleSpinBox->setPrefix( "$ " );
	doubleSpinBox->setValue( 750.00 );

	QLineEdit *lineEditE = new QLineEdit( this );
	lineEditE->setText( "Editable Line/Text Edit" );

	QLineEdit *lineEditD = new QLineEdit( this );
	lineEditD->setText( "Disabled Line/Text Edit" );
	lineEditD->setDisabled( true );

	QGroupBox *groupBox1 = new QGroupBox( "SpinBox", this );
	groupBox1->setCheckable( true );
	groupBox1->setChecked( true );
	QVBoxLayout *g1Lyt = new QVBoxLayout();
	g1Lyt->addWidget( spinBox );
	g1Lyt->addWidget( doubleSpinBox );
	groupBox1->setLayout( g1Lyt );

	QGroupBox *groupBox2 = new QGroupBox( "LineEdit", this );
	QVBoxLayout *g2Lyt = new QVBoxLayout();
	g2Lyt->addWidget( lineEditE );
	g2Lyt->addWidget( lineEditD );
	groupBox2->setLayout( g2Lyt );

	QSlider *slider = new QSlider( this );
	slider->setOrientation( Qt::Horizontal );
	slider->setRange( 0, 100 );
	slider->setTickInterval( 10 );
	slider->setValue( 45 );

	QGridLayout *t2Lyt = new QGridLayout();
	t2Lyt->addWidget( groupBox1, 0, 0 );
	t2Lyt->addWidget( groupBox2, 0, 1 );
	t2Lyt->addWidget( slider, 1, 0, 1, 2 );

	QWidget *t2 = new QWidget ( this );
	t2->setLayout( t2Lyt );
	tabWidget->addTab( t2, "Tab 2" );

	/* Preview Layout */
	QHBoxLayout *pLyt = new QHBoxLayout();
	pLyt->addWidget( tabWidget );
	pLyt->addWidget( scrollBar );

	QGroupBox *previewGB = new QGroupBox( "Preview", this );
	previewGB->setLayout( pLyt );

	QHBoxLayout *baseLyt = new QHBoxLayout();
	baseLyt->addStretch();
	baseLyt->addWidget( previewGB );
	baseLyt->addStretch();

	setLayout( baseLyt );
};

void NBSSPreviewWidget::showStyle( const QString style ) {

	QStyle *newStyle = QStyleFactory::create( style );

	if ( newStyle ) {
		setStyle( newStyle );
		Q_FOREACH( QWidget *w, findChildren<QWidget*>() )
			w->setStyle( newStyle );
	}
};

void NBSStyleWidget::applyStyle() {

	qApp->setStyle( styleListCB->currentText() );
	Settings->setValue( "Style", styleListCB->currentText() );
};
