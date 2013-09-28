/*
	*
	* NBSPDeviceView.hpp - NBSPDeviceView.cpp Header
	*
*/

#ifndef NBSPDEVICEVIEW_HPP
#define NBSPDEVICEVIEW_HPP

#include <Global.hpp>
#include <NBDeviceModel.hpp>
#include <NBSPDeviceDelegate.hpp>

class NBSPDeviceView : public QListView {
	Q_OBJECT

	public :
		NBSPDeviceView();
		QSize sizeHint() const;
		QSize minimumSizeHint() const;

		int maxItemWidth() const;

	private :
		NBDeviceModel *dataModel;
		NBSPDeviceDelegate *dataDelegate;

		QFileSystemWatcher *settingsWatcher;
		int widthHint;

	private slots:
		void handleMouseClick( const QModelIndex & );
		void changeWidthHint();

	protected:
		void dragEnterEvent( QDragEnterEvent* );
		void dragMoveEvent( QDragMoveEvent* );
		void dropEvent( QDropEvent* );

	signals :
		void mountDrive( QString );
};

#endif
