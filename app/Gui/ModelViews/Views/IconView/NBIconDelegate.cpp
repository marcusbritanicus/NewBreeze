/*
	*
	* NBIconDelegate.cpp - ItemDelegate Class for NBIconView class
	*
*/

#include "NBIconDelegate.hpp"
#include "NBItemViewModel.hpp"

void NBIconDelegate::paintIcons( QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index ) const {

	maxLines = 1;
	textLines = 1;

	if ( index.column() != 0 )
		QItemDelegate::paint( painter, option, index );

	else {
		// const NBItemViewModel *model = static_cast<const NBItemViewModel*>( index.model() );

		QRect optionRect( option.rect );

		/* Font Mentrics for elided text */
		QFontMetrics fm( qApp->font() );

		/* Get icon size */
		QSize iconSize( option.decorationSize );

		/* Text to be displayed */
		QString text = index.data( Qt::DisplayRole ).toString();

		/* Icon to be painted */
		QPixmap icon;
		if ( option.state & QStyle::State_Selected )
			icon = index.data( Qt::DecorationRole ).value<QIcon>().pixmap( iconSize, QIcon::Selected );

		else if ( option.state & QStyle::State_MouseOver )
			icon = index.data( Qt::DecorationRole ).value<QIcon>().pixmap( iconSize, QIcon::Active );

		else
			icon = index.data( Qt::DecorationRole ).value<QIcon>().pixmap( iconSize, QIcon::Normal );

		if ( ( icon.size().width() > iconSize.width() ) or ( icon.size().height() > iconSize.height() ) )
			icon = icon.scaled( iconSize, Qt::KeepAspectRatio, Qt::SmoothTransformation );

		/* Icon Size */
		QSize iSize( icon.size() );

		/*
			*
			* In this case, we need to center the image horizontally and vertically.
			* +------------------------+
			* |       +--------+       |
			* |       |        |       |
			* |       |  icon  |       |
			* |       |        |       |
			* |       +--------+       |
			* |    This is the text    |
			* +------------------------+
			*
		*/

		// Icon padding
		int padding = ( int ) round( iconSize.width() * 0.1 );

		QRect textRect;
		// Horizontal Centering, so don't bother
		textRect.setX( optionRect.x() + padding );
		// Original Y + Image Height + Image Padding Top + Text-Image Padding ( max = 3 )
		textRect.setY( optionRect.y() + padding + qMin( 3, padding / 2 ) + iconSize.height() );
		// Left and Right Border
		textRect.setSize( optionRect.size() - QSize( 2 * padding, qMin( 3, padding / 2 ) + padding + iconSize.height() ) );

		// Set elided text
		text = fm.elidedText( text, Qt::ElideRight, textRect.width() );
		if ( iconSize.width() <= 40 ) {
			maxLines = 1;
			textRect.adjust( 0, 3, 0, 0 );
		}
		else if ( iconSize.width() <= 60 ) {
			maxLines = 2;
		}
		else if ( iconSize.width() <= 80 ) {
			maxLines = 3;
		}
		else if ( iconSize.width() <= 100 ) {
			maxLines = 4;
		}
		else if ( iconSize.width() < 120 ) {
			maxLines = 5;
		}
		else {
			maxLines = 6;
		}

		/* Icon rect */
		QRect iconRect;
		// Original X
		iconRect.setX( optionRect.x() + ( optionRect.width() - iSize.width() ) / 2 );
		// Original Y + Image Top Border + Height to make the image center of the icon rectangle
		iconRect.setY( optionRect.y() + padding + ( iconSize.height() - iSize.height() ) / 2 );
		// Icon Size
		iconRect.setSize( iSize );

		painter->save();

		/* Antialiasing for rounded rect */
		painter->setRenderHint( QPainter::Antialiasing, true );

		/* Selection painter settings */
		painter->setPen( QPen( Qt::NoPen ) );
		if ( ( option.state & QStyle::State_Selected ) and ( option.state & QStyle::State_MouseOver ) )
			painter->setBrush( option.palette.color( QPalette::Highlight ).darker( 125 ) );

		else if ( option.state & QStyle::State_Selected )
			painter->setBrush( option.palette.color( QPalette::Highlight ) );

		else if ( option.state & QStyle::State_MouseOver )
			painter->setBrush( option.palette.color( QPalette::Highlight ).lighter( 125 ) );

		else
			painter->setBrush( QBrush( Qt::transparent ) );

		/* Selection rectangle */
		painter->drawRoundedRect( optionRect.adjusted( padding / 2, padding / 2, -padding / 2, -padding / 2 ), 3, 3 );
		painter->restore();

		painter->save();
		/* Focus Rectangle - In our case focus under line */
		if ( option.state & QStyle::State_HasFocus ) {
			painter->setBrush( Qt::NoBrush );
			QPoint bl = optionRect.bottomLeft() + QPoint( 7, -padding / 2 );
			QPoint br = optionRect.bottomRight() - QPoint( 7, padding / 2 );

			QLinearGradient hLine( bl, br );

			hLine.setColorAt( 0, Qt::transparent );
			hLine.setColorAt( 0.3, option.palette.color( QPalette::BrightText ) );
			hLine.setColorAt( 0.7, option.palette.color( QPalette::BrightText ) );
			hLine.setColorAt( 1, Qt::transparent );

			painter->setPen( QPen( QBrush( hLine ), 2 ) );
			painter->drawLine( bl, br );
		}
		painter->restore();

		// Paint Icon
		painter->drawPixmap( iconRect, icon );

		painter->save();
		// Text Painter Settings
		painter->setPen( option.palette.color( QPalette::Text ) );

		// Draw Text
		painter->drawText( textRect, Qt::AlignHCenter, text );

		// Draw the extra details
		painter->setPen( option.palette.color( QPalette::ButtonText ) );
		paintIconTextDetails( painter, textRect, index );

		painter->restore();
	}
};

