/*!
 * \file ModelSngReqMatrix.h
 * \brief Definition of the class ModelSngReqMatrix
 * \date 2016-07-21
 * \author f.souliers
 */

#ifndef MODELSNGREQMATRIX_H_
#define MODELSNGREQMATRIX_H_

#include <QAbstractItemModel>
#include <QString>
#include <QMap>

#include "Requirement.h"
#include "IRequirementFile.h"

/*!
 * \class ModelSngReqMatrix
 * \brief Singleton containing the information of all requirements.
 *
 * This class is also used as a model (see Qt Framework and MVC) to display the summary of requirement
 * traceability for the files configured (nb of requirements, errors, coverage for/in each file)
 *
 * Furthermore, this class contains the methods that should be called by file parser class when they detect
 * some requirements. Those methods are :
 * - addDefinedRequirement
 * - addExpectedCompositeRequirement
 * - addExpectedCoveredRequirement
 */
class ModelSngReqMatrix : public QAbstractItemModel
{
Q_OBJECT

public:

	/*!
	 * \brief Instance getter for the singleton object
	 * \return The reference of the instance
	 */
	static ModelSngReqMatrix& instance()
	{
		static ModelSngReqMatrix r;
		return (r);
	}

	/*!
	 * \brief Adds a defined requirement to the global requirement matrix.
	 *
	 *  A defined requirement is a requirement identified with the req_regex ; on the opposite, an
	 *  expected requirement is a requirement created as it has been identified as a reference of another
	 *  requirement (and maybe not yet defined).
	 * \param[in] file_id   file id being parsed in for witch the method has been called
	 * \param[in] p_r       requirement object to be added into the matrix
	 * \return
	 * - true if the requirement has been properly added
	 * - false if not
	 */
	bool addDefinedRequirement(const QString& file_id, RequirementRef p_r);

	/*!
	 * \brief Add an expected requirement as it has been defined as a subelement of another one
	 * \param[in] file_id file id being parsed in for witch the method has been called
	 * \param[in] p_parentId The Id of the requirement composed of at least p_r
	 * \param[in] p_r the requirement object which is a sublement of the requirement identified by p_parentId
	 */
	void addExpectedCompositeRequirement(const QString& file_id, const QString& p_parentId, RequirementRef p_r);

	/*!
	 * \brief Add an expected requirement as it has been defined as covered by the current one
	 *
	 * That means that p_reqId is a downstream requirement for p_coveredReq and p_coveredReq is an upstream
	 * requirement for p_reqId.
	 * \param[in] file_id file id being parsed in for witch the method has been called
	 * \param[in] p_reqId current requirement that covers p_coveredReq
	 * \param[in] p_coveredReq Requirement covered by the current one
	 */
	void addExpectedCoveredRequirement(const QString& file_id, const QString& p_reqId, RequirementRef p_coveredReq);

	/*!
	 * \brief Add a file containing requirements and parse it
	 *
	 * This method typically call the parse method on the file parser object.
	 * \param[in] p  The file parser object that has to be taken into account. This must be a valid pointer.
	 */
	void addRequirementFile(IRequirementFilePtr p);

	/*!
	 * \brief Compute the coverage values of files once they all have been parsed.
	 *
	 * At the end of the computation, the method emits dataChanged and layoutChanged signals to freshen
	 * file coverage view
	 */
	void computeCoverage();

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
	 * \see ModelSngReqMatrix.cpp for the details of the columns
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
	 * \see ModelSngReqMatrix.cpp for the details of the columns
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
	 * \brief Getter for a file object
	 * \param[in] p_fileID  Unique file ID of the required file object
	 * \return
	 * A pointer to the required file object
	 */
	IRequirementFilePtr getRequirementFile(const QString& p_fileID) const;

	/*!
	 * \brief Clear all the data contained in the model
	 */
	void clear();

	/*!
	 * \brief Builds the string representing the files summary table when building the report.
	 * \param[in] p_writer  writer for which the summary table must be built (html, csv)
	 * \param[in] p_delimiter  delimiter character (needed for csv writer)
	 * \return
	 * The string representing the file summary table (traceability matrix) for the report that is being built
	 */
	const QString getReportFileSummaryTable(const QString& p_writer, const QString& p_delimiter) const;

	/*!
	 * \brief Builds the string representing the details of files when building the report.
	 * \param[in] p_writer  writer for which the summary table must be built (html, csv)
	 * \param[in] p_delimiter  delimiter character (needed for csv writer)
	 * \return
	 * The string representing the details of files (detailed traceability matrix) for the report that is being built
	 */
	const QString getReportFileDetails(const QString& p_writer, const QString& p_delimiter) const;

private:

	/*!
	 * \brief Constructor of the model, does nothing special
	 * \param[in]  parent   Not used
	 */
	ModelSngReqMatrix(QObject *parent = Q_NULLPTR);

	/*!
	 * \brief Destructor of the model, does nothing
	 */
	virtual ~ModelSngReqMatrix();

	/*!
	 * \brief Map of the requirement objects, accessible by name (that means by the req_regex match)
	 *
	 * The requirements are only instantiated here, every other attributes use pointer to those objects
	 */
	QMap<QString, Requirement> __reqsByName;

	/*!
	 * \brief Vector of pointers to all of the valid requirements of a given file id
	 */
	QMap<QString, QVector<Requirement*>> __reqsByFileId;

	/*!
	 * \brief Map of the file objects, accessible by file ID
	 */
	QMap<QString, IRequirementFilePtr> __filesByFileId;

	/*!
	 * \brief Builds the string representing the upstream and downstream files of the current one
	 *
	 * This string is basically the first paragraph of the report for each requirement file. It shows
	 * the current document, its upstream documents and its downstream documents.
	 * \param[in] file_id     The ID of the current file
	 * \param[in] p_writer    Writer (csv, html) that must be used to build the string
	 * \param[in] p_delimiter Field delimiter in case of csv
	 * \return
	 * The string representing the file document coverage
	 */
	const QString __getReportFileDetailsDocCoverage(const QString& file_id,
	                                                const QString& p_writer,
	                                                const QString& p_delimiter) const;

	/*!
	 * \brief Builds the string representing the table of the requirements of the file and their coverage
	 *
	 * This string is basically the second paragraph of the report for each requirement file. It shows
	 * the requirements of the current document and for each of them, the corresponding downstream
	 * requirement if any.
	 * \param[in] file_id     The ID of the current file
	 * \param[in] p_writer    Writer (csv, html) that must be used to build the string
	 * \param[in] p_delimiter Field delimiter in case of csv
	 * \return
	 * The string representing the coverage table
	 */
	const QString __getReportFileDetailsReqCoverage(const QString& file_id,
	                                                const QString& p_writer,
	                                                const QString& p_delimiter) const;

	/*!
	 * \brief Builds the string representing the table of the requirements of the file and their upstream
	 *
	 * This string is basically the third paragraph of the report for each requirement file. It shows
	 * the requirements of the current document and for each of them, the corresponding upstream
	 * requirement if any.
	 * \param[in] file_id     The ID of the current file
	 * \param[in] p_writer    Writer (csv, html) that must be used to build the string
	 * \param[in] p_delimiter Field delimiter in case of csv
	 * \return
	 * The string representing the reverse coverage table
	 */
	const QString __getReportFileDetailsRevCov(const QString& file_id,
	                                           const QString& p_writer,
	                                           const QString& p_delimiter) const;

};

#endif /* MODELSNGREQMATRIX_H_ */
