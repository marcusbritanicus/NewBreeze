/*
	*
	* NBFDItemDelegate.cpp - NewBreeze ItemDelegate Class
	*
*/

#include <NBFDItemDelegate.hpp>
#include <NBFileSystemModel.hpp>
#include <NBTools.hpp>

void NBFDItemDelegate::paint( QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index ) const {

	if ( index.column() != 0 )
		QItemDelegate::paint( painter, option, index );

	else {
		const NBFileSystemModel *model = static_cast<const NBFileSystemModel*>( index.model() );
		QFileInfo ftype = model->nodeInfo( index );

		// Font Mentrics for elided text;
		QFontMetrics fm( qApp->font() );

		// Get icon size
		QSize iconSize = QSize( 48, 48 );

		QString text = model->data( index, Qt::DisplayRole ).toString();
		QPixmap icon = model->data( index, Qt::DecorationRole ).value<QIcon>().pixmap( iconSize );
		QSize iSize = icon.size();

		// Text and text boundary
		QRect textRect;
		// Horizontal Centering, so don't bother
		textRect.setX( option.rect.x() );
		// Original Y + Image Top Border + Image Height
		textRect.setY( option.rect.y() + 2 + iconSize.height() + 3 );
		// Text Bottom Border
		textRect.setSize( option.rect.size() - QSize( 0, 2 ) );

		// Set elided text
		text = fm.elidedText( text, Qt::ElideRight, textRect.width() );

		// Icon Boundary
		QRect iconRect;
		// Original X
		iconRect.setX( option.rect.x() + ( option.rect.width() - iconSize.width() ) / 2 );
		// Original Y + Image Top Border + Height to make the image center of the icon rectangle
		iconRect.setY( option.rect.y() + 3 + ( iconSize.height() - iSize.height() ) / 2 );
		// Icon Size
		iconRect.setSize( iSize );
		painter->save();

		// Basic Painter Settings
		painter->setRenderHint( QPainter::Antialiasing, true );
		painter->setRenderHint( QPainter::HighQualityAntialiasing, true );
		painter->setRenderHint( QPainter::TextAntialiasing, true );

		// Background Painter Settings and Background
		painter->setPen( QPen( Qt::NoPen ) );
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
		painter->drawRoundedRect( option.rect, 7, 7 );

		if ( option.state & QStyle::State_HasFocus ) {
			painter->setBrush( Qt::NoBrush );
			QPoint bl = option.rect.bottomLeft() + QPoint( 7, 0 );
			QPoint br = option.rect.bottomRight() - QPoint( 7, 0 );
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
		if ( ftype.isSymLink() ) {
			if ( ( Settings->General.Style == QString( "LightGray" ) ) or ( Settings->General.Style == QString( "TransLight" ) ) )
				painter->setPen( Settings->Colors.SymLinkPenColorAlt );

			else
				painter->setPen( Settings->Colors.SymLinkPenColor );
		}

		else if ( ftype.isExecutable() && ftype.isFile() )
			painter->setPen( Settings->Colors.ExecPenColor );

		else if ( ftype.isHidden() ) {
			if ( ( Settings->General.Style == QString( "LightGray" ) ) or ( Settings->General.Style == QString( "TransLight" ) ) )
				painter->setPen( Settings->Colors.HiddenPenColorAlt );

			else
				painter->setPen( Settings->Colors.HiddenPenColor );
		}

		else if ( !ftype.isReadable() )
			painter->setPen( Settings->Colors.NoReadPenColor );

		else {
			if ( ( Settings->General.Style == QString( "LightGray" ) ) or ( Settings->General.Style == QString( "TransLight" ) ) )
				painter->setPen( Settings->Colors.TextPenColorAlt );

			else
				painter->setPen( Settings->Colors.TextPenColor );
		}

		painter->drawText( textRect, Qt::AlignHCenter, text );

		painter->restore();
	}
};

QSize NBFDItemDelegate::sizeHint( const QStyleOptionViewItem &optItem, const QModelIndex &mIndex ) const {

	Q_UNUSED( optItem );
	Q_UNUSED( mIndex );

	return QSize( 110, 70 );
};