void NBIconDelegate::paintTiles( QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index ) const {

	maxLines = 1;
	textLines = 1;

	if ( index.column() != 0 )
		QItemDelegate::paint( painter, option, index );

	else {
		const NBItemViewModel *model = static_cast<const NBItemViewModel*>( index.model() );
		QFileInfo ftype = model->nodeInfo( index );

		QRect optionRect( option.rect );

		// Font Mentrics for elided text;
		QFontMetrics fm( qApp->font() );

		// Get icon size
		QSize iconSize( option.decorationSize );

		QString text = index.data( Qt::DisplayRole ).toString();
		QPixmap icon;
		if ( option.state & QStyle::State_Selected )
			icon = index.data( Qt::DecorationRole ).value<QIcon>().pixmap( iconSize, QIcon::Selected );

		else if ( option.state & QStyle::State_MouseOver )
			icon = index.data( Qt::DecorationRole ).value<QIcon>().pixmap( iconSize, QIcon::Active );

		else
			icon = index.data( Qt::DecorationRole ).value<QIcon>().pixmap( iconSize, QIcon::Normal );

		if ( ( icon.size().width() > iconSize.width() ) or ( icon.size().height() > iconSize.height() ) )
			icon = icon.scaled( iconSize, Qt::KeepAspectRatio, Qt::SmoothTransformation );

		QSize iSize( icon.size() );

		/*
			*
			* In this case, we need to center the image horizontally and vertically.
			* The text is rendered vertically centered
			* +-------------------------------+
			* | +--------+                    |
			* | |        |                    |
			* | |  icon  | This is the text   |
			* | |        |                    |
			* | +--------+                    |
			* +-------------------------------+
			*
		*/

		// Icon padding
		int padding = ( int ) round( iconSize.width() * 0.1 );

		QRect textRect;
		// Original X + Image Width + Image-padding
		textRect.setX( optionRect.x() + iconSize.width() + 2 * padding );
		// Vertical Centering, so don't bother
		textRect.setY( optionRect.y() + padding * 1.5 );
		// Original Width - Image Left Border - Image Width - Image Text Gap - Text Right Border
		textRect.setSize( optionRect.size() - QSize( iconSize.width(), 0 ) - 3 * QSize( padding, 0 ) - QSize( 0, padding * 1.5 ) );

		// Set elided text
		text = fm.elidedText( text, Qt::ElideRight, textRect.width() );
		if ( iconSize.height() < 30 ) {
			maxLines = 1;
		}
		else if ( iconSize.height() < 50 ) {
			maxLines = 2;
		}
		else if ( iconSize.height() < 65 ) {
			maxLines = 3;
		}
		else if ( iconSize.height() < 80 ) {
			maxLines = 4;
		}
		else if ( iconSize.height() < 95 ) {
			maxLines = 5;
		}
		else {
			maxLines = 6;
		}

		QRect iconRect;
		// Original X + Image Left Border + Width to make the ima
		iconRect.setX( optionRect.x() + padding + ( iconSize.width() - iSize.width() ) / 2 );
		// Original Y + Image Top Border + Height to make the image center of the icon rectangle
		iconRect.setY( optionRect.y() + ( optionRect.height() - iSize.height() ) / 2 );
		// Icon Size
		iconRect.setSize( iSize );

		painter->save();

		/* Antialiasing for rounded rect */
		painter->setRenderHint( QPainter::Antialiasing, true );

		/* Selection painter settings */
		painter->setPen( QPen( Qt::NoPen ) );
		if ( ( option.state & QStyle::State_Selected ) and ( option.state & QStyle::State_MouseOver ) )
			painter->setBrush( option.palette.color( QPalette::Highlight ).darker( 125 ) );

		else if ( option.state & QStyle::State_Selected )
			painter->setBrush( option.palette.color( QPalette::Highlight ) );

		else if ( option.state & QStyle::State_MouseOver )
			painter->setBrush( option.palette.color( QPalette::Highlight ).lighter( 125 ) );

		else
			painter->setBrush( QBrush( Qt::transparent ) );

		/* Selection rectangle */
		painter->drawRoundedRect( optionRect.adjusted( padding / 2, padding / 2, -padding / 2, -padding / 2 ), 3, 3 );
		painter->restore();

		painter->save();
		/* Focus Rectangle - In our case focus under line */
		if ( option.state & QStyle::State_HasFocus ) {
			painter->setBrush( Qt::NoBrush );
			QPoint bl = optionRect.bottomLeft() + QPoint( 7, -padding / 2 );
			QPoint br = optionRect.bottomRight() - QPoint( 7, padding / 2 );

			QLinearGradient hLine( bl, br );

			hLine.setColorAt( 0, Qt::transparent );
			hLine.setColorAt( 0.3, option.palette.color( QPalette::BrightText ) );
			hLine.setColorAt( 0.7, option.palette.color( QPalette::BrightText ) );
			hLine.setColorAt( 1, Qt::transparent );

			painter->setPen( QPen( QBrush( hLine ), 2 ) );
			painter->drawLine( bl, br );
		}
		painter->restore();

		// Paint Icon
		painter->drawPixmap( iconRect, icon );

		painter->save();
		// Text Painter Settings
		painter->setPen( option.palette.color( QPalette::Text ) );

		// Draw Text
		if ( maxLines == 1 )
			painter->drawText( textRect, Qt::AlignVCenter, text );

		else
			painter->drawText( textRect, text );

		// Draw the extra details
		painter->setPen( option.palette.color( QPalette::ButtonText ) );
		paintTileTextDetails( painter, textRect, index );

		painter->restore();
	}
};

