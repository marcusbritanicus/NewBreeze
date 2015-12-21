/*
	*
	* NBDialog.hpp - NBDialog.cpp header
	*
*/

#pragma once
#ifndef ABOUTNB_HPP
#define ABOUTNB_HPP

#include <Global.hpp>
#include <NBDialog.hpp>
#include <NBButton.hpp>
#include <NBGuiWidgets.hpp>

class AboutNB : public NBDialog {
	Q_OBJECT

	public:
		AboutNB( QWidget *parent = NULL );

		void setDisplayTab( int );

	private:
		int mDisplayTab;
};

#endif
