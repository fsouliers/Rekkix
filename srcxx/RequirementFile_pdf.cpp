/*!
 * \file RequirementFile_pdf.cpp
 * \brief Implementation of the class RequirementFile_pdf
 * \date 2016-08-23
 * \author f.souliers
 */

#include <QDebug>
#include <QFile>

#include <poppler-document.h>
#include <poppler-page.h>
#include <poppler-global.h>

#include "AnalysisError.h"
#include "ModelSngAnalysisErrors.h"
#include "RequirementFile_pdf.h"
#include "Requirement.h"


RequirementFile_pdf::RequirementFile_pdf(const ModelConfiguration::CnfFileAttributesMap_t& p_cnfFile)
		: IRequirementFile(p_cnfFile)
{
}

RequirementFile_pdf::~RequirementFile_pdf()
{
}

void RequirementFile_pdf::parseFile()
{
	poppler::document* pdfDoc = poppler::document::load_from_file(_cnfFile[ModelConfiguration::REQFILE_ATTR_PATH].toStdString()) ;

	delete(pdfDoc) ;
}

