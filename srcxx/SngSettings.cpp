/*!
 * \file SngSettings.cpp
 * \brief Implementation of the class SngSettings
 * \date 2016-09-02
 * \author f.souliers
 */

#include "SngSettings.h"

// Only for Eclipse colorisation --> APP_VERSION is defined in rekkix.pro
#ifndef APP_VERSION
#define APP_VERSION "UNEXPECTED APP VERSION"
#endif

const QString SngSettings::section_general = "General" ;

const QString SngSettings::key_version = "version" ;
const QString SngSettings::key_parsingThreadsNum = "nbParsingThreads" ;

const QString SngSettings::value_version = APP_VERSION ;
const int SngSettings::value_nbParsingThreadsDefault = 10 ;


SngSettings::SngSettings()
	: __settings(QSettings::UserScope, "Rekkix", "Rekkix")
{
	__settings.beginGroup(section_general) ;
	QString storedVersionNumber = __settings.value(key_version).toString() ;
	__settings.endGroup() ;

	if (storedVersionNumber.isEmpty())
	{
		// this is the first time the application is launched, store default values
		__settings.beginGroup(section_general) ;
		__settings.setValue(key_version, value_version) ;
		__settings.setValue(key_parsingThreadsNum, value_nbParsingThreadsDefault) ;

		__settings.endGroup() ;
	}
	else
	{
		// TODO Verify this value versus the current application version
		// ... some configuration updates may be necessary ...
	}

}

SngSettings::~SngSettings()
{

}

