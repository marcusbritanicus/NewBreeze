/*
	*
	* NBIconDelegate.cpp - ItemDelegate Class for NBIconView class
	*
*/

#include <NBIconDelegate.hpp>
#include <NBFileSystemModel.hpp>

void NBIconDelegate::paint( QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index ) const {

	if ( Settings->General.FolderView == QString( "IconsView" ) )
		paintIcons( painter, option, index );

	else if ( Settings->General.FolderView == QString( "TilesView" ) )
		paintTiles( painter, option, index );

	else
		paintDetails( painter, option, index );
}

void NBIconDelegate::paintIcons( QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index ) const {

	if ( index.column() != 0 )
		QItemDelegate::paint( painter, option, index );

	else {
		const NBFileSystemModel *model = static_cast<const NBFileSystemModel*>( index.model() );
		QFileInfo ftype = model->nodeInfo( index );

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

		QRect iconRect;
		// Original X
		iconRect.setX( optionRect.x() + ( optionRect.width() - iSize.width() ) / 2 );
		// Original Y + Image Top Border + Height to make the image center of the icon rectangle
		iconRect.setY( optionRect.y() + padding + ( iconSize.height() - iSize.height() ) / 2 );
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
		painter->drawRoundedRect( optionRect.adjusted( padding / 2, padding / 2, -padding / 2, -padding / 2 ), 4, 4 );

		// Focus Rectangle
		if ( option.state & QStyle::State_HasFocus ) {
			painter->setBrush( Qt::NoBrush );
			QPoint bl = optionRect.bottomLeft() + QPoint( 7, -padding / 2 );
			QPoint br = optionRect.bottomRight() - QPoint( 7, padding / 2 );
			if ( ( Settings->General.Style == QString( "LightGray" ) ) or ( Settings->General.Style == QString( "TransLight" ) ) ) {
				QLinearGradient hLine( bl, br );
				hLine.setColorAt( 0, Qt::transparent );
				hLine.setColorAt( 0.3, Settings->Colors.FocusPenColorAlt );
				hLine.setColorAt( 0.7, Settings->Colors.FocusPenColorAlt );
				hLine.setColorAt( 1, Qt::transparent );
				painter->setPen( QPen( QBrush( hLine ), 2 ) );
				painter->drawLine( bl, br );
			}

			else {
				QLinearGradient hLine( bl, br );
				hLine.setColorAt( 0, Qt::transparent );
				hLine.setColorAt( 0.3, Settings->Colors.FocusPenColor );
				hLine.setColorAt( 0.7, Settings->Colors.FocusPenColor );
				hLine.setColorAt( 1, Qt::transparent );
				painter->setPen( QPen( QBrush( hLine ), 2 ) );
				painter->drawLine( bl, br );
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

		// Draw Text
		painter->drawText( textRect, Qt::AlignHCenter, text );
		paintIconTextDetails( painter, textRect, index );

		painter->restore();
	}
};

void NBIconDelegate::paintTiles( QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index ) const {

	if ( index.column() != 0 )
		QItemDelegate::paint( painter, option, index );

	else {
		const NBFileSystemModel *model = static_cast<const NBFileSystemModel*>( index.model() );
		QFileInfo ftype = model->nodeInfo( index );

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
		painter->drawRoundedRect( optionRect.adjusted( padding / 2, padding / 2, -padding / 2, -padding / 2 ), 4, 4 );

		// Focus Rectangle
		if ( option.state & QStyle::State_HasFocus ) {
			painter->setBrush( Qt::NoBrush );
			QPoint bl = optionRect.bottomLeft() + QPoint( 7, -padding / 2 );
			QPoint br = optionRect.bottomRight() - QPoint( 7, padding / 2 );
			if ( ( Settings->General.Style == QString( "LightGray" ) ) or ( Settings->General.Style == QString( "TransLight" ) ) ) {
				QLinearGradient hLine( bl, br );
				hLine.setColorAt( 0, Qt::transparent );
				hLine.setColorAt( 0.3, Settings->Colors.FocusPenColorAlt );
				hLine.setColorAt( 0.7, Settings->Colors.FocusPenColorAlt );
				hLine.setColorAt( 1, Qt::transparent );
				painter->setPen( QPen( QBrush( hLine ), 2 ) );
				painter->drawLine( bl, br );
			}

			else {
				QLinearGradient hLine( bl, br );
				hLine.setColorAt( 0, Qt::transparent );
				hLine.setColorAt( 0.3, Settings->Colors.FocusPenColor );
				hLine.setColorAt( 0.7, Settings->Colors.FocusPenColor );
				hLine.setColorAt( 1, Qt::transparent );
				painter->setPen( QPen( QBrush( hLine ), 2 ) );
				painter->drawLine( bl, br );
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

		// Draw Text
		if ( maxLines == 1 )
			painter->drawText( textRect, Qt::AlignVCenter, text );

		else
			painter->drawText( textRect, text );
		paintTileTextDetails( painter, textRect, index );

		painter->restore();
	}
};

void NBIconDelegate::paintDetails( QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index ) const {

	if ( index.column() != 0 )
		QItemDelegate::paint( painter, option, index );

	else {
		const NBFileSystemModel *model = static_cast<const NBFileSystemModel*>( index.model() );
		QFileInfo ftype = model->nodeInfo( index );

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

		if ( iconSize.width() >= 40 ) {
			maxLines = 2;
		}
		else if ( iconSize.width() >= 60 ) {
			maxLines = 3;
		}
		else if ( iconSize.width() >= 90 ) {
			maxLines = 4;
		}
		else if ( iconSize.width() == 128 ) {
			maxLines = 6;
		}
		else {
			maxLines = 1;
		}

		QRect iconRect;
		// Original X + Image Left Border + Width to make the ima
		iconRect.setX( optionRect.x() + padding + ( iconSize.width() - iSize.width() ) / 2 );
		// Original Y + Height to make the image center of the icon rectangle
		iconRect.setY( optionRect.y() + ( optionRect.height() - iSize.height() ) / 2 );
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
		painter->drawRoundedRect( optionRect.adjusted( padding / 2, padding / 2, -padding / 2, -padding / 2 ), 4, 4 );

		// Focus Rectangle
		if ( option.state & QStyle::State_HasFocus ) {
			painter->setBrush( Qt::NoBrush );
			QPoint bl = optionRect.bottomLeft() + QPoint( 7, -padding / 2 );
			QPoint br = optionRect.bottomRight() - QPoint( 7, padding / 2 );
			if ( ( Settings->General.Style == QString( "LightGray" ) ) or ( Settings->General.Style == QString( "TransLight" ) ) ) {
				QLinearGradient hLine( bl, br );
				hLine.setColorAt( 0, Qt::transparent );
				hLine.setColorAt( 0.3, Settings->Colors.FocusPenColorAlt );
				hLine.setColorAt( 0.7, Settings->Colors.FocusPenColorAlt );
				hLine.setColorAt( 1, Qt::transparent );
				painter->setPen( QPen( QBrush( hLine ), 2 ) );
				painter->drawLine( bl, br );
			}

			else {
				QLinearGradient hLine( bl, br );
				hLine.setColorAt( 0, Qt::transparent );
				hLine.setColorAt( 0.3, Settings->Colors.FocusPenColor );
				hLine.setColorAt( 0.7, Settings->Colors.FocusPenColor );
				hLine.setColorAt( 1, Qt::transparent );
				painter->setPen( QPen( QBrush( hLine ), 2 ) );
				painter->drawLine( bl, br );
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

		// Draw Text
		painter->drawText( textRect, Qt::AlignVCenter, text );
		paintExtraDetails( painter, rectList, index );

		painter->restore();
	}
};

void NBIconDelegate::paintIconTextDetails( QPainter *painter, QRect &textRect, const QModelIndex &index ) const {

	// The First Line is already drawn
	// Padding between the lines is 3px
	// TextRect needs to be adjusted: y

	painter->save();
	if ( Settings->General.Style == QString( "TransDark" ) or Settings->General.Style == ( "DullBlack" ) )
		painter->setPen( Settings->Colors.TextPenColor.darker() );

	else
		painter->setPen( Settings->Colors.TextPenColor.darker( 135 ) );

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
	if ( Settings->General.Style == QString( "TransDark" ) or Settings->General.Style == ( "DullBlack" ) )
		painter->setPen( Settings->Colors.TextPenColor.darker() );

	else
		painter->setPen( Settings->Colors.TextPenColor.darker( 135 ) );

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
};
