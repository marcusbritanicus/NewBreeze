/*
	*
	* NBIconDelegate.cpp - ItemDelegate Class for NBIconView class
	*
*/

#include <NBOpenWithDelegate.hpp>

void NBOpenWithDelegate::paint( QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index ) const {

	if ( index.column() != 0 )
		QItemDelegate::paint( painter, option, index );

	else {
		// const NBApplicationsModel *model = static_cast<const NBApplicationsModel*>( index.model() );

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

		QRect textRect;
 		// Original X + Image Left Border + Image Width + Image-Text Gap
		textRect.setX( optionRect.x() + 3 + iconSize.width() + 5 );
		// Vertical Centering, so don't bother
		textRect.setY( optionRect.y() );
		// Original Width - Image Left Border - Image Width - Image Text Gap -Text Right Border
		textRect.setSize( optionRect.size() - QSize( 3, 0 ) - QSize( iconSize.width(), 0 ) - QSize( 5, 0 ) - QSize( 3, 0 ) );

		// Set elided text
		text = fm.elidedText( text, Qt::ElideRight, textRect.width() );

		int padding = ( int ) round( iconSize.width() * 0.1 );
		QRect iconRect;
		if ( iSize.width() > iSize.height() ) {
			/*
				*
				* In this case, we need to center the image vertically.
				* So we add a small padding along the x-axis.
				*
			*/

			// Original X + Image Left Border
			iconRect.setX( optionRect.x() + 2 );
			// Original Y + Height to make the image center of the selection rectangle
			iconRect.setY( optionRect.y() + ( optionRect.height() - iSize.height() ) / 2 );
			// Icon Size
			iconRect.setSize( iSize );
		}

		else if ( iSize.height() > iSize.width() ) {
			/*
				*
				* Here, we need to center the image horizontally.
				* We have no padding on top
				*
			*/

			// Original X + Width to make the image center of the image rectangle
			iconRect.setX( optionRect.x() + ( iconSize.width() - iSize.width() ) / 2 + 2 );
			// Original Y + Image Top Border
			iconRect.setY( optionRect.y() + 1 );
			// Icon Size
			iconRect.setSize( iSize );
		}

		else {
			/*
				*
				* Here, we need to center the image horizontally and vertically
				*
			*/

			// Original X + Width to make the image center of the image rectangle
			iconRect.setX( optionRect.x() + ( iconSize.width() - iSize.width() ) / 2 + 2 );
			// Original Y + Height to make the image center of the image rectangle
			iconRect.setY( optionRect.y() + ( optionRect.height() - iSize.height() ) / 2 );
			// Icon Size
			iconRect.setSize( iSize );
		}

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

		// Paint Background
		painter->drawRoundedRect( optionRect, 7, 7 );
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

		painter->restore();
	}
};
