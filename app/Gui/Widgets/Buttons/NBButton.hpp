/*
	*
	* NBButton.hpp - The NBButton Class, a simple replacement for QPushButton and QToolButton
	*
*/

#pragma once

#include <Global.hpp>
#include <NBGuiWidgets.hpp>

class NBButton : public QPushButton {
	Q_OBJECT

	Q_ENUMS(Segment)

	public:
		enum Segment {
			FirstSegment,
			MiddleSegment,
			LastSegment,
			SingleButton
		};

		/* A button */
		NBButton( QWidget *parent = NULL );

		/* ToolButton */
		NBButton( QIcon, QWidget *parent = NULL  );

		/* PushButton */
		NBButton( QString, QWidget *parent = NULL  );

		/* PushButton */
		NBButton( QIcon, QString, QWidget *parent = NULL  );

		/* Get/Set icon */
		QIcon icon();
		void setIcon( QIcon );

		/* Get/Set text */
		QString text();
		void setText( QString );

		/* Get/Set icon size */
		QSize iconSize();
		void setIconSize( QSize );

		/* Button Size */
		QSize size();

		/* Size hint */
		QSize sizeHint() const;

		/* ObjectName */
		void setObjectName( QString );

		/* Segment in a SegmentButton */
		int segment();
		void setSegment( NBButton::Segment );

		/* Highly customized stylesheet */
		void setButtonStyle();

	private:
		void initPrivateVariables();

		/* Icon */
		QIcon mIcon;

		/* Text */
		QString mText;

		/* Icon Size */
		QSize mIconSize;

		/* Button Size */
		int mHeight, mWidth;

		/* Button Segment */
		NBButton::Segment mSegment;

		/* Button StyleSheet info */
		QColor mButtonColor;
		QStringList radius;
		QString leftBorderSize;
};