void NBIconDelegate::paintDetails( QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index ) const {

	maxLines = 1;
	textLines = 1;

	if ( index.column() != 0 )
		QItemDelegate::paint( painter, option, index );

	else {
		const NBItemViewModel *model = static_cast<const NBItemViewModel*>( index.model() );
		QFileInfo ftype = model->nodeInfo( index );

		QRect optionRect( option.rect );

		// Font Mentrics for elided text;
		QFontMetrics fm( qApp->font() );

		// Get icon size
		QSize iconSize( option.decorationSize );

		QString text = index.data( Qt::DisplayRole ).toString();
		QPixmap icon;
		if ( option.state & QStyle::State_Selected )
			icon = index.data( Qt::DecorationRole ).value<QIcon>().pixmap( iconSize, QIcon::Selected );

		else if ( option.state & QStyle::State_MouseOver )
			icon = index.data( Qt::DecorationRole ).value<QIcon>().pixmap( iconSize, QIcon::Active );

		else
			icon = index.data( Qt::DecorationRole ).value<QIcon>().pixmap( iconSize, QIcon::Normal );

		if ( ( icon.size().width() > iconSize.width() ) or ( icon.size().height() > iconSize.height() ) )
			icon = icon.scaled( iconSize, Qt::KeepAspectRatio, Qt::SmoothTransformation );

		QSize iSize( icon.size() );

		/*
			*
			* In this case, we need to center the image horizontally and vertically.
			* The text is rendered vertically centered
			* +----------------------------------------------------------------------------------------------------------+
			* | +--------+                                                                                               |
			* | |        |                                                                                               |
			* | |  icon  | This is the text                              Detail1    Detail2      Detail3      Detail4    |
			* | |        |                                                                                               |
			* | +--------+                                                                                               |
			* +----------------------------------------------------------------------------------------------------------+
			*
		*/

		// Icon padding
		int padding = ( int ) round( iconSize.width() * 0.1 );

		QRect textRect;
		// Original X + Image Width + Image-padding
		textRect.setX( optionRect.x() + iconSize.width() + 2 * padding );
		// Vertical Centering, so don't bother
		textRect.setY( optionRect.y() );
		// Original Width - Image Left Border - Image Width - Image Text Gap -Text Right Border
		textRect.setSize( optionRect.size() - QSize( iconSize.width(), 0 ) - 3 * QSize( padding, 0 ) - QSize( 350, 0 ) );

		// Set elided text
		text = fm.elidedText( text, Qt::ElideRight, textRect.width() );
		QList<QRect> rectList;

		// First detail
		rectList << QRect( textRect.x() + textRect.width(), textRect.y(), 75, textRect.height() );
		rectList << QRect( rectList.at( 0 ).x() + 75, textRect.y(), 100, textRect.height() );
		rectList << QRect( rectList.at( 1 ).x() + 100, textRect.y(), 100, textRect.height() );
		rectList << QRect( rectList.at( 2 ).x() + 100, textRect.y(), 75, textRect.height() );

		QRect iconRect;
		// Original X + Image Left Border + Width to make the ima
		iconRect.setX( optionRect.x() + padding + ( iconSize.width() - iSize.width() ) / 2 );
		// Original Y + Height to make the image center of the icon rectangle
		iconRect.setY( optionRect.y() + ( optionRect.height() - iSize.height() ) / 2 );
		// Icon Size
		iconRect.setSize( iSize );

		painter->save();

		/* Antialiasing for rounded rect */
		painter->setRenderHint( QPainter::Antialiasing, true );

		/* Selection painter settings */
		painter->setPen( QPen( Qt::NoPen ) );
		if ( ( option.state & QStyle::State_Selected ) and ( option.state & QStyle::State_MouseOver ) )
			painter->setBrush( option.palette.color( QPalette::Highlight ).darker( 125 ) );

		else if ( option.state & QStyle::State_Selected )
			painter->setBrush( option.palette.color( QPalette::Highlight ) );

		else if ( option.state & QStyle::State_MouseOver )
			painter->setBrush( option.palette.color( QPalette::Highlight ).lighter( 125 ) );

		else
			painter->setBrush( QBrush( Qt::transparent ) );

		/* Selection rectangle */
		painter->drawRoundedRect( optionRect.adjusted( padding / 2, padding / 2, -padding / 2, -padding / 2 ), 3, 3 );
		painter->restore();

		painter->save();
		/* Focus Rectangle - In our case focus under line */
		if ( option.state & QStyle::State_HasFocus ) {
			painter->setBrush( Qt::NoBrush );
			QPoint bl = optionRect.bottomLeft() + QPoint( 7, -padding / 2 );
			QPoint br = optionRect.bottomRight() - QPoint( 7, padding / 2 );

			QLinearGradient hLine( bl, br );

			hLine.setColorAt( 0, Qt::transparent );
			hLine.setColorAt( 0.3, option.palette.color( QPalette::BrightText ) );
			hLine.setColorAt( 0.7, option.palette.color( QPalette::BrightText ) );
			hLine.setColorAt( 1, Qt::transparent );

			painter->setPen( QPen( QBrush( hLine ), 2 ) );
			painter->drawLine( bl, br );
		}
		painter->restore();

		// Paint Icon
		painter->drawPixmap( iconRect, icon );

		painter->save();
		// Text Painter Settings
		painter->setPen( option.palette.color( QPalette::Text ) );

		// Draw Text
		painter->drawText( textRect, Qt::AlignVCenter, text );

		// Draw the extra details
		painter->setPen( option.palette.color( QPalette::ButtonText ) );
		paintExtraDetails( painter, rectList, index );

		painter->restore();
	}
};

