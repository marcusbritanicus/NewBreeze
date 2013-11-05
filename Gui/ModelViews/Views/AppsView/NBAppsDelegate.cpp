/*
	*
	* NBIconDelegate.cpp - ItemDelegate Class for NBIconView class
	*
*/

#include <NBAppsDelegate.hpp>
#include <NBApplicationsModel.hpp>

void NBAppsDelegate::paint( QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index ) const {

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
		// Horizontal Centering, so don't bother
		textRect.setX( optionRect.x() );
		// Original Y + Image Top Border + Image Height
		textRect.setY( optionRect.y() + 2 + iconSize.height() + 3 );
		// Text Bottom Border
		textRect.setSize( optionRect.size() - QSize( 0, 2 ) );

		// Set elided text
		text = fm.elidedText( text, Qt::ElideRight, textRect.width() );

		QRect iconRect;
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

		// Original X
		iconRect.setX( optionRect.x() + ( optionRect.width() - iSize.width() ) / 2 );
		// Original Y + Image Top Border + Height to make the image center of the icon rectangle
		iconRect.setY( optionRect.y() + 3 + ( iconSize.height() - iSize.height() ) / 2 );
		// Icon Size
		iconRect.setSize( iSize );

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
		painter->drawRoundedRect( optionRect, 7, 7 );

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
		painter->drawText( textRect, Qt::AlignHCenter, text );

		painter->restore();
	}
};

QSize NBAppsDelegate::sizeHint( const QStyleOptionViewItem &optItem, const QModelIndex &mIndex ) const {

	Q_UNUSED( optItem );
	Q_UNUSED( mIndex );

	return QSize( 110, 70 );
};
