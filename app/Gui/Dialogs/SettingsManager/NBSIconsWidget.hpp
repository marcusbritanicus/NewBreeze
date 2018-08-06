/*
	*
	* NBSGeneralWidget.hpp - NBSGeneralWidget.cpp header
	*
*/

#pragma once

#include "Global.hpp"
#include "NBButton.hpp"
#include "NBIconManager.hpp"

class NBIconThemeView;

class NBIconThemeWidget : public QWidget {
	Q_OBJECT

	public:
		NBIconThemeWidget( QWidget *parent );

	private:
		void createGUI();

		NBIconThemeView *iconViewWidget;

	private slots:
		void switchTheme( const QModelIndex& );

	Q_SIGNALS:
		void reloadIcons();
};

class NBIconThemeModel: public QAbstractListModel {
	Q_OBJECT

	public:
		NBIconThemeModel( QObject * );

		int rowCount( const QModelIndex &parent = QModelIndex() ) const;
		QVariant data( const QModelIndex &index, int role ) const;

		inline Qt::ItemFlags flags( const QModelIndex ) const {

			return Qt::ItemIsSelectable | Qt::ItemIsEnabled | Qt::ItemIsUserCheckable;
		};

	private:
		QStringList themesList;
		QMap<QString, QIcon> iconsMap;

	private Q_SLOTS:
		void setupModel();

	Q_SIGNALS:
};

class NBIconThemeView: public QListView {
	Q_OBJECT

	public:
		NBIconThemeView( QWidget * );

		void select( QString );

	private:
		NBIconThemeModel *model;
};
