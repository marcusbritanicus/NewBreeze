/*
 *  This file is part of Quartica.
 *
 *  Copyright( c ) 2008 Matteo Bertozzi <theo.bertozzi@gmail.com>
 *
 *  Quartica is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 * ( at your option ) any later version.
 *
 *  Quartica is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with Quartica.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <QMouseEvent>

#include <NBSelectableWidget.hpp>

class NBSelectableWidget::Private {

	public:
		bool isSelected;
};

NBSelectableWidget::NBSelectableWidget( QWidget *parent ) : QWidget( parent ), d( new NBSelectableWidget::Private ) {

	d->isSelected = false;
}

NBSelectableWidget::~NBSelectableWidget( ) {

	delete d;
	d = NULL;
}

bool NBSelectableWidget::isSelected( void ) const {

	return( d->isSelected );
}

void NBSelectableWidget::select( void ) {

	select( true );
}

void NBSelectableWidget::select( bool isSelected ) {

	if ( d->isSelected == isSelected ) return;

	d->isSelected = isSelected;

	if ( isSelected ) {

		emit selected( );
		emit selected( this );
	}

	update( );
}

void NBSelectableWidget::unselect( void ) {

	select( false );
}

void NBSelectableWidget::mouseReleaseEvent( QMouseEvent *event ) {

	QWidget::mouseReleaseEvent( event );

	// Left Click, Select/Unselect Item
	if ( event->button( ) == Qt::LeftButton )
		select( !d->isSelected );
}
