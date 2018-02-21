/*
	*
	* NBSPluginsWidget.hpp - Plugins Manager Widget for Settings Manager
	*
*/

#pragma once

#include "Global.hpp"
#include "NBButtons.hpp"
#include "NBPluginManager.hpp"
#include "NBFileDialog.hpp"

class NBSPluginsWidget : public QWidget {
	Q_OBJECT

	public:
		NBSPluginsWidget( QWidget *parent );

	private:
		void createGUI();

		QListWidget *pPathsLW;
		NBSegmentButton *pathsBtn;

	private Q_SLOTS:
		void buttonClick( int );

		void addPath();
		void removePath();

		void updateSettings();
};
