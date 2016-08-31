/*
 * ModelSngReqMatrix.cpp
 *
 *  Created on: 21 juil. 2016
 *      Author: f.souliers
 */

#include <QDebug>
#include <QMutexLocker>

#include "ModelSngReqMatrix.h"
#include "ModelSngAnalysisErrors.h"
#include "FactoryReportBaseString.h"

static const int FILECOVERAGE_COL_ID = 0;
static const int FILECOVERAGE_COL_NBREQ = 1;
static const int FILECOVERAGE_COL_HASDWN = 2;
static const int FILECOVERAGE_COL_COVERAGE = 3;
static const int FILECOVERAGE_COL_NBERR = 4;
static const int FILECOVERAGE_COL_FILEPATH = 5;

static const QVector<QString> FILECOVERAGE_HEADERS { QObject::trUtf8("ID"),
                                                     QObject::trUtf8("Nb Exigences"),
                                                     QObject::trUtf8("Doit être couvert ?"),
                                                     QObject::trUtf8("Taux de couverture"),
                                                     QObject::trUtf8("Nombre d'erreurs dans le fichier"),
                                                     QObject::trUtf8("Lien fichier") };

static const QString STR_CSS_INVALID_ROW_CLASS_ATTR = " class=\"invalid\" ";


ModelSngReqMatrix::ModelSngReqMatrix(QObject *parent)
		: QAbstractItemModel(parent)
{
}

ModelSngReqMatrix::~ModelSngReqMatrix()
{
	clear();
}

void ModelSngReqMatrix::clear()
{
	QMap<QString, RequirementFileAbstractPtr>::iterator it;
	for (it = __filesByFileId.begin(); it != __filesByFileId.end() ; ++it)
	{
		RequirementFileAbstractPtr p = it.value();
		if (p) delete (p);

		__filesByFileId[it.key()] = NULL;
	}

	__reqsByName.clear();
	__reqsByFileId.clear();
}

void ModelSngReqMatrix::addRequirementFile(RequirementFileAbstractPtr p)
{
	QString f_id = p->getFileId();

	// If the file id is defined several times, then a configuration error is reported (see ModelConfiguration)
	// ... so direct inserting is safe here
	__filesByFileId.insert(f_id, p);
}

void ModelSngReqMatrix::updateRequirementFileWithAcceptedRequirements(RequirementFileAbstractPtr p)
{
	QMutexLocker l_rbf(&__reqsByFileIdMutex) ; // avoid simultaneous access

	QString f_id = p->getFileId();

	// If the file id is defined several times, then a configuration error is reported (see ModelConfiguration)
	// ... so direct inserting is safe here
	p->setRequirements(__reqsByFileId[f_id]);
}

bool ModelSngReqMatrix::addDefinedRequirement(const QString& file_id, RequirementRef p_r)
{
	bool retval = false;  // by default, the method fails

	if (p_r.getState() != Requirement::Defined)
	{
		AnalysisError e(AnalysisError::ERROR,
		                AnalysisError::CONSISTENCY,
		                file_id,
		                QObject::trUtf8("État non défini d'une exigence qui devrait être définie --> erreur dans l'analyseur de fichier"));

		ModelSngAnalysisErrors::instance().addError(e);
		return (retval);
	}

	QMutexLocker l_rbn(&__reqsByNameMutex) ; // avoid simultaneous access
	QString req_id = p_r.getId();
	if (!__reqsByName.contains(req_id))
	{
		// The requirement does not exist at all in the global matrix, this is the easy case : it is
		// just necessary to add it
		__reqsByName.insert(req_id, p_r);

		retval = true;
	}
	else
	{
		if (__reqsByName[req_id].getState() == Requirement::Expected)
		{
			// The requirement exists in the matrix, only because it has been referenced by another
			// requirement --> we just have to update it and set it as defined
			__reqsByName[req_id].setState(Requirement::Defined);
			__reqsByName[req_id].setLocation(p_r.getLocation());
			__reqsByName[req_id].setMustBeCovered(p_r.mustBeCovered());

			retval = true;
		}
		else
		{
			// Bad case : the requirement has already been defined --> this is an error case
			// Info of this requirement is not taken into account and the error is stored for reporting
			qDebug() << "Error double definition of " << req_id;
			AnalysisError e(AnalysisError::ERROR,
			                AnalysisError::CONSISTENCY,
			                p_r.getLocationId(),
			                QObject::trUtf8("Redéfinition de %1, préalablement définie dans %2").arg(req_id).arg(__reqsByName[req_id].getLocationId()));

			ModelSngAnalysisErrors::instance().addError(e);
			retval = false;
		}
	}

	// If the retval is true, it means the requirement is actually defined in the
	// current location --> store it in the __reqsByFileId map
	QMutexLocker l_rbf(&__reqsByFileIdMutex) ; // avoid simultaneous writing
	if (retval) __reqsByFileId[p_r.getLocationId()].append(&__reqsByName[req_id]);

	return (retval);
}

