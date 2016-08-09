/*
 * AnalysisError.cpp
 *
 *  Created on: 1 août 2016
 *      Author: f.souliers
 */

#include <QObject>

#include "AnalysisError.h"

AnalysisError::AnalysisError(const severity_t& severity, const category_t& category, const QString& location, const QString& description)
	: __severity(severity)
	, __category(category)
	, __location(location)
	, __description(description)
{

}

AnalysisError::~AnalysisError()
{
}

QString
AnalysisError::getSeverityString() const
{
	QString r ;

	switch (__severity)
	{
		case WARNING :
			r = QObject::trUtf8("Alerte") ;
			break;

		case ERROR :
			r = QObject::trUtf8("Erreur") ;
			break ;
	}

	return(r) ;
}

QString
AnalysisError::getCategoryString() const
{
	QString r ;

	switch (__category)
	{
		case PARSING :
			r = QObject::trUtf8("Analyse") ;
			break;

		case CONSISTENCY :
			r = QObject::trUtf8("Cohérence") ;
			break ;

		case CONTENT :
			r = QObject::trUtf8("Contenu") ;
			break ;
	}

	return(r) ;
}
