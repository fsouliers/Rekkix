/*!
 * \file RequirementFile_txt.cpp
 * \brief Implementation of the class RequirementFile_txt
 * \date 2016-08-15
 * \author f.souliers
 */

#include <QDebug>
#include <QFile>


#include "AnalysisError.h"
#include "ModelSngAnalysisErrors.h"
#include "RequirementFile_txt.h"
#include "Requirement.h"


RequirementFile_txt::RequirementFile_txt(const ModelConfiguration::XmlConfiguredFileAttributesMap_t& p_cnfFile)
		: IRequirementFile(p_cnfFile)
{
}

RequirementFile_txt::~RequirementFile_txt()
{
}

void RequirementFile_txt::parseFile()
{
	// Open the file. In case of error, just log it and return
	QString filePath = _cnfFile[ModelConfiguration::REQFILE_ATTR_PATH] ;
	QFile f(filePath) ;
	if (!f.open(QIODevice::ReadOnly | QIODevice::Text))
	{
		AnalysisError e(AnalysisError::ERROR,
			            AnalysisError::PARSING,
			            filePath,
			            QObject::trUtf8("Impossible d'ouvrir le fichier %1").arg(filePath));

		ModelSngAnalysisErrors::instance().addError(e);
		return;
	}

	// for each line of the file, verify if any regex can detect something
	QString current_req = "";  // current detected requirement (used when regex compose or covers matches)
	bool isCurrentReqAcceptable = false;  // just to avoid parsing parts of file if the current requirement isn't correct (eg already defined)
	while (!f.atEnd())
	{
		QString current_line = f.readLine() ;

		// If the «requirement definition» regex matches, then nothing more to do with this line
		if (_hasRequirementDefinition(current_line, current_req, isCurrentReqAcceptable)) continue ;

		// If the the «composed of several requirements» regex matches, then nothing more to do with this line
		if (isCurrentReqAcceptable && hasCmpRegex())
		{
			if (_hasExpectedCompositeRequirements(current_line, current_req)) continue ;
		}

		// If the the «covering several requirements» regex matches, then nothing more to do with this line
		if (isCurrentReqAcceptable && hasCovRegex())
		{
			if (_hasExpectedUpstreamRequirements(current_line, current_req)) continue ;
		}

		// if the stopAfter regex is reached, then stop parsing the file
		if (hasStopAfterRegex())
		{
			if (_mustStopParsing(current_line)) break ;
		}
	}
}

