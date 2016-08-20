/*!
 * \file Rekkix.h
 * \brief Definition of the class Rekkix
 * \date 2016-07-15
 * \author f.souliers
 */

#ifndef REKKIX_H
#define REKKIX_H

#include <QApplication>
#include <QMainWindow>

// Auto-generated ui headers
#include "ui_Rekkix.h"

// Application headers
#include "ModelConfiguration.h"
#include "ModelStreamDocuments.h"
#include "ModelReqsCoveringUpstream.h"
#include "ModelReqsCoveredDownstream.h"
#include "ModelCompositeReqs.h"
#include "ModelReqs.h"
#include "ModelConfigurationErrors.h"

/*!
 * \class Rekkix
 * \brief Class of the main window
 */
class Rekkix : public QMainWindow, public Ui::RekkixMW
{
Q_OBJECT

public:

	/*!
	 * \brief Constructor
	 * \param[in]  parent   Not used
	 * \param[in]  setupUI  should the constructor initialize some Graphical User Interface ?
	 */
	Rekkix(QApplication * parent, bool setupUI = true);

	/*!
	 * \brief Destructor of the model, only ensures to close the configuration file if needed
	 */
	~Rekkix();

public slots :

	/*!
	 * \brief slot used to load a configuration file (open file dialog)
	 */
	void slt_loadConfigurationFile();

	/*!
	 * \brief start the analysis with the loaded configuration file
	 * \param[in] isBatchMode  If set to true, it means the slot hasn't been called from the gui but from
	 *                         the batch mode running code. In such a case, do not interacts with the gui
	 *                         (as it doesn't exist)
	 */
	void slt_startAnalysis(bool isBatchMode = false);

	/*!
	 * \brief this slot is called when a file is selected in the files coverage summary view
	 * \param[in] p_index  Index selected in the view (needed to identify the selected row)
	 */
	void slt_fileCoverageSummary_selected(QModelIndex p_index);

	/*!
	 * \brief generate the report as specified in the loaded configuration file
	 * \param[in] isBatchMode  If set to true, it means the slot hasn't been called from the gui but from
	 *                         the batch mode running code. In such a case, do not interacts with the gui
	 *                         (as it doesn't exist)
	 */
	void slt_generateReports(bool isBatchMode = false);

	/*!
	 * \brief load a configuration file (from gui or from command line) and initialize gui accordingly
	 * \param[in] p_filename  file name & path of the file to be loaded
	 */
	void loadFileAndInitGui(const char* p_filename);

	/*!
	 * \brief load a configuration file (from command line), compute analysis and generate reports
	 * \param[in] p_filename  file name & path of the file to be loaded
	 * \return
	 * - EXIT_SUCCESS in case everything goes properly
	 * - EXIT_FAILURE else and errors have been displayed on stderr
	 */
	int loadFileAndRunBatch(const char* p_filename);

private:

	QApplication * __app;  //!< Application running the window
	ModelConfiguration __cnfModel;  //!< loaded configuration
	ModelConfigurationErrors __cnfErrorsModel;  //!< configuration errors detected when loading the configuration file
	ModelStreamDocuments __currentlyDisplayedUpstreamDocsModel; //!< upstream documents of the currently selected one
	ModelStreamDocuments __currentlyDisplayedDownstreamDocsModel;//!< downstream documents of the currently selected one
	ModelReqsCoveringUpstream __upstreamCoverageModel; //!< model for upstream requirements
	ModelReqsCoveredDownstream __downstreamCoverageModel; //!< model for downstream requirements
	ModelCompositeReqs __compositeRequirementsModel; //!< model for composite requirements
	ModelReqs __requirementsModel; //!< model for requirements of the selected file

	/*!
	 * \brief When building a report, builds the string used for the summary of the report (list of files, errors, ...)
	 * \param[in] p_writer  writer to be used for string generation (html, csv)
	 * \param[in] p_delimiter  in case of csv, field delimiter
	 * \return
	 * The string representing the summary table according to the required format
	 */
	const QString __getReportSummaryTable(const QString& p_writer, const QString& p_delimiter) const;

	/*!
	 * \brief When building a report, builds the string used for the table of errors (severity, category, location, description)
	 * \param[in] p_writer  writer to be used for string generation (html, csv)
	 * \param[in] p_delimiter  in case of csv, field delimiter
	 * \return
	 * The string representing the table of errors according to the required format
	 */
	const QString __getReportErrorsTable(const QString& p_writer, const QString& p_delimiter) const;
};

/*!
 * \typedef RekkixPtr
 * \brief Just to make the code easier to type when a pointer to Rekkix is required
 */
typedef Rekkix* RekkixPtr;

#endif

