/*
	*
	* NBSStyleWidget.cpp - Style Settings Widget for NewBreeze
	*
*/

#include <NBTools.hpp>
#include <NBSStyleWidget.hpp>

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

	tabWidget = new QTabWidget( this );

	/* Tab 1 */
	comboBox = new QComboBox( this );
	comboBox->addItems( QStringList() << "One" << "Two" << "Three" );

	pushButton = new QPushButton( this );
	pushButton->setIcon( QIcon::fromTheme( "application-x-executable" ) );
	pushButton->setText( "Default Push Button" );

	toolButton = new QToolButton( this );
	toolButton->setIcon( QIcon::fromTheme( "preferences-desktop-gaming" ) );

	radioButton = new QRadioButton( this );
	radioButton->setText( "Default RadioButton" );
	radioButton->setChecked( true );

	checkBox = new QCheckBox( this );
	checkBox->setTristate( true );
	checkBox->setCheckState( Qt::PartiallyChecked );
	checkBox->setText( "Default CheckBox");

	scrollBar = new QScrollBar( this );
	scrollBar->setOrientation( Qt::Vertical );
	scrollBar->setRange( 0, 10 );
	scrollBar->setValue( 5 );

	QGridLayout *t1Lyt = new QGridLayout();
	t1Lyt->addWidget( comboBox, 0, 0 );
	t1Lyt->addWidget( pushButton, 0, 1 );
	t1Lyt->addWidget( toolButton, 0, 2 );
	t1Lyt->addWidget( radioButton, 1, 0 );
	t1Lyt->addWidget( checkBox, 1, 1, 1, 2 );

	QWidget *t1 = new QWidget ( this );
	t1->setLayout( t1Lyt );
	tabWidget->addTab( t1, "Tab 1" );

	/* Tab 2 */
	spinBox = new QSpinBox( this );
	spinBox->setRange( -100, 100 );
	spinBox->setSuffix( " pixels" );
	spinBox->setValue( 72 );

	doubleSpinBox = new QDoubleSpinBox( this );
	doubleSpinBox->setRange( 0, 100000.00 );
	doubleSpinBox->setPrefix( "$ " );
	doubleSpinBox->setValue( 750.00 );

	lineEditE = new QLineEdit( this );
	lineEditE->setText( "Editable Line/Text Edit" );

	lineEditD = new QLineEdit( this );
	lineEditD->setText( "Disabled Line/Text Edit" );
	lineEditD->setDisabled( true );

	groupBox1 = new QGroupBox( "SpinBox", this );
	groupBox1->setCheckable( true );
	groupBox1->setChecked( true );
	QVBoxLayout *g1Lyt = new QVBoxLayout();
	g1Lyt->addWidget( spinBox );
	g1Lyt->addWidget( doubleSpinBox );
	groupBox1->setLayout( g1Lyt );

	groupBox2 = new QGroupBox( "LineEdit", this );
	QVBoxLayout *g2Lyt = new QVBoxLayout();
	g2Lyt->addWidget( lineEditE );
	g2Lyt->addWidget( lineEditD );
	groupBox2->setLayout( g2Lyt );

	slider = new QSlider( this );
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

	setStyle( newStyle );
	tabWidget->setStyle( newStyle );

	pushButton->setStyle( newStyle );
	toolButton->setStyle( newStyle );
	checkBox->setStyle( newStyle );
	radioButton->setStyle( newStyle );
	scrollBar->setStyle( newStyle );
	comboBox->setStyle( newStyle );

	spinBox->setStyle( newStyle );
	doubleSpinBox->setStyle( newStyle );
	lineEditE->setStyle( newStyle );
	lineEditD->setStyle( newStyle );
	groupBox1->setStyle( newStyle );
	groupBox2->setStyle( newStyle );
	slider->setStyle( newStyle );
};

void NBSStyleWidget::applyStyle() {

	qApp->setStyle( styleListCB->currentText() );
	Settings->setValue( "Style", styleListCB->currentText() );
};
