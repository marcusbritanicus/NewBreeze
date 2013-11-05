/*
	*
	* NBTreeDelegate.cpp - ItemDelegate Class for NBTreeView class
	*
*/

#include <NBTreeDelegate.hpp>
#include <NBFileSystemModel.hpp>
#include <NBTools.hpp>

static QStringList ListView = QStringList() << "SmallListView" << "NormalListView" << "TilesView";
static QStringList IconView = QStringList() << "SmallIconsView" << "NormalIconsView" << "LargeIconsView" << "HugeIconsView";
static QStringList TinyView = QStringList() << "SmallListView" << "SmallIconsView";
static QStringList HugeView = QStringList() << "LargeIconsView" << "HugeIconsView";

void NBTreeDelegate::paint( QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index ) const {

	if ( index.column() != 0 )
		QItemDelegate::paint( painter, option, index );

	else {
		const NBFileSystemModel *model = static_cast<const NBFileSystemModel*>( index.model() );
		QFileInfo ftype = model->nodeInfo( index );

		// Font Mentrics for elided text;
		QFontMetrics fm( qApp->font() );

		// Get icon size
		QSize iconSize;
		if ( Settings->General.FolderView == QString( "SmallListView" ) )
			iconSize = QSize( 24, 24 );

		else if ( Settings->General.FolderView == QString( "NormalListView" ) )
			iconSize = QSize( 48, 48 );

		else if ( Settings->General.FolderView == QString( "TilesView" ) )
			iconSize = QSize( 48, 48 );

		else if ( Settings->General.FolderView == QString( "SmallIconsView" ) )
			iconSize = QSize( 24, 24 );

		else if ( Settings->General.FolderView == QString( "NormalIconsView" ) )
			iconSize = QSize( 48, 48 );

		else if ( Settings->General.FolderView == QString( "LargeIconsView" ) )
			iconSize = QSize( 64, 64 );

		else if ( Settings->General.FolderView == QString( "HugeIconsView" ) )
			iconSize = QSize( 128, 128 );

		QString text = model->data( index, Qt::DisplayRole ).toString();
		QPixmap icon = model->data( index, Qt::DecorationRole ).value<QIcon>().pixmap( iconSize );

		QSize iSize = icon.size();

		QRect textRect;
		if ( ListView.contains( Settings->General.FolderView ) ) {
			// Original X + Image Left Border + Image Width + Image-Text Gap
			textRect.setX( option.rect.x() + 3 + iconSize.width() + 5 );
			// Vertical Centering, so don't bother
			textRect.setY( option.rect.y() );
			// Original Width - Image Left Border - Image Width - Image Text Gap -Text Right Border
			textRect.setSize( option.rect.size() - QSize( 3, 0 ) - QSize( iconSize.width(), 0 ) - QSize( 5, 0 ) - QSize( 3, 0 ) );

			// Set elided text
			text = fm.elidedText( text, Qt::ElideRight, textRect.width() );

		}

		else {
			// Horizontal Centering, so don't bother
			textRect.setX( option.rect.x() );
			// Original Y + Image Top Border + Image Height
			textRect.setY( option.rect.y() + 2 + iconSize.height() + 3 );
			// Text Bottom Border
			textRect.setSize( option.rect.size() - QSize( 0, 2 ) );

			// Set elided text
			text = fm.elidedText( text, Qt::ElideRight, textRect.width() );
		}

		QRect iconRect;
		if ( ListView.contains( Settings->General.FolderView ) ) {
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

				// Original X + Width to make the image center of the image rectangle
				iconRect.setX( option.rect.x() + ( iconSize.width() - iSize.width() ) / 2 + 2 );
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
				iconRect.setX( option.rect.x() + ( iconSize.width() - iSize.width() ) / 2 + 2 );
				// Original Y + Height to make the image center of the image rectangle
				iconRect.setY( option.rect.y() + ( option.rect.height() - iSize.height() ) / 2 );
				// Icon Size
				iconRect.setSize( iSize );
			}
		}

		else {
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
			iconRect.setX( option.rect.x() + ( option.rect.width() - iconSize.width() ) / 2 );
			// Original Y + Image Top Border + Height to make the image center of the icon rectangle
			iconRect.setY( option.rect.y() + 3 + ( iconSize.height() - iSize.height() ) / 2 );
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
		if ( TinyView.contains( Settings->General.FolderView ) )
			painter->drawRoundedRect( option.rect, 5, 5 );

		else if ( HugeView.contains( Settings->General.FolderView ) )
			painter->drawRoundedRect( option.rect, 15, 15 );

		else
			painter->drawRoundedRect( option.rect, 7, 7 );

		// Focus Rectangle
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

		// In NormalListView paint an arrow to show its a directory
		if ( ( ftype.isDir() ) and ( Settings->General.FolderView == QString( "NormalListView" ) ) ) {
			QPixmap arrowPix = QPixmap( ":/icons/arrow-right.png" ).scaled( 24, 24, Qt::KeepAspectRatio, Qt::SmoothTransformation );
			QRect arrowRect = QRect( option.rect.width() - 32, option.rect.y() + 12, 24, 24 );
			painter->drawPixmap( arrowRect, arrowPix );
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

		if ( ListView.contains( Settings->General.FolderView ) )
			painter->drawText( textRect, Qt::AlignLeft | Qt::AlignVCenter, text );

		else
			painter->drawText( textRect, Qt::AlignHCenter, text );

		painter->restore();
	}
};

QSize NBTreeDelegate::sizeHint( const QStyleOptionViewItem &optItem, const QModelIndex &mIndex ) const {

	Q_UNUSED( optItem );
	Q_UNUSED( mIndex );

	if ( Settings->General.FolderView == QString( "SmallListView" ) )
		return QSize( 248, 26 );

	else if ( Settings->General.FolderView == QString( "NormalListView" ) )
		return QSize( 248, 52 );

	else if ( Settings->General.FolderView == QString( "TilesView" ) )
		return QSize( 248, 56 );

	else if ( Settings->General.FolderView == QString( "SmallIconsView" ) )
		return QSize( 74, 46 );

	else if ( Settings->General.FolderView == QString( "NormalIconsView" ) )
		return QSize( 110, 70 );

	else if ( Settings->General.FolderView == QString( "LargeIconsView" ) )
		return QSize( 150, 90 );

	else if ( Settings->General.FolderView == QString( "HugeIconsView" ) )
		return QSize( 290, 170 );

	return QSize( 120, 90 );
};
