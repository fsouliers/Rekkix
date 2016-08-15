/*!
 * \file FactoryReportBaseString.h
 * \brief Definition of the class FactoryReportBaseString
 * \date 2016-08-07
 * \author f.souliers
 */

#ifndef FACTORYREPORTBASESTRING_H_
#define FACTORYREPORTBASESTRING_H_

#include <QString>

/*!
 * \class FactoryReportBaseString
 * \brief Simple and small factory used to retrieve the information of the template files stored in the resource system
 *
 * When Rekkix builds the requirement matrix report, several formats are available and can be set in the
 * configuration file. Each kind of report (csv or html) is composed of several templates stored through
 * ./resources/rekkix.qrc.
 * As a consequence, this factory is here to facilitate the getting of the file content of those templates.
 */
class FactoryReportBaseString
{
public:
	/*!
	 * \brief Constructor of the class, does nothing special
	 */
	FactoryReportBaseString();

	/*!
	 * \brief Destructor of the class, does nothing special
	 */
	virtual ~FactoryReportBaseString();

	/*!
	 * \brief Method use to retrieve the content of a given template for a given file format (writer)
	 * \param[in] p_templateName  Unique part of the file name of the report template as described in the
	 *                            resources hierarchy, eg "base" or "errors_base" or " fileDetails_docCoverage"
	 * \param[in] p_writer        Writer for which the template is expected : html or csv
	 * \param[in] p_delimiter     In case of csv file, it is the field delimiter ... totally useless for html
	 * \return
	 * The string containing all the lines read from the template so it is possible to replace reports
	 * keywords by their value.
	 */
	QString getBaseString(const char * p_templateName,
	                      const QString& p_writer,
	                      const QString& p_delimiter) const;

	/*!
	 * \brief Exactly the same approach as getBaseString, but the CSS file is unique and only used in html reporting
	 * \return
	 * The string containing all the lines of the HtmlReportStyleSheet.css file
	 */
	QString getCSString() const;
};

#endif /* FACTORYREPORTBASESTRING_H_ */