void ModelSngReqMatrix::addExpectedCompositeRequirement(const QString& location,
                                                        const QString& p_parentId,
                                                        RequirementRef p_r)
{
	QMutexLocker l_rbn(&__reqsByNameMutex) ; // avoid simultaneous access

	// The method has been called while there is no consistent «current requirement» parsing
	// the requirement file
	if (!__reqsByName.contains(p_parentId))
	{
		AnalysisError e(AnalysisError::ERROR,
		                AnalysisError::PARSING,
		                location,
		                QObject::trUtf8("Tentative de définition de composition d'une exigence indéfinie (exigence composante %1)").arg(p_r.getId()));

		ModelSngAnalysisErrors::instance().addError(e);
		return;
	}

	// First, if the requirement isn't in the global matrix, just add it. In case it is already
	// present, we don't have more information to add
	QString req_id = p_r.getId();
	if (!__reqsByName.contains(req_id)) __reqsByName.insert(req_id, p_r);

	// Second, create the link between the parent and this new requirement
	__reqsByName[p_parentId].addComposingRequirement(&(__reqsByName[req_id]));
	__reqsByName[req_id].setParent(&(__reqsByName[p_parentId]));
}

void ModelSngReqMatrix::addExpectedCoveredRequirement(const QString& location,
                                                      const QString& p_reqId,
                                                      RequirementRef p_coveredReq)
{
	QMutexLocker l_rbn(&__reqsByNameMutex) ; // avoid simultaneous access

	// The method has been called while there is no consistent «current requirement» parsing
	// the requirement file
	if (!__reqsByName.contains(p_reqId))
	{
		AnalysisError e(AnalysisError::ERROR,
		                AnalysisError::PARSING,
		                location,
		                QObject::trUtf8("Tentative de définition de couverture d'une exigence indéfinie (exigence couverte : %1)").arg(p_coveredReq.getId()));

		ModelSngAnalysisErrors::instance().addError(e);
		return;
	}

	// First, if the covered requirement ins't in the global matrix, just add it. In case it is already
	// present, we don't have more information to add
	QString covered_req_id = p_coveredReq.getId();
	if (!__reqsByName.contains(covered_req_id)) __reqsByName.insert(covered_req_id, p_coveredReq);

	// If the currently covered requirement is already covered ... there is an error ...
	if (__reqsByName[covered_req_id].isCovered())
	{
		AnalysisError e(AnalysisError::ERROR,
		                AnalysisError::CONSISTENCY,
		                location,
		                QObject::trUtf8("Couverture multiple : %1 déjà couverte par %2 et tentative de recouverture par %3").arg(covered_req_id).arg(__reqsByName[covered_req_id].getDownstreamRequirement()->getId()).arg(p_reqId));

		ModelSngAnalysisErrors::instance().addError(e);
	}
	else
	{
		// else, we just have to set this requirement as covered by the current one
		__reqsByName[covered_req_id].setDownstreamRequirement(&(__reqsByName[p_reqId]));

		// And the current requirement covers the covered requirement \o/ (so obvious ...)
		__reqsByName[p_reqId].addUpstreamRequirement(&(__reqsByName[covered_req_id]));
	}

}

