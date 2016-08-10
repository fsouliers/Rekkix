/*!
 * \file AnalysisError.h
 * \brief Definition of the class AnalysisError
 * \date 2016-08-01
 * \author f.souliers
 */

#ifndef ANALYSISERROR_H_
#define ANALYSISERROR_H_

#include <QString>

/*!
 * \class AnalysisError
 * \brief Contains all the information of an error detected during analysis
 *
 * The requirement analysis is performed just after the files are parsed (so every defined requirement is
 * known). During this phase, many errors can be detected (requirement used by another one and defined
 * nowhere, requirement defined several times, etc ...).
 */
class AnalysisError
{
public:
	/*!
	 * \typedef severity_t
	 * \brief Severity of an error during analysis.
	 */
	typedef enum e_severity
	{
		WARNING,  //!< Something went wrong, but the traceability matrix may still provide usable information
		ERROR  //!< Something went so wrong that the traceability matrix is probably meaningless
	} severity_t;

	/*!
	 * \typedef category_t
	 * \brief Category of an error during analysis. Just to help fixing the problem.
	 */
	typedef enum e_category
	{
		PARSING,  //!< The error occurred during file parsing (but it is not a file parsing error)
		CONSISTENCY,  //!< The error occurred while checking requirements consistency
		CONTENT  //!< The content of a requirement is incorrect
	} category_t;

	/*!
	 * \brief Constructor of an AnalysisError
	 * \param[in] severity     The severity to be set at initialization, see AnalysisError::severity_t
	 * \param[in] category     The category of error to be set at initialization, see AnalysisError::category_t
	 * \param[in] location     The location where the error has been detected, by default it's nowhere in space.
	 *                         Such location is the ID of the file where the error has been detected, see
	 *                         ModelConfiguration::REQFILE_ATTR_ID
	 * \param[in] description  The description of the error to be displayed on screen or printed in the reports
	 *
	 * Once the error has been created, there is no way to modify its attributes : it has to be instantiated with
	 * the whole required information at one time.
	 *
	 * The location of an analysis error can be an empty string. For instance when checking the whole requirement
	 * matrix consistency, if an error is found ... there is no way to locate it with a file ID.
	 */
	AnalysisError(const severity_t& severity = WARNING,
	              const category_t& category = CONTENT,
	              const QString& location = "",
	              const QString& description = "");

	/*!
	 * \brief Destructor of an AnalysisError
	 */
	virtual ~AnalysisError();

	/*!
	 * \brief getter for severity attribute
	 * \return The method returns the severity level it has been created with
	 */
	severity_t getSeverity() const
	{
		return (__severity);
	}

	/*!
	 * \brief getter for severity attribute as a string
	 * \return The method returns a QString representing the severity level the AnalysisError has been created with
	 */
	QString getSeverityString() const;

	/*!
	 * \brief getter for category attribute
	 * \return The method returns the category it has been created with
	 */
	category_t getCategory() const
	{
		return (__category);
	}

	/*!
	 * \brief getter for category attribute as a string
	 * \return The method returns a QString representing the category the AnalysisError has been created with
	 */
	QString getCategoryString() const;

	/*!
	 * \brief getter for location attribute as a string
	 * \return The method returns a QString representing the file ID in which the error has been detected or
	 *         an empty string if the location hasn't been set.
	 */
	QString getLocation() const
	{
		return (__location);
	}

	/*!
	 * \brief getter for description of the error
	 * \return The method returns a QString representing a description of the error. This description must be
	 *         created as a translatable string with QObject::trUtf8.
	 */
	QString getDescription() const
	{
		return (__description);
	}

private:
	severity_t __severity;
	category_t __category;
	QString __location;
	QString __description;
};

/*!
 * \typedef AnalysisErrorPtr
 * \brief Just to make the code easier to type when a pointer to AnalysisError is required
 */
typedef AnalysisError* AnalysisErrorPtr;

/*!
 * \typedef AnalysisErrorRef
 * \brief Just to make the code easier to type when a reference of an AnalysisError is required
 */
typedef AnalysisError& AnalysisErrorRef;

#endif /* ANALYSISERROR_H_ */
