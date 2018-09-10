/*
	*
	* NBIconDelegate.cpp - ItemDelegate Class for NBIconView class
	*
*/

#include "NBOpenWithDelegate.hpp"

void NBOpenWithDelegate::paint( QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index ) const {

	if ( index.column() != 0 )
		QItemDelegate::paint( painter, option, index );

	else {
		QRect optionRect( option.rect );

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
			* +------------------------------------------------------------------------------------------------------+
			* | +----+                                                                                               |
			* | |icon| This is the text                                                                              |
			* | +----+                                                                                               |
			* +------------------------------------------------------------------------------------------------------+
			*
		*/

		int padding = ( optionRect.height() - iconSize.width() ) / 2;

		// Icon padding
		QRect iconRect;
		// Original X + Image Left Border + Width to make center the icon
		iconRect.setX( optionRect.x() + padding + ( iconSize.width() - iSize.width() ) / 2 );
		// Original Y + Height to make the image center of the icon rectangle
		iconRect.setY( optionRect.y() + ( optionRect.height() - iSize.height() ) / 2 );
		// Icon Size
		iconRect.setSize( iSize );

		QRect textRect;
		// Original X + Image Width + Image-padding
		textRect.setX( optionRect.x() + iconSize.width() + 2 * padding );
		// Vertical Centering, so don't bother
		textRect.setY( optionRect.y() );
		// Original Width - Image Left Border - Image Width - Image Text Gap -Text Right Border
		textRect.setSize( optionRect.size() - QSize( iconSize.width(), 0 ) - 3 * QSize( padding, 0 ) );

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
		if ( option.state & QStyle::State_HasFocus )
			painter->setFont( QFont( painter->font().family(), painter->font().pointSize(), QFont::Bold ) );

		/* Font Mentrics for elided text */
		QFontMetrics fm( painter->font() );
		text = fm.elidedText( text, Qt::ElideRight, textRect.width() );
		painter->drawText( textRect, Qt::AlignVCenter, text );

		painter->restore();
	}
};