void NBIconDelegate::paintIconTextDetails( QPainter *painter, QRect &textRect, const QModelIndex &index ) const {

	// The First Line is already drawn
	// Padding between the lines is 3px
	// TextRect needs to be adjusted: y

	/* @painter will already have the correct color, we just have to draw the text */

	painter->save();

	int lineSpacing = ( textRect.height() - QFontInfo( qApp->font() ).pixelSize() * maxLines ) / ( maxLines + 1 );

	switch( maxLines ) {
		case 2 : {
			/* One detail */
			textRect.adjust( 0, lineSpacing + QFontInfo( qApp->font() ).pixelSize(), 0, 0 );
			QString detail = index.data( Qt::UserRole + 1 ).toString();
			painter->drawText( textRect, Qt::AlignHCenter, detail );
			break;
		}

		case 3 : {
			/* Two details */
			textRect.adjust( 0, lineSpacing + QFontInfo( qApp->font() ).pixelSize(), 0, 0 );
			QString detail = index.data( Qt::UserRole + 1 ).toString();
			painter->drawText( textRect, Qt::AlignHCenter, detail );

			textRect.adjust( 0, lineSpacing + QFontInfo( qApp->font() ).pixelSize(), 0, 0 );
			detail = index.data( Qt::UserRole + 3 ).toString();
			painter->drawText( textRect, Qt::AlignHCenter, detail );

			break;
		}

		case 4 : {

			/* Three details */
			textRect.adjust( 0, lineSpacing + QFontInfo( qApp->font() ).pixelSize(), 0, 0 );
			QString detail = index.data( Qt::UserRole + 1 ).toString();
			painter->drawText( textRect, Qt::AlignHCenter, detail );

			textRect.adjust( 0, lineSpacing + QFontInfo( qApp->font() ).pixelSize(), 0, 0 );
			detail = index.data( Qt::UserRole + 3 ).toString();
			painter->drawText( textRect, Qt::AlignHCenter, detail );

			textRect.adjust( 0, lineSpacing + QFontInfo( qApp->font() ).pixelSize(), 0, 0 );
			detail = index.data( Qt::UserRole + 4 ).toString();
			painter->drawText( textRect, Qt::AlignHCenter, detail );

			break;
		}

		case 5 : {

			/* Four details */
			textRect.adjust( 0, lineSpacing + QFontInfo( qApp->font() ).pixelSize(), 0, 0 );
			QString detail = index.data( Qt::UserRole + 1 ).toString();
			painter->drawText( textRect, Qt::AlignHCenter, detail );

			textRect.adjust( 0, lineSpacing + QFontInfo( qApp->font() ).pixelSize(), 0, 0 );
			detail = index.data( Qt::UserRole + 3 ).toString();
			painter->drawText( textRect, Qt::AlignHCenter, detail );

			textRect.adjust( 0, lineSpacing + QFontInfo( qApp->font() ).pixelSize(), 0, 0 );
			detail = index.data( Qt::UserRole + 4 ).toString();
			painter->drawText( textRect, Qt::AlignHCenter, detail );

			textRect.adjust( 0, lineSpacing + QFontInfo( qApp->font() ).pixelSize(), 0, 0 );
			detail = index.data( Qt::UserRole + 5 ).toString();
			painter->drawText( textRect, Qt::AlignHCenter, detail );

			break;
		}

		case 6 : {

			/* Five details */
			textRect.adjust( 0, lineSpacing + QFontInfo( qApp->font() ).pixelSize(), 0, 0 );
			QString detail = index.data( Qt::UserRole + 1 ).toString();
			painter->drawText( textRect, Qt::AlignHCenter, detail );

			textRect.adjust( 0, lineSpacing + QFontInfo( qApp->font() ).pixelSize(), 0, 0 );
			detail = index.data( Qt::UserRole + 3 ).toString();
			painter->drawText( textRect, Qt::AlignHCenter, detail );

			textRect.adjust( 0, lineSpacing + QFontInfo( qApp->font() ).pixelSize(), 0, 0 );
			detail = index.data( Qt::UserRole + 4 ).toString();
			painter->drawText( textRect, Qt::AlignHCenter, detail );

			textRect.adjust( 0, lineSpacing + QFontInfo( qApp->font() ).pixelSize(), 0, 0 );
			detail = index.data( Qt::UserRole + 5 ).toString();
			painter->drawText( textRect, Qt::AlignHCenter, detail );

			textRect.adjust( 0, lineSpacing + QFontInfo( qApp->font() ).pixelSize(), 0, 0 );
			detail = index.data( Qt::UserRole + 6 ).toString();
			painter->drawText( textRect, Qt::AlignHCenter, detail );

			break;
		}

		default: {
			break;
		}
	}

	painter->restore();
};

