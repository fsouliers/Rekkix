/*!
 * \file ModelSngAnalysisErrors.h
 * \brief Definition of the class ModelSngAnalysisErrors
 * \date 2016-08-01
 * \author f.souliers
 */

#ifndef MODELSNGANALYSISERRORS_H_
#define MODELSNGANALYSISERRORS_H_

#include <QAbstractItemModel>

#include "AnalysisError.h"

/*!
 * \class ModelSngAnalysisErrors
 * \brief Model (see Qt Framework and MVC) used to display the list of errors found during analysis
 *
 * This class is also a singleton as all the errors must be centralized and displayed at the same time
 */
class ModelSngAnalysisErrors : public QAbstractItemModel
{
Q_OBJECT

public:

	/*!
	 * \brief Instance getter for the singleton object
	 * \return The reference of the instance
	 */
	static ModelSngAnalysisErrors& instance()
	{
		static ModelSngAnalysisErrors r;
		return (r);
	}

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
	 * \see ModelSngAnalysisErrors.cpp for the details of the columns
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
	 * \see ModelSngAnalysisErrors.cpp for the details of the columns
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
	 * \brief Append one single error to the list of errors
	 * \param[in] e  the error object to be added
	 */
	void addError(const AnalysisError& e);

	/*!
	 * \brief Append several errors to the list of errors
	 * \param[in] ve  the vector of error objects to be added
	 */
	void addErrors(const QVector<AnalysisError>& ve);

	/*!
	 * \brief Getter for the number of errors found in a given file
	 * \param[in] p_fileId  File ID, as described in the XML configuration, for which the number of
	 *                      errors have to be retrieved.
	 * \return
	 * The number of errors found in the given file
	 */
	int getNberrorsInAFile(const QString& p_fileId);

	/*!
	 * \brief Clear all the data contained in the model
	 */
	void clear();

	/*!
	 * \brief Ask for a refresh of the model so the UI is updated
	 *
	 * this method emits the dataChanged and layoutChanged signals so the Qt framework updates the graphical
	 * view associated to this model.
	 */
	void refresh();

	/*!
	 * \brief Builds the string representing the errors summary table when building the report.
	 * \param[in] p_writer  writer for which the summary table must be built (html, csv)
	 * \param[in] p_delimiter  delimiter character (needed for csv writer)
	 * \return
	 * The string representing the errors summary table for the report that is being built
	 */
	const QString getReportErrorSummaryTable(const QString& p_writer, const QString& p_delimiter) const;

private:

	/*!
	 * \brief Constructor of the model, does nothing special
	 * \param[in]  parent   Not used
	 */
	ModelSngAnalysisErrors(QObject *parent = Q_NULLPTR);

	/*!
	 * \brief Destructor of the model, does nothing
	 */
	virtual ~ModelSngAnalysisErrors();

	/*!
	 * \brief Vector containing all the errors of analysis that have been detected
	 */
	QVector<AnalysisError> __errors;

	/*!
	 * \brief Map of the number of errors detected for each file id
	 */
	QMap<QString, int> __nbErrorsByFileId;
};

#endif /* MODELSNGANALYSISERRORS_H_ */
