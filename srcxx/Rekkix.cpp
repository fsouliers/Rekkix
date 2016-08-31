/*!
 * \file Rekkix.cpp
 * \brief Implementation of the class Rekkix
 * \date 2016-07-15
 * \author f.souliers
 */

#include <QDebug>
#include <QFileDialog>
#include <QItemSelectionModel>
#include <QTextStream>
#include <QDateTime>
#include <QMessageBox>
#include <QMutexLocker>
#include <QThread>

#include "Rekkix.h"
#include "ModelSngReqMatrix.h"
#include "ModelSngAnalysisErrors.h"
#include "ModelConfiguration.h"
#include "FactoryRequirementFile.h"
#include "FactoryReportBaseString.h"
#include "AnalysisSngWaiterThread.h"
#include "AnalysisSngStarterThread.h"

// Constructor #1
Rekkix::Rekkix(QApplication * parent, bool setupUI)
		: QMainWindow(NULL, Qt::Window)
{

	// Set the application running the window
	__app = parent;

	if (setupUI)
	{
		// Build GUI
		Ui::RekkixMW::setupUi(this);

		// tab "configuration"
		this->tv_configuredFiles->setModel(&__cnfModel);
		this->tv_configurationErrors->setModel(&__cnfErrorsModel);

		// tab "results"
		this->tv_filesCoverageSummary->setModel(&ModelSngReqMatrix::instance());
		this->tv_downstreamDocs->setModel(&__currentlyDisplayedDownstreamDocsModel);
		this->tv_upstreamDocs->setModel(&__currentlyDisplayedUpstreamDocsModel);
		this->tv_upstreamCoverage->setModel(&__upstreamCoverageModel);
		this->tv_downstreamCoverage->setModel(&__downstreamCoverageModel);
		this->tv_compositeReqs->setModel(&__compositeRequirementsModel);
		this->tv_definedReqs->setModel(&__requirementsModel);
		this->tv_errors->setModel(&ModelSngAnalysisErrors::instance());
	}

	__isBatchMode = false ;
	__nbFiles = -1 ;
}

Rekkix::~Rekkix()
{
}

void Rekkix::loadFileAndInitGui(const char* p_filename)
{
	__cnfModel.setFile(p_filename, __cnfErrorsModel);
	this->progbar_analysis->setValue(0);
	this->tv_configuredFiles->resizeColumnsToContents();
	this->tv_configurationErrors->resizeColumnsToContents();

	if (__cnfErrorsModel.hasSignificantError())
	{
		this->pb_startAnalysis->setEnabled(false);
	}
	else
	{
		this->pb_startAnalysis->setEnabled(true);
	}
}

int Rekkix::loadFileAndRunBatch(const char* p_filename)
{
	// the program is running without GUI
	__isBatchMode = true ;

	fprintf(stdout, "\n%s\n", QObject::trUtf8("Chargement du fichier de configuration : %1").arg(p_filename).toStdString().c_str()) ;
	__cnfModel.setFile(p_filename, __cnfErrorsModel);

	// Check if the configuration is OK
	if (__cnfErrorsModel.hasSignificantError())
	{
		fprintf(stdout, "%s", QObject::trUtf8("Erreurs graves de configuration identifées, impossible de continuer :").toStdString().c_str()) ;

		QString err = "" ;
		__cnfErrorsModel.appendErrorString(err) ;
		__cnfErrorsModel.appendWarningString(err) ;
		fprintf(stdout, "%s\n", err.toStdString().c_str()) ;

		return(EXIT_FAILURE) ;
	}
	else if (__cnfErrorsModel.hasWarning())
	{
		fprintf(stdout, "%s", QObject::trUtf8("Alertes de configuration (l'analyse peut continuer) :").toStdString().c_str()) ;

		QString err = "" ;
		__cnfErrorsModel.appendWarningString(err) ;
		fprintf(stdout, "%s\n", err.toStdString().c_str()) ;
	}
	else
	{
		fprintf(stdout, "%s\n", QObject::trUtf8("Configuration OK").toStdString().c_str()) ;
	}

	// Run analysis and generate report
	fprintf(stdout, "%s\n", QObject::trUtf8("Analyse des documents mentionnés en configuration :").toStdString().c_str()) ;
	slt_startAnalysis() ;

	fprintf(stdout, "%s\n", QObject::trUtf8("Génération des rapports mentionnés en configuration :").toStdString().c_str()) ;
	slt_generateReports() ;

	// Happy end
	return(EXIT_SUCCESS) ;
}



