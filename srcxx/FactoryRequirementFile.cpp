/*
 * FactoryRequirementFile.cpp
 *
 *  Created on: 21 juil. 2016
 *      Author: f.souliers
 */

#include "FactoryRequirementFile.h"
#include "RequirementFile_docx.h"

static const QString EXT_DOCX = "docx" ;
//static const QString EXT_PDF = "pdf" ;


FactoryRequirementFile::FactoryRequirementFile()
{

}

FactoryRequirementFile::~FactoryRequirementFile()
{
}

IRequirementFilePtr
FactoryRequirementFile::getRequirementFile(ModelConfiguration::XmlAttributesMap_t& p_cnfFile)
{

	QString filename = p_cnfFile[ModelConfiguration::REQFILE_ATTR_PATH] ;
	if (filename.endsWith(EXT_DOCX, Qt::CaseInsensitive)) return(new RequirementFile_docx(p_cnfFile)) ;

	// TODO : implémenter la lecture des autres formats de fichier

	return(NULL) ;
}
