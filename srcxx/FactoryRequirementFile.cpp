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

FactoryRequirementFile::FactoryRequirementFile()
{

}

FactoryRequirementFile::~FactoryRequirementFile()
{
}

IRequirementFilePtr FactoryRequirementFile::getRequirementFile(ModelConfiguration::XmlConfiguredFileAttributesMap_t& p_cnfFile)
{

	QString filename = p_cnfFile[ModelConfiguration::REQFILE_ATTR_PATH];
	if (filename.endsWith(EXT_DOCX, Qt::CaseInsensitive)) return (new RequirementFile_docx(p_cnfFile));

	// TODO : implement other file formats + add an XML attribute to force a parser

	return (NULL);
}
