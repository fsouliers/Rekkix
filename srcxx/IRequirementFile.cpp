/*!
 * \file IRequirementFile.cpp
 * \brief Implementation of the class IRequirementFile
 * \date 2016-07-15
 * \author f.souliers
 */

#include <QDebug>

#include "IRequirementFile.h"
#include "ModelSngAnalysisErrors.h"

IRequirementFile::IRequirementFile(ModelConfiguration::XmlConfiguredFileAttributesMap_t& p_cnfFile)
		: _nbReqs(0), _sumOfCov(0.0), _avgCoverage(0.0), _cnfFile(p_cnfFile)
{
	// regex validy is verified while reading the configuration file, see ModelConfiguration
	_regexReq.setPattern(_cnfFile[ModelConfiguration::REQFILE_ATTR_REQREGEX]);

	if (hasCmpRegex())
	{
		_regexCmp.setPattern(_cnfFile[ModelConfiguration::REQFILE_ATTR_CMPREGEX]);
	}

	if (hasCovRegex())
	{
		_regexCov.setPattern(_cnfFile[ModelConfiguration::REQFILE_ATTR_COVREGEX]);
	}

	if (hasStopAfterRegex())
	{
		_regexStopAfter.setPattern(_cnfFile[ModelConfiguration::REQFILE_ATTR_STOPAFTERREGEX]);
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