void ModelSngReqMatrix::computeCoverage()
{
	qDebug() << "ModelSngReqMatrix::computeCoverage : consistency check" ;
	QMap<QString, Requirement>::iterator it;

	// First of all, compute consistency for all requirements
	for (it = __reqsByName.begin(); it != __reqsByName.end() ; ++it)
	{
		Requirement& req = it.value();
		req.computeConsistency() ;
	}

	// Then the upstream and downstream actually read can be updated
	for (it = __reqsByName.begin(); it != __reqsByName.end() ; ++it)
	{
		Requirement& req = it.value();

		// If the requirement is inconsistent, it must be ignored
		if (!req.isConsistent()) continue ;

		// Otherwize, the requirement can be taken into account
		QString req_id = req.getId();
		RequirementFileAbstractPtr reqFile = req.getLocation();

		// If any, downstream document can also be added
		RequirementPtr dw_req = req.getDownstreamRequirement();
		if (dw_req)
		{
			RequirementFileAbstractPtr dw_reqFile = dw_req->getLocation();
			if (dw_reqFile)
			{
				reqFile->addDownstreamDocument(dw_reqFile->getFileId(), dw_reqFile);
				dw_reqFile->addUpstreamDocument(reqFile->getFileId(), reqFile);

				// It is moreover possible to check consistency of the configuration
				// (important for reporting phase)
				if (!reqFile->mustHaveDownstreamDocuments())
				{
					// The file actually has downstream doc but its configuration says it shouldn't
					AnalysisError e(AnalysisError::WARNING,
					                AnalysisError::CONSISTENCY,
					                req.getExpectedBy(),
					                QObject::trUtf8("Incohérence de paramétrage, %1 doit avoir des documents avals (downstream) mais sa configuration ne le mentionne pas").arg(reqFile->getFileId()));

					ModelSngAnalysisErrors::instance().addError(e);
				}

				if (!dw_reqFile->mustHaveUpstreamDocuments())
				{
					// The file actually has upstream doc but its configuration says it shouldn't
					AnalysisError e(AnalysisError::WARNING,
					                AnalysisError::CONSISTENCY,
					                req.getExpectedBy(),
					                QObject::trUtf8("Incohérence de paramétrage, %1 doit avoir des documents amonts (upstream) mais sa configuration ne le mentionne pas").arg(dw_reqFile->getFileId()));

					ModelSngAnalysisErrors::instance().addError(e);
				}
			}
		}

		// With the same logic, if the current requirement is covering other requirements (ie upstream
		// requirements), it means that the documents containing those requirements are upstream documents
		// for the current one
		foreach (RequirementPtr up_req, req.getUpstreamRequirements())
		{
			RequirementFileAbstractPtr up_reqFile = up_req->getLocation();
			if (up_reqFile)
			{
				reqFile->addUpstreamDocument(up_reqFile->getFileId(), up_reqFile);
				up_reqFile->addDownstreamDocument(reqFile->getFileId(), reqFile);

				// It is moreover possible to check consistency of the configuration
				// (important for reporting phase)
				if (!up_reqFile->mustHaveDownstreamDocuments())
				{
					// The file actually has downstream doc but its configuration says it shouldn't
					AnalysisError e(AnalysisError::WARNING,
					                AnalysisError::CONSISTENCY,
					                req.getExpectedBy(),
					                QObject::trUtf8("Incohérence de paramétrage, %1 doit avoir des documents avals (downstream) mais sa configuration ne le mentionne pas").arg(up_reqFile->getFileId()));

					ModelSngAnalysisErrors::instance().addError(e);
				}

				if (!reqFile->mustHaveUpstreamDocuments())
				{
					// The file actually has upstream doc but its configuration says it shouldn't
					AnalysisError e(AnalysisError::WARNING,
					                AnalysisError::CONSISTENCY,
					                req.getExpectedBy(),
					                QObject::trUtf8("Incohérence de paramétrage, %1 doit avoir des documents amonts (upstream) mais sa configuration ne le mentionne pas").arg(reqFile->getFileId()));

					ModelSngAnalysisErrors::instance().addError(e);
				}
			}
		}

		// TODO WTF with the composite requirements ? maybe create another section called «Related Documents»
	}

	// Finally, the coverage for each file can be calculated
	QMap<QString, RequirementFileAbstractPtr>::iterator file_it ;
	for (file_it = __filesByFileId.begin(); file_it != __filesByFileId.end() ; ++file_it)
	{
		qDebug() << "ModelSngReqMatrix::computeCoverage : Computing coverage for " << file_it.key() ;
		file_it.value()->computeCoverage() ;
	}

	// Data has been updated, the view can be freshened
	QModelIndex topLeft = index(0, 0);
	QModelIndex bottomRight = index(rowCount() - 1, columnCount() - 1);

	emit dataChanged(topLeft, bottomRight);
	emit layoutChanged();

	// same for error matrix
	ModelSngAnalysisErrors::instance().refresh();
}

