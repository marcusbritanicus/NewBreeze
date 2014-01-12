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
		QPixmap icon = model->data( index, Qt::DecorationRole ).value<QIcon>().pixmap( QSize( 24, 24 ) );
		QSize iSize = icon.size();

		QRect textRect;
		// Original X + Image Left Border + Image Width + Image-Text Gap
		textRect.setX( option.rect.x() + 5 + 5 + 24 );
		// Vertical Centering, so don't bother
		textRect.setY( option.rect.y() );
		// Original Width - Image Left Border - Image Width - Image Text Gap -Text Right Border
		textRect.setSize( option.rect.size() - QSize( 5, 0 ) - QSize( 5, 0 ) - QSize( 3, 0 ) );

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
			iconRect.setX( option.rect.x() + 2 );
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

			// Original X + Width to make the image center of the image rectangle + Image left border
			iconRect.setX( option.rect.x() + ( 24 - iSize.width() ) / 2 + 5 );
			// Original Y + Image Top Border
			iconRect.setY( option.rect.y() + 1 );
			// Icon Size
			iconRect.setSize( iSize );
		}

		else {
			/* Here, we need to center the image horizontally and vertically */

			// Original X + Width to make the image center of the image rectangle + Imge left border
			iconRect.setX( option.rect.x() + ( 24 - iSize.width() ) / 2 + 5 );
			// Original Y + Height to make the image center of the image rectangle
			iconRect.setY( option.rect.y() + ( option.rect.height() - iSize.height() ) / 2 );
			// Icon Size
			iconRect.setSize( iSize );
		}

		painter->save();

		if ( index.flags() != Qt::NoItemFlags ) {
			// Basic Painter Settings
			painter->setRenderHint( QPainter::Antialiasing, true );
			painter->setRenderHint( QPainter::HighQualityAntialiasing, true );
			painter->setRenderHint( QPainter::TextAntialiasing, true );

			// Background Painter Settings and Background
			painter->setPen( QPen( Qt::NoPen ) );
			if ( option.state & QStyle::State_MouseOver ) {
				if ( ( Settings->General.Style == QString( "LightGray" ) ) or ( Settings->General.Style == QString( "TransLight" ) ) )
					painter->setBrush( Settings->Colors.MouseBrushColor.darker() );

				else
					painter->setBrush( Settings->Colors.MouseBrushColor );

				painter->setPen( Qt::gray );
				painter->drawLine( option.rect.topLeft(), option.rect.topRight() );
				painter->drawLine( option.rect.bottomLeft(), option.rect.bottomRight() );
				painter->setPen( Qt::NoPen );
			}

			else {
				painter->setBrush( QBrush( Qt::transparent ) );
			}

			// Paint Background
			painter->drawRect( option.rect );
		}

		// Paint Icon
		painter->setPen( Qt::transparent );
		painter->setBrush( Qt::transparent );
		painter->drawPixmap( iconRect, icon );

		// Text Painter Settings
		if ( ( Settings->General.Style == QString( "LightGray" ) ) or ( Settings->General.Style == QString( "TransLight" ) ) )
			painter->setPen( Settings->Colors.TextPenColorAlt );

		else
			painter->setPen( Settings->Colors.TextPenColor );

		painter->drawText( textRect, Qt::AlignVCenter, text );
		painter->restore();
	}
};