void Rekkix::slt_loadConfigurationFile()
{
	QString filename;

	filename = QFileDialog::getOpenFileName(this, QObject::trUtf8("Choix du fichier à ouvrir"), ".", QObject::trUtf8("Fichiers ini (*.ini);;Tous (*.*)"));

	if (filename.size() > 0)
	{
		loadFileAndInitGui(filename.toStdString().c_str());
	}
}

void Rekkix::slt_startAnalysis()
{
	ModelConfiguration::CnfFileAttributesMapsByFileId_t files = __cnfModel.getConfiguredRequirementFiles();
	__nbFiles = files.count() ;
	ModelSngReqMatrix::instance().clear();
	ModelSngAnalysisErrors::instance().clear();

	// 1st Step of analysis : reading all the expected files
	QVector<RequirementFileAbstractPtr> registeredFiles ;
	ModelConfiguration::CnfFileAttributesMapsByFileId_t::iterator it;
	int i;
	for (it = files.begin(), i = 1; it != files.end() ; ++it, ++i)
	{
		RequirementFileAbstractPtr f = FactoryRequirementFile::getRequirementFile(it.value());
		if (f)
		{
			// If the Requirement file is a valid object according to its description in the configuration,
			// it can be added to the list of files and the thread can be started
			registeredFiles.append(f) ;
			ModelSngReqMatrix::instance().addRequirementFile(f);

			QObject::connect(f, SIGNAL(parsingFinished()), this, SLOT(slt_reqFileParsingOneMoreFileFinished())) ;
			QObject::connect(f, SIGNAL(started()), &AnalysisSngWaiterThread::instance(), SLOT(slt_parsingStarted())) ;
		}
	}

	// 2nd step start the thread waiting for parsing threads ending
	AnalysisSngWaiterThread::instance().setRegisteredFiles(registeredFiles) ;
	QObject::connect(&AnalysisSngWaiterThread::instance(), SIGNAL(allRegisteredTerminated()), this, SLOT(slt_reqFileParsingAllFilesFinished())) ;
	AnalysisSngWaiterThread::instance().start() ;

	// 3rd step start the the thread starting the other ones (main goal is not to interfere with Qt main loop)
	AnalysisSngStarterThread::instance().setRegisteredFiles(registeredFiles) ;
	AnalysisSngStarterThread::instance().release(20) ; // TODO 20 max threads at the same time ... should be configurable
	AnalysisSngStarterThread::instance().start() ;
}

void Rekkix::slt_reqFileParsingAllFilesFinished()
{
	// 3rd Step of analysis : computing coverage (only once all requirements are known)
	QMutexLocker l(&__fileParsingFinishedGuiUpdate) ;
	ModelSngReqMatrix::instance().computeCoverage();

	int progbar_value = 100 ; // 100% reached ... arbitrarily ...
	if (__isBatchMode)
	{
		fprintf(stdout, "%s\n", QObject::trUtf8("\rAnalyse : %01 %\n").arg(progbar_value).toStdString().c_str()) ;
	}
	else
	{
		this->progbar_analysis->setValue(progbar_value);

		// Last Step : change current tab to display results
		this->mw_tabs->setCurrentWidget(this->tab_results);
		this->tv_filesCoverageSummary->resizeColumnsToContents();
		this->tv_errors->resizeColumnsToContents();
	}
}