int ModelSngReqMatrix::columnCount(const QModelIndex & /*parent*/) const
{
	return (FILECOVERAGE_HEADERS.count());
}

int ModelSngReqMatrix::rowCount(const QModelIndex & /*parent*/) const
{
	return (__filesByFileId.count());
}

QVariant ModelSngReqMatrix::data(const QModelIndex &index, int role) const
{
	QVariant retVal;

	// The framework is requiring the data to display
	if (role == Qt::DisplayRole)
	{
		retVal = QObject::trUtf8("ERREUR DATA");
		RequirementFileAbstractPtr f = __filesByFileId.values().at(index.row());
		if (!f) return (retVal);

		QString file_id = f->getFileId();
		switch (index.column())
		{
			case FILECOVERAGE_COL_ID:
				retVal = file_id;
				break;

			case FILECOVERAGE_COL_NBREQ:
				retVal = __reqsByFileId[file_id].count();
				break;

			case FILECOVERAGE_COL_HASDWN:
				if (__filesByFileId[file_id]->mustHaveDownstreamDocuments())
				{
					retVal = QObject::trUtf8("Oui");
				}
				else
				{
					retVal = QObject::trUtf8("Non");
				}
				break;

			case FILECOVERAGE_COL_COVERAGE:
				if (__filesByFileId[file_id]->mustHaveDownstreamDocuments())
				{
					retVal = QString("%1 %").arg(__filesByFileId[file_id]->getCoverage() * 100);
				}
				else
				{
					retVal = QObject::trUtf8("NA");
				}
				break;

			case FILECOVERAGE_COL_NBERR:
				retVal = ModelSngAnalysisErrors::instance().getNberrorsInAFile(file_id);
				break;

			case FILECOVERAGE_COL_FILEPATH:
				retVal = __filesByFileId[file_id]->getFilePath();
				break;

			default:
				retVal = QObject::trUtf8("ERREUR COLONNE");
		}
	}

	return (retVal);
}

QVariant ModelSngReqMatrix::headerData(int section, Qt::Orientation orientation, int role) const
{
	if (role == Qt::DisplayRole)
	{
		if (orientation == Qt::Horizontal)
		{
			return (FILECOVERAGE_HEADERS[section]);
		}
		else if (orientation == Qt::Vertical)
		{
			return (section + 1);
		}
	}

	return (QAbstractItemModel::headerData(section, orientation, role));
}

QModelIndex ModelSngReqMatrix::index(int row, int column, const QModelIndex & /*parent*/) const
{
	return (createIndex(row, column));
}

QModelIndex ModelSngReqMatrix::parent(const QModelIndex & /*index*/) const
{
	return (QModelIndex());
}

RequirementFileAbstractPtr ModelSngReqMatrix::getRequirementFile(const QString& p_fileID) const
{
	if (__filesByFileId.contains(p_fileID))
	{
		return (__filesByFileId[p_fileID]);
	}
	else
	{
		return (NULL);
	}
}

