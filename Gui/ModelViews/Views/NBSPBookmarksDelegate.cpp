/*
	*
	* NBSPBookmarksDelegate.cpp - NewBreeze ItemDelegate Class
	*
*/

#include <NBSPBookmarksDelegate.hpp>
#include <NBBookmarksModel.hpp>
#include <NBTools.hpp>

void NBSPBookmarksDelegate::paint( QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index ) const {

	if ( index.column() == 0 ) {
		painter->save();
		const NBBookmarksModel *model = static_cast<const NBBookmarksModel*>( index.model() );

		QSize iconSize = QSize( 16, 16 );
		QString text = model->data( index, Qt::DisplayRole ).toString();
		QPixmap icon = model->data( index, Qt::DecorationRole ).value<QIcon>().pixmap( iconSize );

		// Font Mentrics for elided text;
		QFontMetrics fm( qApp->font() );

		QRect iconRect, textRect;
		// Original X + Image Left Border + Image Width + Image-Text Gap
		textRect.setX( option.rect.x() + 3 + iconSize.width() + 5 );
		// Vertical Centering, so don't bother
		textRect.setY( option.rect.y() );
		// Original Width - Image Left Border - Image Width - Image Text Gap -Text Right Border
		textRect.setSize( option.rect.size() - QSize( 3, 0 ) - QSize( iconSize.width(), 0 ) - QSize( 5, 0 ) - QSize( 3, 0 ) );

		// Set elided text
		text = fm.elidedText( text, Qt::ElideRight, textRect.width() );

		// Original X + Image Left Border
		iconRect.setX( option.rect.x() + 3 );
		// Original Y + Icon Offset = 2
		iconRect.setY( option.rect.y() + 2 );
		// Icon Size
		iconRect.setSize( iconSize );

		// Basic Painter Settings
		painter->setRenderHint( QPainter::Antialiasing, true );
		painter->setRenderHint( QPainter::HighQualityAntialiasing, true );
		painter->setRenderHint( QPainter::TextAntialiasing, true );

		// Background Painter Settings and Background
		painter->setPen( QPen( Qt::NoPen ) );
		if ( option.state & QStyle::State_Editing )
			painter->setBrush( QBrush( Qt::white ) );

		else if ( ( option.state & QStyle::State_Selected ) and ( option.state & QStyle::State_MouseOver ) )
			if ( ( Settings.General.Style == QString( "LightGray" ) ) or ( Settings.General.Style == QString( "TransLight" ) ) )
				painter->setBrush( Settings.Colors.SelectionMouseBrushColor.darker() );

			else
				painter->setBrush( Settings.Colors.SelectionMouseBrushColor.lighter() );

		else if ( option.state & QStyle::State_Selected )
			if ( ( Settings.General.Style == QString( "LightGray" ) ) or ( Settings.General.Style == QString( "TransLight" ) ) )
				painter->setBrush( Settings.Colors.SelectionBrushColor.darker() );

			else
				painter->setBrush( Settings.Colors.SelectionBrushColor.lighter() );

		else if ( option.state & QStyle::State_MouseOver )
			if ( ( Settings.General.Style == QString( "LightGray" ) ) or ( Settings.General.Style == QString( "TransLight" ) ) )
				painter->setBrush( Settings.Colors.MouseBrushColor.darker() );

			else
				painter->setBrush( Settings.Colors.MouseBrushColor.lighter() );

		else
			painter->setBrush( QBrush( Qt::transparent ) );

		// Paint Background
		painter->drawRoundedRect( option.rect, 7, 7 );

		// Focus Rectangle
		if ( option.state & QStyle::State_HasFocus ) {
			painter->setBrush( Qt::NoBrush );
			QPoint bl = option.rect.bottomLeft() + QPoint( 7, 0 );
			QPoint br = option.rect.bottomRight() - QPoint( 7, 0 );
			if ( ( Settings.General.Style == QString( "LightGray" ) ) or ( Settings.General.Style == QString( "TransLight" ) ) ) {
				painter->setPen(  Settings.Colors.FocusPenColorAlt );
				painter->drawLine( bl, br );
				painter->drawLine( bl + QPoint( 0, -1 ), br - QPoint( 0, 1 ) );
			}

			else {
				painter->setPen( Settings.Colors.FocusPenColor );
				painter->drawLine( bl, br );
				painter->drawLine( bl + QPoint( 0, -1 ), br - QPoint( 0, 1 ) );
			}
		}

		// Paint Icon
		painter->drawPixmap( iconRect, icon );

		// Paint Text
		if ( ( Settings.General.Style == QString( "LightGray" ) ) or ( Settings.General.Style == QString( "TransLight" ) ) )
			painter->setPen( Settings.Colors.TextPenColorAlt );

		else
			painter->setPen( Settings.Colors.TextPenColor );

		painter->drawText( textRect, Qt::AlignLeft | Qt::AlignVCenter, text );

		painter->restore();

		return;
	}

	QItemDelegate::paint( painter, option, index );
};

QSize NBSPBookmarksDelegate::sizeHint( const QStyleOptionViewItem &optItem, const QModelIndex &mIndex ) const {

	Q_UNUSED( mIndex );

	return QSize( optItem.rect.width(), 24 );
};
