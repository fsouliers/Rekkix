/*
 * FactoryReportBaseString.h
 *
 *  Created on: 7 août 2016
 *      Author: f.souliers
 */

#ifndef FACTORYREPORTBASESTRING_H_
#define FACTORYREPORTBASESTRING_H_

#include <QString>

class FactoryReportBaseString
{
public:
	FactoryReportBaseString();
	virtual ~FactoryReportBaseString();

	/*
	 * Returns the string contained in the template report files (see rekkix.qrc)
	 * - p_templateName : unique part of the file name of the report template as described in the resources hierarchy,
	 *                    eg "base" or "errors_base" or " fileDetails_docCoverage"
	 * - p_writer : writer for which the template is expected : html or csv
	 * - p_delimiter : in case of csv file, it is the field delimiter
	 */
	QString getBaseString(const char * p_templateName, const QString& p_writer, const QString& p_delimiter) const ;

	/*
	 * Same, but CSS file is unique
	 */
	QString getCSString() const ;
};

#endif /* FACTORYREPORTBASESTRING_H_ */
