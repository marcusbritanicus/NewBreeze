/*
	*
	* NBIconDelegate.cpp - ItemDelegate Class for NBIconView class
	*
*/

#include <NBOpenWithDelegate.hpp>
#include <NBApplicationsModel.hpp>

void NBOpenWithDelegate::paint( QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index ) const {

	if ( index.column() != 0 )
		QItemDelegate::paint( painter, option, index );

	else {
		const NBApplicationsModel *model = static_cast<const NBApplicationsModel*>( index.model() );

		QRect optionRect( option.rect );

		// Font Mentrics for elided text;
		QFontMetrics fm( qApp->font() );

		// Get icon size
		QSize iconSize( option.decorationSize );

		QString text = model->data( index, Qt::DisplayRole ).toString();
		QPixmap icon;
		if ( option.state & QStyle::State_Selected )
			icon = model->data( index, Qt::DecorationRole ).value<QIcon>().pixmap( iconSize, QIcon::Selected );

		else if ( option.state & QStyle::State_MouseOver )
			icon = model->data( index, Qt::DecorationRole ).value<QIcon>().pixmap( iconSize, QIcon::Active );

		else
			icon = model->data( index, Qt::DecorationRole ).value<QIcon>().pixmap( iconSize, QIcon::Normal );

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

		// Basic Painter Settings
		painter->setRenderHint( QPainter::Antialiasing, true );
		painter->setRenderHint( QPainter::HighQualityAntialiasing, true );
		painter->setRenderHint( QPainter::TextAntialiasing, true );

		// Background Painter Settings and Background
		painter->setPen( QPen( Qt::NoPen ) );
		if ( ( option.state & QStyle::State_Selected ) and ( option.state & QStyle::State_MouseOver ) ) {
			if ( ( Settings->General.Style == QString( "LightGray" ) ) or ( Settings->General.Style == QString( "TransLight" ) ) )
				painter->setBrush( Settings->Colors.SelectionMouseBrushColor.darker() );

			else
				painter->setBrush( Settings->Colors.SelectionMouseBrushColor );
		}

		else if ( option.state & QStyle::State_Selected ) {
			if ( ( Settings->General.Style == QString( "LightGray" ) ) or ( Settings->General.Style == QString( "TransLight" ) ) )
				painter->setBrush( Settings->Colors.SelectionBrushColor.darker() );

			else
				painter->setBrush( Settings->Colors.SelectionBrushColor );
		}

		else if ( option.state & QStyle::State_MouseOver ) {
			if ( ( Settings->General.Style == QString( "LightGray" ) ) or ( Settings->General.Style == QString( "TransLight" ) ) )
				painter->setBrush( Settings->Colors.MouseBrushColor.darker() );

			else
				painter->setBrush( Settings->Colors.MouseBrushColor );
		}

		else {
			painter->setBrush( QBrush( Qt::transparent ) );
		}

		// Paint Background
		painter->drawRoundedRect( optionRect, 5, 5 );

		// Focus Rectangle
		if ( option.state & QStyle::State_HasFocus ) {
			painter->setBrush( Qt::NoBrush );
			QPoint bl = optionRect.bottomLeft() + QPoint( 7, 0 );
			QPoint br = optionRect.bottomRight() - QPoint( 7, 0 );
			if ( ( Settings->General.Style == QString( "LightGray" ) ) or ( Settings->General.Style == QString( "TransLight" ) ) ) {
				painter->setPen(  Settings->Colors.FocusPenColorAlt );
				painter->drawLine( bl, br );
				painter->drawLine( bl + QPoint( 0, -1 ), br - QPoint( 0, 1 ) );
			}

			else {
				painter->setPen( Settings->Colors.FocusPenColor );
				painter->drawLine( bl, br );
				painter->drawLine( bl + QPoint( 0, -1 ), br - QPoint( 0, 1 ) );
			}
		}

		// Paint Icon
		painter->drawPixmap( iconRect, icon );

		// Text Painter Settings
		if ( ( Settings->General.Style == QString( "LightGray" ) ) or ( Settings->General.Style == QString( "TransLight" ) ) )
			painter->setPen( Settings->Colors.TextPenColorAlt );

		else
			painter->setPen( Settings->Colors.TextPenColor );

		// Draw Text
		painter->drawText( textRect, Qt::AlignLeft | Qt::AlignVCenter, text );

		painter->restore();
	}
};
