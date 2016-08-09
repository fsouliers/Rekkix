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


class Rekkix : public QMainWindow, public Ui::RekkixMW
{
	Q_OBJECT

public :
	Rekkix(QApplication * parent) ;
	Rekkix(QApplication * parent, const char * p_filename) ;
	~Rekkix() ;
	void loadConfigurationFile(const char* p_filename) ;

public slots :
	void slt_loadConfigurationFile() ;
	void slt_startAnalysis() ;
	void slt_fileCoverageSummary_selected(QModelIndex p_index) ;
	void slt_generateReports() ;

private :
	QApplication * __app ; // Application running the window

	ModelConfiguration __cnfModel ; // Model (as part of MVC) used to access configuration data
	ModelConfigurationErrors __cnfErrorsModel ; // Model (as part of MVC) used to access configuration data
	ModelStreamDocuments __currentlyDisplayedUptreamDocsModel ;
	ModelStreamDocuments __currentlyDisplayedDownstreamDocsModel ;
	ModelReqsCoveringUpstream __upstreamCoverageModel ;
	ModelReqsCoveredDownstream __downstreamCoverageModel ;
	ModelCompositeReqs __compositeRequirementsModel ;
	ModelReqs __requirementsModel ;

	void __common_init() ; // common initialisations

	void __loadFileAndInitGui(const char* p_filename) ;

	const QString __getReportSummaryTable(const QString& p_writer, const QString& p_delimiter) const ;
	const QString __getReportErrorsTable(const QString& p_writer, const QString& p_delimiter) const ;
} ;

typedef Rekkix* RekkixPtr ;

#endif