void Rekkix::slt_reqFileParsingOneMoreFileFinished()
{
	QMutexLocker l(&__fileParsingFinishedGuiUpdate) ;
	int progbar_value ;
	static int nbParsedFiles = 0 ;

	nbParsedFiles++ ;

	// Calculate GUI update so the user can see the software is alive
	progbar_value = 84 * nbParsedFiles / __nbFiles + 1;  // dumb calculation just to ensure the value will alway be in [1;85] ... why 85 ? because 42 is not big enough
	if (__isBatchMode)
	{
		fprintf(stdout, "%s", QObject::trUtf8("\rAnalyse : %01 %").arg(progbar_value).toStdString().c_str()) ;
	}
	else
	{
		this->progbar_analysis->setValue(progbar_value);
	}

	// One new thread can be launched
	AnalysisSngStarterThread::instance().release(1) ;
}


void Rekkix::slt_fileCoverageSummary_selected(QModelIndex /*p_index*/)
{
	// clear all the models of the other views
	__currentlyDisplayedUpstreamDocsModel.clear();
	__currentlyDisplayedDownstreamDocsModel.clear();
	__requirementsModel.clear();
	__upstreamCoverageModel.clear();
	__downstreamCoverageModel.clear();
	__compositeRequirementsModel.clear();

	// then compute their new value
	QItemSelectionModel* ism = tv_filesCoverageSummary->selectionModel();
	if (ism->hasSelection())
	{
		foreach(QModelIndex mi, ism->selectedRows())
		{
			QString file_id = mi.sibling(mi.row(), 0).data().toString();
			qDebug() << "Rekkix::slt_fileCoverageSummary_selected " << file_id;

			RequirementFileAbstractPtr f = ModelSngReqMatrix::instance().getRequirementFile(file_id);
			if (f)
			{
				__currentlyDisplayedUpstreamDocsModel.addDocumentSet(f->getUpstreamDocuments());
				__currentlyDisplayedDownstreamDocsModel.addDocumentSet(f->getDownstreamDocuments());

				QVector<Requirement*> v_r = f->getRequirements();
				__requirementsModel.addRequirementsOfAFile(v_r);
				__upstreamCoverageModel.addRequirementsOfAFile(v_r);
				__downstreamCoverageModel.addRequirementsOfAFile(v_r);
				__compositeRequirementsModel.addRequirementsOfAFile(v_r);
			}
		}

		// Models data are up to date : request for refresh GUI
		__currentlyDisplayedUpstreamDocsModel.refresh();
		__currentlyDisplayedDownstreamDocsModel.refresh();
		__requirementsModel.refresh();
		__upstreamCoverageModel.refresh();
		__downstreamCoverageModel.refresh();
		__compositeRequirementsModel.refresh();

		this->tv_downstreamDocs->resizeColumnsToContents();
		this->tv_upstreamDocs->resizeColumnsToContents();
		this->tv_upstreamCoverage->resizeColumnsToContents();
		this->tv_downstreamCoverage->resizeColumnsToContents();
		this->tv_compositeReqs->resizeColumnsToContents();
		this->tv_definedReqs->resizeColumnsToContents();
	}
}

