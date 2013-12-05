/*
	*
	* NBIconDelegate.cpp - ItemDelegate Class for NBIconView class
	*
*/

#include <NBIconDelegate.hpp>
#include <NBFileSystemModel.hpp>

static QStringList ListView = QStringList() << "SmallListView" << "NormalListView" << "TilesView";
static QStringList IconView = QStringList() << "SmallIconsView" << "NormalIconsView" << "LargeIconsView" << "HugeIconsView";
static QStringList TinyView = QStringList() << "SmallListView" << "SmallIconsView";
static QStringList HugeView = QStringList() << "LargeIconsView" << "HugeIconsView";

void NBIconDelegate::paint( QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index ) const {

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

		painter->restore();
	}
};

QSize NBIconDelegate::sizeHint( const QStyleOptionViewItem &optItem, const QModelIndex &mIndex ) const {

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
