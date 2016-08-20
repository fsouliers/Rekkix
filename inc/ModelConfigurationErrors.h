/*!
 * \file ModelConfigurationErrors.h
 * \brief Definition of the class ModelConfigurationErrors
 * \date 2016-08-01
 * \author f.souliers
 */

#ifndef MODELCONFIGURATIONERRORS_H_
#define MODELCONFIGURATIONERRORS_H_

#include <QAbstractItemModel>

/*!
 * \class ModelConfigurationErrors
 * \brief Model (see Qt Framework and MVC) used to display the list of errors found in the configuration file
 * \see ModelConfiguration
 */
class ModelConfigurationErrors : public QAbstractItemModel
{
public:

	/*!
	 * \typedef severity_t
	 * \brief Severity of an error during configuration reading
	 *
	 * - WARNING: something is not normal, but some operations can however be performed
	 * - ERROR: something is so wrong that it is impossible to start the analysis of the configured files
	 * - CRITICAL: end of the world
	 * \see ModelConfiguration::__hasAReqFileConsistecyError and ModelConfiguration::__hasAnOutputFileConsistecyError
	 */
	typedef enum e_severity
	{
		WARNING, ERROR, CRITICAL
	} severity_t;

	/*!
	 * \typedef category_t
	 * \brief Category of an error during configuration reading
	 *
	 * - PARSING: error/warning found during the parsing of the configuration file
	 * - FILESYSTEM: error/warning related to the filesystem (eg file access)
	 * - CONTENT: error/warning related to the content (the information is present, readable ... but inconsistent)
	 */
	typedef enum e_category
	{
		PARSING, FILESYSTEM, CONTENT
	} category_t;

	/*!
	 * \typedef error_t
	 * \brief Data structure of an error
	 */
	typedef struct s_error
	{
		severity_t severity;
		category_t category;
		QString description;
	} error_t;

	/*!
	 * \brief Constructor of the class; does nothing special.
	 * \param parent  Transmitted to QAbstractItemModel but not used
	 */
	ModelConfigurationErrors(QObject *parent = Q_NULLPTR);

	/*!
	 * \brief Destructor of the class; does nothing
	 */
	virtual ~ModelConfigurationErrors();

	/*!
	 * \brief Column counter for the model
	 * \see Qt documentation about implementing subclasses of QAbstractItemModel
	 * \param[in] parent  Required by the framework, but not used here
	 * \return
	 * The number of columns the model has
	 */
	int columnCount(const QModelIndex &parent = QModelIndex()) const;

	/*!
	 * \brief Row counter for the model
	 * \see Qt documentation about implementing subclasses of QAbstractItemModel
	 * \param[in] parent  Required by the framework, but not used here
	 * \return
	 * The number of rows the model has
	 */
	int rowCount(const QModelIndex &parent = QModelIndex()) const;

	/*!
	 * \brief Getter for data in the model
	 * \param[in] index  Model index (that is row & column) for which the data is required
	 * \param[in] role   Role for which the data is required (see Qt documentation)
	 * \see ModelConfiguration.cpp for the details of the columns
	 * \return
	 * The required data
	 */
	QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;

	/*!
	 * \brief Getter for header data in the model
	 *
	 * The approach is totally the same as for the data method, but here only for columns & row headers
	 * \param[in] index       Model index (that is row & column) for which the header data is required
	 * \param[in] orientation Orientation for which the data is required
	 * \param[in] role        Role for which the data is required (see Qt documentation)
	 * \see ModelConfigurationErros.cpp for the details of the columns
	 * \return
	 * The required data
	 */
	QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;

	/*!
	 * \brief Index builder for a given set of row, column & parent
	 * \param[in] row    row number that must be used to create the index
	 * \param[in] column column number that must be used to create the index
	 * \param[in] parent not used in this implementation
	 * \return
	 * A new created index object
	 */
	QModelIndex index(int row, int column, const QModelIndex &parent = QModelIndex()) const;

	/*!
	 * \brief Parent getter for a given index
	 * \param[in] index  Not used in this implementation
	 * \return
	 * An empty index
	 */
	QModelIndex parent(const QModelIndex &index) const;

	/*!
	 * \brief Empty the list of errors (typically, to start loading a new configuration file)
	 */
	void clear()
	{
		__errors.clear();
	}

	/*!
	 * \brief Ask for a refresh of the model so the UI is updated
	 *
	 * this method emits the dataChanged and layoutChanged signals so the Qt framework updates the graphical
	 * view associated to this model.
	 */
	void refresh();

	/*!
	 * \brief Append one single error to the list of errors
	 * \param[in] e  the error object to be added
	 */
	void addError(const error_t& e)
	{
		__errors.append(e);
	}

	/*!
	 * \brief Append several errors to the list of errors
	 * \param[in] ve  the vector of error objects to be added
	 */
	void addErrors(const QVector<error_t>& ve)
	{
		__errors.append(ve);
	}

	/*!
	 * \brief Getter used to know if there is at least one real problem
	 *
	 * This method is used to prevent user from launching analysis if there are some configuration errors
	 * \return
	 * - true if at least one error with severity ERROR or CRITICAL.
	 * - else false
	 */
	bool hasSignificantError();

	/*!
	 * \brief Getter used to know if there is at least one warning
	 *
	 * This method is used to display the list of warnings in batch mode
	 * \return
	 * - true if at least one error with severity WARNING.
	 * - else false
	 */
	bool hasWarning();

	/*!
	 * \brief used in batch mode to display configuration errors in the console (shell)
	 * \param[out]  str  the string that must be completed with the errors (separated by new lines).
	 *                   The initial string is not reseted by this method (only new characters added)
	 */
	void appendErrorString(QString& str) ;

	/*!
	 * \brief used in batch mode to display configuration warnings in the console (shell)
	 * \param[out]  str  the string that must be completed with the warnings (separated by new lines)
	 *                   The initial string is not reseted by this method (only new characters added)
	 */
	void appendWarningString(QString& str) ;

private:
	/*!
	 * Vector of errors found in the configuration file
	 */
	QVector<error_t> __errors;

	/*!
	 * \brief Converts Severity enumerator into translated QString
	 * \param[in] s  severity enumerator
	 * \return The translated string corresponding to the enumerator
	 */
	QString __severityToString(const severity_t& s) const;

	/*!
	 * \brief Converts Category enumerator into translated QString
	 * \param[in] c category enumerator
	 * \return The translated string corresponding to the enumerator
	 */
	QString __categoryToString(const category_t& c) const;
};

/*!
 * \typedef ModelConfigurationErrorsPtr
 * \brief Just to make the code easier to type when a pointer to ModelConfigurationErrors is required
 */
typedef ModelConfigurationErrors* ModelConfigurationErrorsPtr;

/*!
 * \typedef ModelConfigurationErrorsRef
 * \brief Just to make the code easier to type when a reference of an ModelConfigurationErrors is required
 */
typedef ModelConfigurationErrors& ModelConfigurationErrorsRef;

#endif /* MODELCONFIGURATIONERRORS_H_ */