void Rekkix::slt_generateReports()
{
	QDateTime reportTimestamp = QDateTime::currentDateTime();

	foreach(ModelConfiguration::CnfFileAttributesMap_t outFileDescription, __cnfModel.getOutputFiles())
	{
		// Step 0 : prepare base file templates
		QString writer = outFileDescription[ModelConfiguration::OUTPUT_ATTR_WRITER];
		QString delimiter = outFileDescription[ModelConfiguration::OUTPUT_ATTR_DELIMITER];
		FactoryReportBaseString fact;
		QString s_base(fact.getBaseString("base", writer, delimiter));

		// In case of html, Step 1 : include CSS and replace the template values
		if (writer == ModelConfiguration::OUTPUT_ATTR_VALUE_HTML)
		{
			s_base.replace("REKKIX_CSS_SECTION", fact.getCSString());
		}

		// Step 2 : build the report header and summary table
		s_base.replace("REKKIX_REPORT_TITLE", QObject::trUtf8("Rapport de traçabilité"));
		s_base.replace("REKKIX_REPORT_SUBTITLE", QObject::trUtf8("Généré le %1").arg(reportTimestamp.toString(QObject::trUtf8("dd/MM/yyyy hh:mm:ss"))));
		s_base.replace("REKKIX_SUMMARY_TITLE", QObject::trUtf8("1/ Matrice de traçabilité"));
		s_base.replace("REKKIX_SUMMARY_TABLE", __getReportSummaryTable(writer, delimiter));

		// Step 3 : build the error section
		s_base.replace("REKKIX_ERRORS_TITLE", QObject::trUtf8("2/ Erreurs trouvées dans l'analyse des fichiers"));
		s_base.replace("REKKIX_ERRORS_CONTENT", __getReportErrorsTable(writer, delimiter));

		// Step 4 : build the report with data and replace the tag
		s_base.replace("REKKIX_DETAILS_TITLE", QObject::trUtf8("3/ Détails de traçabilité des fichiers"));
		s_base.replace("REKKIX_DETAILS_CONTENT", ModelSngReqMatrix::instance().getReportFileDetails(writer, delimiter));

		// Step 5 : store file on disk
		QString outFilePath(outFileDescription[ModelConfiguration::OUTPUT_ATTR_PATH]);
		outFilePath.replace("REKKIXTIMESTAMP", reportTimestamp.toString("yyyyMMddhhmmss"));

		qDebug() << "Rekkix::slt_generateReports : generating " << outFilePath;
		QFile fout(outFilePath);
		fout.open(QIODevice::WriteOnly | QIODevice::Text);
		QTextStream out_stream(&fout);
		out_stream.setCodec("UTF-8");
		out_stream << s_base;
		fout.close();
	}

	if (__isBatchMode)
	{
		fprintf(stdout, "%s\n", QObject::trUtf8("Fin de génération des rapports").toStdString().c_str()) ;
	}
	else
	{
		QMessageBox::information(NULL,
		                         QObject::trUtf8("Génération du rapport terminée"),
		                         QObject::trUtf8("Fin de génération des rapports"),
		                         QMessageBox::Ok);
	}
}


const QString Rekkix::__getReportSummaryTable(const QString& p_writer, const QString& p_delimiter) const
{
	FactoryReportBaseString fact;
	QString s_base(fact.getBaseString("summaryTable_base", p_writer, p_delimiter));

	s_base.replace("REKKIX_SUMMARY_TABLE_FILE_ID_TITLE", QObject::trUtf8("ID Fichier"));
	s_base.replace("REKKIX_SUMMARY_TABLE_FILE_NB_REQS_TITLE", QObject::trUtf8("Nb Exigences"));
	s_base.replace("REKKIX_SUMMARY_TABLE_FILE_COVERAGE_TITLE", QObject::trUtf8("Couverture"));
	s_base.replace("REKKIX_SUMMARY_TABLE_FILE_NB_ERRORS_TITLE", QObject::trUtf8("Nb Erreurs"));
	s_base.replace("REKKIX_SUMMARY_TABLE_FILE_PATH_TITLE", QObject::trUtf8("Path Fichier"));
	s_base.replace("REKKIX_SUMMARY_TABLE_LINES_CONTENT", ModelSngReqMatrix::instance().getReportFileSummaryTable(p_writer, p_delimiter));

	return (s_base);
}

const QString Rekkix::__getReportErrorsTable(const QString& p_writer, const QString& p_delimiter) const
{
	FactoryReportBaseString fact;
	QString s_base(fact.getBaseString("errors_base", p_writer, p_delimiter));

	s_base.replace("REKKIX_ERRORS_SEVERITY_TITLE", QObject::trUtf8("Sévérité"));
	s_base.replace("REKKIX_ERRORS_CATEGORY_TITLE", QObject::trUtf8("Catégorie"));
	s_base.replace("REKKIX_ERRORS_LOCATION_ID_TITLE", QObject::trUtf8("Localisation"));
	s_base.replace("REKKIX_ERRORS_DESCRIPTION_TITLE", QObject::trUtf8("Description"));
	s_base.replace("REKKIX_ERRORS_LINES_CONTENT", ModelSngAnalysisErrors::instance().getReportErrorSummaryTable(p_writer, p_delimiter));

	return (s_base);
}

