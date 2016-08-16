/*!
 * \file FactoryRequirementFile.cpp
 * \brief Implementation of the class FactoryRequirementFile
 * \date 2016-07-21
 * \author f.souliers
 */

#include "FactoryRequirementFile.h"
#include "RequirementFile_docx.h"

/*!
 * \brief File extension for docx format
 */
static const QString EXT_DOCX = "docx";

/*!
 * \brief File extension for odt format
 */
//static const QString EXT_ODT = "odt";

/*!
 * \brief File extension for text format
 */
static const QString EXT_TXT = "txt";

/*!
 * \brief File extension for pdf format
 */
//static const QString EXT_PDF = "pdf";


static const QVector<QString> AVAILABLE_PARSERS = {
                                                   EXT_DOCX,
                                                   EXT_TXT
};

FactoryRequirementFile::FactoryRequirementFile()
{

}

FactoryRequirementFile::~FactoryRequirementFile()
{
}

IRequirementFilePtr FactoryRequirementFile::getRequirementFile(const ModelConfiguration::XmlConfiguredFileAttributesMap_t& p_cnfFile)
{
	IRequirementFilePtr retPtr = NULL ;

	QString filename = p_cnfFile[ModelConfiguration::REQFILE_ATTR_PATH];
	QString parser = p_cnfFile[ModelConfiguration::REQFILE_ATTR_PARSER];

	if (AVAILABLE_PARSERS.contains(parser))
	{
		// The required parser is known --> just use it
		if (parser == EXT_DOCX) retPtr = new RequirementFile_docx(p_cnfFile) ;

	}
	else
	{
		// The required parser isn't known or mentionned in the configuration file, let's try to guess it
		if (filename.endsWith(EXT_DOCX, Qt::CaseInsensitive)) retPtr = new RequirementFile_docx(p_cnfFile) ;


	}


	// TODO : implement other file formats

	return (retPtr);
}
