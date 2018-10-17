/*
	*
	* NBMessageDialog.cpp - NBMessageDialog class
	*
*/

#include "NBMessageDialog.hpp"

int NBMessageDialog::information( QWidget *parent, QString title, QString text ) {

	QMessageBox *mbox = new QMessageBox( parent );
	mbox->setIconPixmap( QIcon( ":/icons/info.png" ).pixmap( 64 ) );
	mbox->setWindowTitle( title );
	mbox->setText( text );
	mbox->setStandardButtons( QMessageBox::Ok );
	mbox->setEscapeButton( QMessageBox::Ok );
	mbox->setAttribute( Qt::WA_DeleteOnClose );

	return mbox->exec();
};

int NBMessageDialog::question( QWidget *parent, QString title, QString text ) {

	QMessageBox *mbox = new QMessageBox( parent );
	mbox->setIconPixmap( QIcon( ":/icons/question.png" ).pixmap( 64 ) );
	mbox->setWindowTitle( title );
	mbox->setText( text );
	mbox->setStandardButtons( QMessageBox::Yes | QMessageBox::No );
	mbox->setDefaultButton( QMessageBox::No );
	mbox->setEscapeButton( QMessageBox::No );
	mbox->setAttribute( Qt::WA_DeleteOnClose );

	return mbox->exec();
};

int NBMessageDialog::warning( QWidget *parent, QString title, QString text ) {

	QMessageBox *mbox = new QMessageBox( parent );
	mbox->setIconPixmap( QIcon( ":/icons/warning.png" ).pixmap( 64 ) );
	mbox->setWindowTitle( title );
	mbox->setText( text );
	mbox->setStandardButtons( QMessageBox::Ok );
	mbox->setEscapeButton( QMessageBox::Ok );
	mbox->setAttribute( Qt::WA_DeleteOnClose );

	return mbox->exec();
};

int NBMessageDialog::error( QWidget *parent, QString title, QString text ) {

	QMessageBox *mbox = new QMessageBox( parent );
	mbox->setIconPixmap( QIcon( ":/icons/error.png" ).pixmap( 64 ) );
	mbox->setWindowTitle( title );
	mbox->setText( text );
	mbox->setStandardButtons( QMessageBox::Ok );
	mbox->setEscapeButton( QMessageBox::Ok );
	mbox->setAttribute( Qt::WA_DeleteOnClose );

	return mbox->exec();
};

int NBMessageDialog::critical( QWidget *parent, QString title, QString text ) {

	QMessageBox *mbox = new QMessageBox( parent );
	mbox->setIconPixmap( QIcon( ":/icons/stop.png" ).pixmap( 64 ) );
	mbox->setWindowTitle( title );
	mbox->setText( text );
	mbox->setStandardButtons( QMessageBox::Ok );
	mbox->setEscapeButton( QMessageBox::Ok );
	mbox->setAttribute( Qt::WA_DeleteOnClose );

	return mbox->exec();
};
