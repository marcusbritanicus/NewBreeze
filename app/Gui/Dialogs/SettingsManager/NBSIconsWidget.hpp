/*
	*
	* NBSGeneralWidget.hpp - NBSGeneralWidget.cpp header
	*
*/

#pragma once

#include "Global.hpp"
#include "NBButton.hpp"
#include "NBIconManager.hpp"

class NBIconThemeChooserWidget;
class NBIconThemeViewerWidget;

class NBIconThemeWidget : public QWidget {
	Q_OBJECT

	public:
		NBIconThemeWidget( QWidget *parent );

	private:
		void createGUI();
		void updateListView();

		NBIconThemeChooserWidget *iconThemesWidget;
		NBIconThemeViewerWidget *folderViewWidget;
};

class NBIconThemeChooserWidget : public QWidget {
	Q_OBJECT

	public:
		NBIconThemeChooserWidget( QWidget * );
		void loadThemes();
		QStringList themesList;

	private:
		int current;
		NBButton *prevBtn, *nextBtn;
		QComboBox *themeCB;

	private slots:
		void switchTheme( int );
		void nextTheme();
		void previousTheme();

	signals:
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

	private slots:
		void setupModel();
};

class NBIconThemeViewerWidget: public QListView {
	Q_OBJECT

	public:
		NBIconThemeViewerWidget( QWidget * );

	Q_SIGNALS:
		void setupModel();
};
