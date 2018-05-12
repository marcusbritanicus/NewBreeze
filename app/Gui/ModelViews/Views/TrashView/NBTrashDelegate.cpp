/*
	*
	* NBTrashDelegate.cpp - ItemDelegate Class for NBTrashView class
	*
*/

#include "NBTrashDelegate.hpp"
#include "NBTrashModel.hpp"

void NBTrashDelegate::paint( QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index ) const {


	if ( index.column() != 0 )
		QItemDelegate::paint( painter, option, index );

	else {
		const NBTrashModel *model = static_cast<const NBTrashModel*>( index.model() );

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
			* | |  icon  | This is the text        Original Path                             Size        Deletion Date   |
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
		textRect.setSize( QSize( 200, optionRect.height() ) - QSize( iconSize.width(), 0 ) - 3 * QSize( padding, 0 ) );

		// Set elided text
		QString textE = fm.elidedText( text, Qt::ElideRight, textRect.width() );
		QList<QRect> rectList;

		// First detail
		rectList << QRect( QPoint( textRect.x() + 200, textRect.y() ), optionRect.size() - QSize( iconSize.width(), 0 ) - 3 * QSize( padding, 0 ) - QSize( 500, 0 ) );
		rectList << QRect( rectList.at( 0 ).x() + rectList.at( 0 ).width(), textRect.y(), 100, textRect.height() );
		rectList << QRect( rectList.at( 1 ).x() + 100, textRect.y(), 200, textRect.height() );

		QRect iconRect;
		// Original X + Image Left Border + Width to make the ima
		iconRect.setX( optionRect.x() + padding + ( iconSize.width() - iSize.width() ) / 2 );
		// Original Y + Height to make the image center of the icon rectangle
		iconRect.setY( optionRect.y() + ( optionRect.height() - iSize.height() ) / 2 );
		// Icon Size
		iconRect.setSize( iSize );

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
		paintExtraDetails( painter, rectList, index );

		painter->restore();
	}
};

void NBTrashDelegate::paintExtraDetails( QPainter *painter, QList<QRect> &textRectList, const QModelIndex &index ) const {

	// The First Line is already drawn
	// Padding between the lines is 3px
	// TextRect needs to be adjusted: y

	QRect textRect;

	textRect = textRectList.at( 0 );
	QString detail = index.data( Qt::UserRole + 1 ).toString();
	painter->drawText( textRect, Qt::AlignLeft | Qt::AlignVCenter | Qt::TextWordWrap, detail );

	textRect = textRectList.at( 1 );
	detail = index.data( Qt::UserRole + 2 ).toString();
	painter->drawText( textRect, Qt::AlignVCenter | Qt::TextWordWrap, detail );

	textRect = textRectList.at( 2 );
	detail = index.data( Qt::UserRole + 3 ).toString();
	painter->drawText( textRect, Qt::AlignVCenter | Qt::TextWordWrap, detail );
};
