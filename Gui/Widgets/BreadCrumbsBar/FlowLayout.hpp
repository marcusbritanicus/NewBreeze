/*
	*
	* FlowLayout.hpp - FlowLayout.cpp header
	*
*/

#ifndef FLOWLAYOUT_H
#define FLOWLAYOUT_H

#include <QLayout>
#include <QRect>
#include <QStyle>
#include <QWidgetItem>
#include <QWidget>

class FlowLayout : public QLayout {
	public:
		FlowLayout( QWidget *parent, int margin = -1, int hSpacing = -1, int vSpacing = -1 );
		FlowLayout( int margin = -1, int hSpacing = -1, int vSpacing = -1 );
		~FlowLayout();

		void addItem( QLayoutItem *item );
		void clear();

		int horizontalSpacing() const;
		int verticalSpacing() const;

		Qt::Orientations expandingDirections() const;
		bool hasHeightForWidth() const;
		int heightForWidth( int ) const;

		int count() const;

		QLayoutItem *itemAt( int index ) const;
		QLayoutItem *takeAt( int index );

		QSize minimumSize() const;

		void setGeometry( const QRect &rect );
		QSize sizeHint() const;

	private:
		int doLayout( const QRect &rect, bool testOnly ) const;
		int smartSpacing( QStyle::PixelMetric pm ) const;

		QList<QLayoutItem *> itemList;

		int m_hSpace;
		int m_vSpace;
};

#endif
