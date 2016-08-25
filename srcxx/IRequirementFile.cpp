/*!
 * \file IRequirementFile.cpp
 * \brief Implementation of the class IRequirementFile
 * \date 2016-07-15
 * \author f.souliers
 */

#include <QDebug>

#include "IRequirementFile.h"
#include "ModelSngReqMatrix.h"
#include "ModelSngAnalysisErrors.h"

IRequirementFile::IRequirementFile(const ModelConfiguration::CnfFileAttributesMap_t& p_cnfFile)
		: _avgCoverage(0.0), _cnfFile(p_cnfFile)
{
	// regex validy is verified while reading the configuration file, see ModelConfiguration
	_regexReq.setPattern(_cnfFile[ModelConfiguration::REQFILE_ATTR_REQREGEX]);
	_regexReq.optimize() ;

	if (hasCmpRegex())
	{
		_regexCmp.setPattern(_cnfFile[ModelConfiguration::REQFILE_ATTR_CMPREGEX]);
		_regexCmp.optimize() ;
	}

	if (hasCovRegex())
	{
		_regexCov.setPattern(_cnfFile[ModelConfiguration::REQFILE_ATTR_COVREGEX]);
		_regexCov.optimize() ;
	}

	if (hasStopAfterRegex())
	{
		_regexStopAfter.setPattern(_cnfFile[ModelConfiguration::REQFILE_ATTR_STOPAFTERREGEX]);
		_regexStopAfter.optimize() ;
	}
}

IRequirementFile::~IRequirementFile()
{

}

void IRequirementFile::computeCoverage()
{
	QVector<Requirement*>::iterator it ;
	double sum = 0.0 ;
	bool invalidFound = false ;
	for (it = _requirements.begin() ; it != _requirements.end() ; ++it)
	{
		if ((*it)->isConsistent())
		{
			sum += (*it)->getCoverage() ;
		}
		else
		{
			// if an invalid requirement is detected, note it but go on with calculating the coverage values
			invalidFound = true ;
		}
	}

	if (invalidFound)
	{
		_avgCoverage = Requirement::COVERAGE_INVALID_VALUE ;
	}
	else
	{
		_avgCoverage = sum / _requirements.count() ;
	}
}

void IRequirementFile::addUpstreamDocument(const QString& p_fileId, IRequirementFile* p_file)
{
	qDebug() << "IRequirementFile::addUpstreamDocument " << _cnfFile[ModelConfiguration::REQFILE_ATTR_ID] << " : trying to add " << p_fileId;

	// If the document is already an upstream document, nothing to do
	if (!_upstreamDocs.contains(p_fileId))
	{
		_upstreamDocs.insert(p_fileId, p_file);
	}
}

void IRequirementFile::addDownstreamDocument(const QString& p_fileId, IRequirementFile* p_file)
{
	qDebug() << "IRequirementFile::addDownstreamDocument " << _cnfFile[ModelConfiguration::REQFILE_ATTR_ID] << " : trying to add " << p_fileId;

	// If the document is already a downstream document, nothing to do
	if (!_downstreamDocs.contains(p_fileId))
	{
		_downstreamDocs.insert(p_fileId, p_file);
	}
}

bool IRequirementFile::_hasStoredAnyRequirementDefinition(const QString& p_text,
                                                 QString& p_reqfound,
                                                 bool& p_reqAcceptable)
{
	QRegularExpressionMatch req_m = _regexReq.match(p_text);
	if (req_m.hasMatch())
	{
		// the real requirement list is only the group of the regex named REQFILE_GRPNAME_REQID,
		// each requirement must be separated by ModelConfiguration::REQFILE_ATTR_CMPSEPARATOR
		p_reqfound = req_m.captured(ModelConfiguration::REQFILE_GRPNAME_REQID);

		// then it is possible to build a requirement object
		Requirement r(p_reqfound, Requirement::Defined, this->getFileId());
		r.setLocation(this);
		r.setMustBeCovered(mustHaveDownstreamDocuments());

		// If the requirement has already been defined, the global Matrix handles itself the error log
		p_reqAcceptable = ModelSngReqMatrix::instance().addDefinedRequirement(_cnfFile[ModelConfiguration::REQFILE_ATTR_ID], r);

		return (true);
	}
	else
	{
		// the regex didn't match
		return (false);
	}
}

bool IRequirementFile::_hasStoredAnyExpectedCompositeRequirements(const QString& p_text, const QString& p_parentReqId)
{
	QRegularExpressionMatch cmp_m = _regexCmp.match(p_text);
	if (cmp_m.hasMatch())
	{
		// the real requirement list is only the group of the regex named REQFILE_GRPNAME_REQLST,
		// each requirement must be separated by ModelConfiguration::REQFILE_ATTR_CMPSEPARATOR
		QString tmp = cmp_m.captured(ModelConfiguration::REQFILE_GRPNAME_REQLST);

		QString loc_str ;
		if (p_parentReqId.isEmpty())
		{
			loc_str = _cnfFile[ModelConfiguration::REQFILE_ATTR_ID] ;
		}
		else
		{
			loc_str = _cnfFile[ModelConfiguration::REQFILE_ATTR_ID] + " / " + p_parentReqId ;
		}

		foreach(QString s, tmp.split(_cnfFile[ModelConfiguration::REQFILE_ATTR_CMPSEPARATOR]))
		{
			Requirement r(s.trimmed(), Requirement::Expected, loc_str);
			ModelSngReqMatrix::instance().addExpectedCompositeRequirement(loc_str, p_parentReqId, r);
		}

		return(true) ;
	}
	else
	{
		return(false) ;
	}
}

bool IRequirementFile::_hasStoredAnyExpectedUpstreamRequirements(const QString& p_text, const QString& p_currentReqId)
{
	QRegularExpressionMatch cov_m = _regexCov.match(p_text);
	if (cov_m.hasMatch())
	{
		qDebug() << "IRequirementFile::_hasExpectedUpstreamRequirements regex match :" << cov_m.capturedTexts() ;

		// the real requirement list is only the group of the regex named REQFILE_GRPNAME_REQLST,
		// each requirement must be separated by ModelConfiguration::REQFILE_ATTR_CMPSEPARATOR
		QString tmp = cov_m.captured(ModelConfiguration::REQFILE_GRPNAME_REQLST);

		QString loc_str ;
		if (p_currentReqId.isEmpty())
		{
			loc_str = _cnfFile[ModelConfiguration::REQFILE_ATTR_ID] ;
		}
		else
		{
			loc_str = _cnfFile[ModelConfiguration::REQFILE_ATTR_ID] + " / " + p_currentReqId ;
		}

		foreach(QString s, tmp.split(_cnfFile[ModelConfiguration::REQFILE_ATTR_COVSEPARATOR]))
		{
			Requirement r(s.trimmed(), Requirement::Expected, loc_str);
			ModelSngReqMatrix::instance().addExpectedCoveredRequirement(loc_str, p_currentReqId, r);
		}

		return(true) ;
	}
	else
	{
		return(false) ;
	}
}

bool IRequirementFile::_mustStopParsing(const QString& p_text)
{
	QRegularExpressionMatch sm_m = _regexStopAfter.match(p_text);
	if (sm_m.hasMatch())
	{
		return(true) ;
	}
	else
	{
		return(false) ;
	}
}
