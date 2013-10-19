/*
	*
	* NBSidePanelDelegate.cpp - ItemDelegate Class for NBIconView class
	*
*/

#include <NBSidePanelDelegate.hpp>
#include <NBSidePanelModel.hpp>
#include <NBTools.hpp>

void NBSidePanelDelegate::paint( QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index ) const {

	if ( index.column() != 0 )
		QItemDelegate::paint( painter, option, index );

	else {
		const NBSidePanelModel *model = static_cast<const NBSidePanelModel*>( index.model() );

		// Font Mentrics for elided text;
		QFontMetrics fm( qApp->font() );

		// Get icon size
		QString text = model->data( index, Qt::DisplayRole ).toString();
		QPixmap icon = model->data( index, Qt::DecorationRole ).value<QIcon>().pixmap( QSize( 16, 16 ) );
		QSize iSize = icon.size();

		QRect textRect;
		// Original X + Image Left Border + Image Width + Image-Text Gap
		textRect.setX( option.rect.x() + 3 + 5 );
		// Vertical Centering, so don't bother
		textRect.setY( option.rect.y() );
		// Original Width - Image Left Border - Image Width - Image Text Gap -Text Right Border
		textRect.setSize( option.rect.size() - QSize( 3, 0 ) - QSize( 5, 0 ) - QSize( 3, 0 ) );

		// Set elided text
		text = fm.elidedText( text, Qt::ElideRight, textRect.width() );

		QRect iconRect;
		if ( iSize.width() > iSize.height() ) {
			/*
				*
				* In this case, we need to center the image vertically.
				* So we add a small padding along the x-axis.
				*
			*/

			// Original X + Image Left Border
			iconRect.setX( option.rect.x() + 2 - 16 );
			// Original Y + Height to make the image center of the selection rectangle
			iconRect.setY( option.rect.y() + ( option.rect.height() - iSize.height() ) / 2 );
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
			iconRect.setX( option.rect.x() + ( 16 - iSize.width() ) / 2 + 2 - 16 );
			// Original Y + Image Top Border
			iconRect.setY( option.rect.y() + 1 );
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
			iconRect.setX( option.rect.x() + ( 16 - iSize.width() ) / 2 + 2 - 16 );
			// Original Y + Height to make the image center of the image rectangle
			iconRect.setY( option.rect.y() + ( option.rect.height() - iSize.height() ) / 2 );
			// Icon Size
			iconRect.setSize( iSize );
		}

		painter->save();

		// Basic Painter Settings
		painter->setRenderHint( QPainter::Antialiasing, true );
		painter->setRenderHint( QPainter::HighQualityAntialiasing, true );
		painter->setRenderHint( QPainter::TextAntialiasing, true );

		// Background Painter Settings and Background
		painter->setPen( QPen( Qt::NoPen ) );
		if ( ( option.state & QStyle::State_Selected ) and ( option.state & QStyle::State_MouseOver ) )
			if ( ( Settings->General.Style == QString( "LightGray" ) ) or ( Settings->General.Style == QString( "TransLight" ) ) )
				painter->setBrush( Settings->Colors.SelectionMouseBrushColor.darker() );

			else
				painter->setBrush( Settings->Colors.SelectionMouseBrushColor );

		else if ( option.state & QStyle::State_Selected )
			if ( ( Settings->General.Style == QString( "LightGray" ) ) or ( Settings->General.Style == QString( "TransLight" ) ) )
				painter->setBrush( Settings->Colors.SelectionBrushColor.darker() );

			else
				painter->setBrush( Settings->Colors.SelectionBrushColor );

		else if ( option.state & QStyle::State_MouseOver )
			if ( ( Settings->General.Style == QString( "LightGray" ) ) or ( Settings->General.Style == QString( "TransLight" ) ) )
				painter->setBrush( Settings->Colors.MouseBrushColor.darker() );

			else
				painter->setBrush( Settings->Colors.MouseBrushColor );

		else
			painter->setBrush( QBrush( Qt::transparent ) );

		// Paint Background
		painter->drawRoundedRect( option.rect, 5, 5 );

		// Paint Icon
		painter->drawPixmap( iconRect, icon );

		// Text Painter Settings
		if ( ( Settings->General.Style == QString( "LightGray" ) ) or ( Settings->General.Style == QString( "TransLight" ) ) )
			painter->setPen( Settings->Colors.TextPenColorAlt );

		else
			painter->setPen( Settings->Colors.TextPenColor );

		painter->drawText( textRect, Qt::AlignLeft, text );
		painter->restore();
	}
};
