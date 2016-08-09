/*
 * AnalysisError.h
 *
 *  Created on: 1 août 2016
 *      Author: f.souliers
 */

#ifndef ANALYSISERROR_H_
#define ANALYSISERROR_H_

#include <QString>

class AnalysisError
{
public:
	/*!
	 * Severity of an error during analysis
	 * - WARNING : something is going wrong, but the analysis is still possible
	 * - ERROR : the analysis cannot be considered as complete and consistent, so no report can be generated if there
	 * is at least one ERROR
	 */
	typedef enum e_severity {WARNING, ERROR} severity_t ;

	/*!
	 * Category of an error during analysis
	 * - PARSING : Error occurred during file parsing
	 * - CONSISTENCY : the error occurred while checking requirements consistency
	 * - CONTENT : the content of a requirement is incorrect
	 */
	typedef enum e_category {PARSING, CONSISTENCY, CONTENT} category_t ;

	AnalysisError(const severity_t& severity = WARNING, const category_t& category = CONTENT, const QString& location = "", const QString& description = "");
	virtual ~AnalysisError();

	severity_t getSeverity() const {return(__severity) ;}
	QString getSeverityString() const ;

	category_t getCategory() const {return(__category) ;}
	QString getCategoryString() const ;

	QString getLocation() const {return(__location) ;}

	QString getDescription() const {return(__description) ;}

private :
	severity_t __severity ;
	category_t __category ;
	QString __location ;
	QString __description ;
};

typedef AnalysisError* AnalysisErrorPtr ;

typedef AnalysisError& AnalysisErrorRef ;

#endif /* ANALYSISERROR_H_ */