void NBIconDelegate::paintTileTextDetails( QPainter *painter, QRect &textRect, const QModelIndex &index ) const {

	// The First Line is already drawn
	// Padding between the lines is 3px
	// TextRect needs to be adjusted: y

	painter->save();

	int lineSpacing = ( textRect.height() - QFontInfo( qApp->font() ).pixelSize() * maxLines ) / ( maxLines + 1 );

	switch( maxLines ) {
		case 2 : {
			textRect.adjust( 0, lineSpacing + QFontInfo( qApp->font() ).pixelSize(), 0, 0 );
			QString detail = index.data( Qt::UserRole + 1 ).toString();
			painter->drawText( textRect, detail );
			break;
		}
		case 3 : {
			textRect.adjust( 0, lineSpacing + QFontInfo( qApp->font() ).pixelSize(), 0, 0 );
			QString detail = index.data( Qt::UserRole + 1 ).toString();
			painter->drawText( textRect, detail );

			textRect.adjust( 0, lineSpacing + QFontInfo( qApp->font() ).pixelSize(), 0, 0 );
			detail = index.data( Qt::UserRole + 2 ).toString();
			painter->drawText( textRect, detail );

			break;
		}
		case 4 : {
			textRect.adjust( 0, lineSpacing + QFontInfo( qApp->font() ).pixelSize(), 0, 0 );
			QString detail = index.data( Qt::UserRole + 1 ).toString();
			painter->drawText( textRect, detail );

			textRect.adjust( 0, lineSpacing + QFontInfo( qApp->font() ).pixelSize(), 0, 0 );
			detail = index.data( Qt::UserRole + 3 ).toString();
			painter->drawText( textRect, detail );

			textRect.adjust( 0, lineSpacing + QFontInfo( qApp->font() ).pixelSize(), 0, 0 );
			detail = index.data( Qt::UserRole + 4 ).toString();
			painter->drawText( textRect, detail );

			break;
		}
		case 5 : {
			textRect.adjust( 0, lineSpacing + QFontInfo( qApp->font() ).pixelSize(), 0, 0 );
			QString detail = index.data( Qt::UserRole + 1 ).toString();
			painter->drawText( textRect, detail );

			textRect.adjust( 0, lineSpacing + QFontInfo( qApp->font() ).pixelSize(), 0, 0 );
			detail = index.data( Qt::UserRole + 3 ).toString();
			painter->drawText( textRect, detail );

			textRect.adjust( 0, lineSpacing + QFontInfo( qApp->font() ).pixelSize(), 0, 0 );
			detail = index.data( Qt::UserRole + 4 ).toString();
			painter->drawText( textRect, detail );

			textRect.adjust( 0, lineSpacing + QFontInfo( qApp->font() ).pixelSize(), 0, 0 );
			detail = index.data( Qt::UserRole + 5 ).toString();
			painter->drawText( textRect, detail );

			break;
		}
		case 6 : {
			textRect.adjust( 0, lineSpacing + QFontInfo( qApp->font() ).pixelSize(), 0, 0 );
			QString detail = index.data( Qt::UserRole + 1 ).toString();
			painter->drawText( textRect, detail );

			textRect.adjust( 0, lineSpacing + QFontInfo( qApp->font() ).pixelSize(), 0, 0 );
			detail = index.data( Qt::UserRole + 3 ).toString();
			painter->drawText( textRect, detail );

			textRect.adjust( 0, lineSpacing + QFontInfo( qApp->font() ).pixelSize(), 0, 0 );
			detail = index.data( Qt::UserRole + 4 ).toString();
			painter->drawText( textRect, detail );

			textRect.adjust( 0, lineSpacing + QFontInfo( qApp->font() ).pixelSize(), 0, 0 );
			detail = index.data( Qt::UserRole + 5 ).toString();
			painter->drawText( textRect, detail );

			textRect.adjust( 0, lineSpacing + QFontInfo( qApp->font() ).pixelSize(), 0, 0 );
			detail = index.data( Qt::UserRole + 6 ).toString();
			painter->drawText( textRect, detail );

			break;
		}
		default: {
			break;
		}
	}

	painter->restore();
};

void NBIconDelegate::paintExtraDetails( QPainter *painter, QList<QRect> &textRectList, const QModelIndex &index ) const {

	// The First Line is already drawn
	// Padding between the lines is 3px
	// TextRect needs to be adjusted: y

	painter->save();

	QRect textRect;

	textRect = textRectList.at( 0 );
	QString detail = index.data( Qt::UserRole + 1 ).toString();
	painter->drawText( textRect, Qt::AlignCenter | Qt::TextWordWrap, detail );

	textRect = textRectList.at( 1 );
	detail = index.data( Qt::UserRole + 3 ).toString();
	painter->drawText( textRect, Qt::AlignCenter | Qt::TextWordWrap, detail );

	textRect = textRectList.at( 2 );
	detail = index.data( Qt::UserRole + 4 ).toString();
	painter->drawText( textRect, Qt::AlignCenter | Qt::TextWordWrap, detail );

	textRect = textRectList.at( 3 );
	detail = index.data( Qt::UserRole + 6 ).toString();
	painter->drawText( textRect, Qt::AlignCenter | Qt::TextWordWrap, detail );

	painter->restore();
};
