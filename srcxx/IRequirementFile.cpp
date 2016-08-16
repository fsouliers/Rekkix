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

IRequirementFile::IRequirementFile(const ModelConfiguration::XmlConfiguredFileAttributesMap_t& p_cnfFile)
		: _nbReqs(0), _sumOfCov(0.0), _avgCoverage(0.0), _cnfFile(p_cnfFile)
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

void IRequirementFile::addOneMoreRequirementCoverage(double p_coverage)
{
	_nbReqs += 1;
	_sumOfCov += p_coverage;
	_avgCoverage = _sumOfCov / _nbReqs;
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

bool IRequirementFile::_hasRequirementDefinition(const QString& p_text,
                                                 QString& p_reqfound,
                                                 bool& p_reqAcceptable)
{
	QRegularExpressionMatch req_m = _regexReq.match(p_text);
	if (req_m.hasMatch())
	{
		// the real requirement is the whole matching of the regex
		p_reqfound = req_m.capturedTexts()[0];

		// then it is possible to build a requirement object
		Requirement r(p_reqfound, Requirement::Defined);
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

bool IRequirementFile::_hasExpectedCompositeRequirements(const QString& p_text, const QString& p_parentReqId)
{
	QRegularExpressionMatch cmp_m = _regexCmp.match(p_text);
	if (cmp_m.hasMatch())
	{
		// the real requirement list is only the group of the regex (index 1 of the list, index 0 being the
		// global match of the regex), each requirement must be separated by ModelConfiguration::REQFILE_ATTR_CMPSEPARATOR
		QString tmp = cmp_m.capturedTexts()[1];

		foreach(QString s, tmp.split(_cnfFile[ModelConfiguration::REQFILE_ATTR_CMPSEPARATOR]))
		{
			Requirement r(s.trimmed(), Requirement::Expected, _cnfFile[ModelConfiguration::REQFILE_ATTR_ID]);
			ModelSngReqMatrix::instance().addExpectedCompositeRequirement(_cnfFile[ModelConfiguration::REQFILE_ATTR_ID], p_parentReqId, r);
		}

		return(true) ;
	}
	else
	{
		return(false) ;
	}
}

bool IRequirementFile::_hasExpectedUpstreamRequirements(const QString& p_text, const QString& p_currentReqId)
{
	QRegularExpressionMatch cov_m = _regexCov.match(p_text);
	if (cov_m.hasMatch())
	{
		qDebug() << "IRequirementFile::_hasExpectedUpstreamRequirements MATCH POINT" << cov_m.capturedTexts() ;

		// the real requirement list is only the group of the regex (index 1 of the list, index 0 being the
		// global match of the regex), each requirement must be separated by ModelConfiguration::REQFILE_ATTR_COVSEPARATOR
		QString tmp = cov_m.capturedTexts()[1];

		foreach(QString s, tmp.split(_cnfFile[ModelConfiguration::REQFILE_ATTR_COVSEPARATOR]))
		{
			Requirement r(s.trimmed(), Requirement::Expected, _cnfFile[ModelConfiguration::REQFILE_ATTR_ID]);
			ModelSngReqMatrix::instance().addExpectedCoveredRequirement(_cnfFile[ModelConfiguration::REQFILE_ATTR_ID], p_currentReqId, r);
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
