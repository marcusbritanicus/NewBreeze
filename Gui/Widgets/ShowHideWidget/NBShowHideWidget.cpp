/*
	*
	* NBShowHideWidget.cpp - The Show/Hide on click widget class for NewBreeze
	*
*/

#include <NBShowHideWidget.hpp>
#include <NBTools.hpp>

NBExpanderLabel::NBExpanderLabel( QString text ) : QToolButton() {

	setFixedHeight( 16 );
	setContentsMargins( QMargins() );
	setContentsMargins( QMargins() );
	setToolButtonStyle( Qt::ToolButtonTextBesideIcon );
	widgetShown = true;

	setText( text );
	setArrowType( Qt::DownArrow );

	if ( ( Settings->General.Style == QString( "TransDark" ) ) or ( Settings->General.Style == QString( "TransLight" ) ) )
		setAttribute( Qt::WA_TranslucentBackground );

	setStyleSheet( getStyleSheet( "NBShowHideButton", Settings->General.Style ) );

	if ( QStyleFactory::keys().contains( "Oxygen" ) )
		setStyle( QStyleFactory::create( "Oxygen" ) );

	setFocusPolicy( Qt::NoFocus );
};

void NBExpanderLabel::mousePressEvent( QMouseEvent *mEvent ) {

	if ( not widgetShown ) {
		setArrowType( Qt::DownArrow );
		widgetShown = true;
	}

	else {
		setArrowType( Qt::RightArrow );
		widgetShown = false;
	}

	emit clicked();
	mEvent->accept();
};

NBShowHideWidget::NBShowHideWidget( QString text, QWidget *w ) : QWidget() {

	clickLabel = new NBExpanderLabel( text );

	widget = w;

	connect( clickLabel, SIGNAL( clicked() ), this, SLOT( toggleVisibility() ) );

	QHBoxLayout *lyt = new QHBoxLayout();
	QVBoxLayout *lytBase = new QVBoxLayout();

	lyt->addWidget( clickLabel );
	lyt->addStretch( 0 );

	lytBase->addLayout( lyt );
	lytBase->addWidget( widget );

	setLayout( lytBase );
};

bool NBShowHideWidget::isWidgetVisible() {

	return widget->isVisible();
};

void NBShowHideWidget::toggleVisibility() {

	if ( widget->isVisible() ) {
		widget->hide();
		adjustSize();
		updateGeometry();
	}

	else {
		widget->show();
		adjustSize();
		updateGeometry();
	}

	emit visibilityChanged();
};