const QString ModelSngReqMatrix::getReportFileSummaryTable(const QString& p_writer,
                                                           const QString& p_delimiter) const
{
	FactoryReportBaseString fact;
	QString s_base(fact.getBaseString("summaryTable_line", p_writer, p_delimiter));

	QString result = "";
	QMap<QString, RequirementFileAbstractPtr>::const_iterator it;
	for (it = __filesByFileId.constBegin(); it != __filesByFileId.constEnd() ; ++it)
	{
		QString newline = s_base;
		QString file_id = it.value()->getFileId();

		if (p_writer == ModelConfiguration::OUTPUT_ATTR_VALUE_HTML)
		{
			newline.replace("REKKIX_SUMMARY_TABLE_FILE_ID",
			                QString("<a href=\"#%1\">%2</a>").arg(file_id).arg(file_id));
		}
		else
		{
			newline.replace("REKKIX_SUMMARY_TABLE_FILE_ID", file_id);
		}

		newline.replace("REKKIX_SUMMARY_TABLE_FILE_NB_REQS",
		                QString("%1").arg(__reqsByFileId[file_id].count()));
		newline.replace("REKKIX_SUMMARY_TABLE_FILE_PATH", it.value()->getFilePath());

		// Errors have a higher priority to set row color
		int nbErrors = ModelSngAnalysisErrors::instance().getNberrorsInAFile(file_id);
		newline.replace("REKKIX_SUMMARY_TABLE_FILE_NB_ERRORS", QString("%1").arg(nbErrors));
		if (nbErrors > 0)
		{
			// in case of CSV, nothing to replace -> nothing happens
			newline.replace("REKKIX_SUMMARY_TABLE_ROW_COLOR", STR_CSS_INVALID_ROW_CLASS_ATTR);
		}

		QString coverage;
		if (it.value()->mustHaveDownstreamDocuments())
		{
			coverage = QString("%1 %").arg(it.value()->getCoverage() * 100);
		}
		else
		{
			coverage = QObject::trUtf8("NA");
		}
		newline.replace("REKKIX_SUMMARY_TABLE_FILE_COVERAGE", coverage);

		// In case no item changed the row color, then remove the tag
		newline.replace("REKKIX_SUMMARY_TABLE_ROW_COLOR", "");

		result += newline + "\n";
	}

	return (result);
}

const QString ModelSngReqMatrix::getReportFileDetails(const QString& p_writer,
                                                      const QString& p_delimiter) const
{
	FactoryReportBaseString fact;
	QString s_details(fact.getBaseString("fileDetails_base", p_writer, p_delimiter));

	QString allFilesDetails = "";

	QMap<QString, RequirementFileAbstractPtr>::const_iterator it;
	for (it = __filesByFileId.constBegin(); it != __filesByFileId.constEnd() ; ++it)
	{
		QString newfile = s_details;
		QString file_id = it.key();

		newfile.replace("REKKIX_FILE_DIVID", file_id);
		newfile.replace("REKKIX_FILEID", file_id);
		newfile.replace("REKKIX_FILE_DOC_COVERAGE_CONTENT",
		                __getReportFileDetailsDocCoverage(file_id, p_writer, p_delimiter));
		if (it.value()->mustHaveDownstreamDocuments())
		{
			newfile.replace("REKKIX_FILE_REQ_COVERAGE_CONTENT",
			                __getReportFileDetailsReqCoverage(file_id, p_writer, p_delimiter));
		}
		else
		{
			newfile.replace("REKKIX_FILE_REQ_COVERAGE_CONTENT", "");
		}

		if (it.value()->mustHaveUpstreamDocuments())
		{
			newfile.replace("REKKIX_FILE_REQ_REVCOV_CONTENT",
			                __getReportFileDetailsRevCov(file_id, p_writer, p_delimiter));
		}
		else
		{
			newfile.replace("REKKIX_FILE_REQ_REVCOV_CONTENT", "");
		}

		allFilesDetails += newfile + "\n";
	}

	return (allFilesDetails);
}

