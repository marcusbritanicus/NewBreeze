/*
	*
	* NBFunction.hpp - NBFunction.cpp header
	*
*/

#include "NBFunctions.hpp"

QStringList getTerminal() {

	QSettings sett( "NewBreeze", "NewBreeze" );

	QString defTerm = sett.value( "Terminals/Default", "Inbuilt" ).toString();

	/* If we have an terminal plugin, and it is the default */
	if ( defTerm == "Inbuilt" )
		return QStringList() << defTerm;

	/* We have set something else as the default terminal */
	if ( sett.value( "Terminals/" + defTerm ).toStringList().count() )
		return sett.value( "Terminals/" + defTerm ).toStringList();

	return QStringList();
};