const QString ModelSngReqMatrix::__getReportFileDetailsDocCoverage(const QString& file_id,
                                                                   const QString& p_writer,
                                                                   const QString& p_delimiter) const
{
	FactoryReportBaseString fact;
	QString s_docCoverage(fact.getBaseString("fileDetails_docCoverage", p_writer, p_delimiter));

	/*
	 * When using CSV, we should not use a \n, otherwise the report will start a new line
	 */
	QString lineSeparator = " ";
	if (p_writer == ModelConfiguration::OUTPUT_ATTR_VALUE_HTML) lineSeparator = "<br/>";

	// Data independent values
	s_docCoverage.replace("REKKIX_FILE_DOC_COVERAGE_TITLE", QObject::trUtf8("Hiérarchie documentaire"));
	s_docCoverage.replace("REKKIX_FILE_DOC_UPSTREAM_TITLE", QObject::trUtf8("Documents amonts"));
	s_docCoverage.replace("REKKIX_FILE_DOC_CURRENT_TITLE", QObject::trUtf8("Ce document"));
	s_docCoverage.replace("REKKIX_FILE_DOC_DOWNSTREAM_TITLE", QObject::trUtf8("Documents avals"));

	// Upstream document list
	QString upDocs = "";
	foreach(QString doc_id, __filesByFileId[file_id]->getUpstreamDocuments().keys()){
	upDocs += doc_id + lineSeparator;
}
	s_docCoverage.replace("REKKIX_FILE_DOC_UPSTREAM_DOCUMENTS", upDocs);

	// Current document
	s_docCoverage.replace("REKKIX_FILE_DOC_CURRENT_DOCUMENT", file_id);

	// Downstream document list
	QString dwDocs = "";
	foreach(QString doc_id, __filesByFileId[file_id]->getDownstreamDocuments().keys()){
	dwDocs += doc_id + lineSeparator;
}
	s_docCoverage.replace("REKKIX_FILE_DOC_DOWNSTREAM_DOCUMENTS", dwDocs);

	return (s_docCoverage);
}

const QString ModelSngReqMatrix::__getReportFileDetailsReqCoverage(const QString& file_id,
                                                                   const QString& p_writer,
                                                                   const QString& p_delimiter) const
{
	QString result = "";
	if (__filesByFileId[file_id]->mustHaveDownstreamDocuments())
	{
		FactoryReportBaseString fact;
		result = fact.getBaseString("fileDetails_reqCoverage_base", p_writer, p_delimiter);
		QString s_reqCoverageLine(fact.getBaseString("fileDetails_reqCoverage_line", p_writer, p_delimiter));

		/*
		 * When using CSV, we should not use a \n, otherwise the report will start a new line
		 */
		QString lineSeparator = " ";
		if (p_writer == ModelConfiguration::OUTPUT_ATTR_VALUE_HTML) lineSeparator = "<br/>";

		// Data independent values
		result.replace("REKKIX_FILE_DEFINED_REQS_WITH_COMPOSITE_AND_COVERAGE_TITLE",
		               QObject::trUtf8("Couverture du fichier"));
		result.replace("REKKIX_FILE_COV_REQ_ID_TITLE", QObject::trUtf8("Exigence"));
		result.replace("REKKIX_FILE_COV_COMPOSING_REQS_TITLE", QObject::trUtf8("Est composée de"));
		result.replace("REKKIX_FILE_COV_REQ_COVERAGE_TITLE", QObject::trUtf8("A un taux de couverture"));
		result.replace("REKKIX_FILE_COV_REQ_COVERED_BY_ID_TITLE", QObject::trUtf8("Est couverte par"));
		result.replace("REKKIX_FILE_COV_REQ_COVERED_BY_LOCATION_ID_TITLE",
		               QObject::trUtf8("Qui est définie dans"));

		// Create one line by requirement, append it to the list of lines and finally replace the table content
		// with the created lines
		QString lines = "";
		foreach(Requirement* req, __reqsByFileId[file_id]){
		QString newline = s_reqCoverageLine;

		newline.replace("REKKIX_FILE_COV_REQ_ID", req->getId());

		double coverage = req->getCoverage()*100;
		newline.replace("REKKIX_FILE_COV_COVERAGE", QString("%1 %").arg(coverage));
		if (coverage == 0)
		{
			newline.replace("REKKIX_COV_ROW_COLOR", STR_CSS_INVALID_ROW_CLASS_ATTR);
		}

		QString compReqs = "";
		foreach(Requirement* cmpReq, req->getComposingRequirements())
		{
			compReqs += cmpReq->getId() + lineSeparator;
		}
		newline.replace("REKKIX_FILE_COV_COMPOSING_REQS", compReqs);

		Requirement* covReq = req->getDownstreamRequirement();
		if (covReq)
		{
			newline.replace("REKKIX_FILE_COV_COVERED_BY_ID", covReq->getId());
			newline.replace("REKKIX_FILE_COV_COVERED_BY_LOCATION_ID", req->getLocationId());
		}
		else
		{
			newline.replace("REKKIX_FILE_COV_COVERED_BY_ID", "");
			newline.replace("REKKIX_FILE_COV_COVERED_BY_LOCATION_ID", "");
		}

		// In case no item changed the row color, then remove the tag
		newline.replace("REKKIX_COV_ROW_COLOR", "");

		lines += newline + "\n";
	}
		result.replace("REKKIX_FILE_COV_LINES_CONTENT", lines);
	}

	return (result);
}

const QString ModelSngReqMatrix::__getReportFileDetailsRevCov(const QString& file_id,
                                                              const QString& p_writer,
                                                              const QString& p_delimiter) const
{
	FactoryReportBaseString fact;
	QString s_reqRevCov(fact.getBaseString("fileDetails_reqRevCov_base", p_writer, p_delimiter));
	QString s_reqRevCovLine(fact.getBaseString("fileDetails_reqRevCov_line", p_writer, p_delimiter));

	/*
	 * When using CSV, we should not use a \n, otherwise the report will start a new line
	 */
	QString lineSeparator = " ";
	if (p_writer == ModelConfiguration::OUTPUT_ATTR_VALUE_HTML) lineSeparator = "<br/>";

	// Data independent values
	s_reqRevCov.replace("REKKIX_FILE_DEFINED_REQS_WITH_COMPOSITE_AND_REVERSE_COVERAGE_TITLE",
	                    QObject::trUtf8("Couverture réalisée par le fichier"));
	s_reqRevCov.replace("REKKIX_FILE_REVCOV_REQ_ID_TITLE", QObject::trUtf8("Exigence"));
	s_reqRevCov.replace("REKKIX_FILE_REVCOV_COMPOSING_REQS_TITLE", QObject::trUtf8("Est composée de"));
	s_reqRevCov.replace("REKKIX_FILE_REVCOV_COVERING_ID_TITLE", QObject::trUtf8("Couvre l'exigence"));
	s_reqRevCov.replace("REKKIX_FILE_REVCOV_COVERING_LOCATION_ID_TITLE",
	                    QObject::trUtf8("Qui est définie dans"));

	// Create one line by requirement, append it to the list of lines and finally replace the table content
	// with the created lines
	QString lines = "";
	foreach(Requirement* req, __reqsByFileId[file_id]){
	QString newline = s_reqRevCovLine;

	newline.replace("REKKIX_FILE_REVCOV_REQ_ID", req->getId());

	QString compReqs = "";
	foreach(Requirement* cmpReq, req->getComposingRequirements())
	{
		compReqs += cmpReq->getId() + lineSeparator;
	}
	newline.replace("REKKIX_FILE_REVCOV_COMPOSING_REQS", compReqs);

	QString covReqsId = "";
	QString covReqsLocation = "";
	foreach(Requirement* covReq, req->getUpstreamRequirements())
	{
		covReqsId += covReq->getId() + lineSeparator;
		covReqsLocation += covReq->getLocationId() + lineSeparator;
	}
	newline.replace("REKKIX_FILE_REVCOV_COVERING_ID", covReqsId);
	newline.replace("REKKIX_FILE_REVCOV_COVERING_LOCATION_ID", covReqsLocation);

	lines += newline + "\n";
}
	s_reqRevCov.replace("REKKIX_FILE_REVCOV_LINES_CONTENT", lines);

	return (s_reqRevCov);
}
