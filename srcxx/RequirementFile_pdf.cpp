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
	QString filePath = _cnfFile[ModelConfiguration::REQFILE_ATTR_PATH] ;
	poppler::document* pdfDoc = poppler::document::load_from_file(filePath.toStdString()) ;
	if (!pdfDoc)
	{
		AnalysisError e(AnalysisError::ERROR,
			            AnalysisError::PARSING,
			            filePath,
			            QObject::trUtf8("Impossible d'ouvrir le fichier %1").arg(filePath));

		ModelSngAnalysisErrors::instance().addError(e);

		delete(pdfDoc) ;
		return;
	}

	QString current_req = "";  // current detected requirement (used when regex compose or covers matches)
	bool isCurrentReqAcceptable = false;  // just to avoid parsing parts of file if the current requirement isn't correct (eg already defined)
	QString txtDoc ;
	QStringList* lst = new QStringList() ;

	qDebug() << "Nb pages : " << pdfDoc->pages() ;
	for (int pgNum = 0 ; pgNum < pdfDoc->pages() ; ++pgNum)
	{
		txtDoc = "" ;
		poppler::byte_array characters = pdfDoc->create_page(pgNum)->text().to_utf8() ;
		for (poppler::byte_array::iterator c = characters.begin() ; c != characters.end() ; ++c)
		{
			txtDoc += *c ;
		}
		qDebug() << "txtDoc lu :" << txtDoc ;

		lst->append(txtDoc.split('\n')) ;
	}

	foreach(QString current_line, *lst)
	{
		// If the «requirement definition» regex matches, then nothing more to do with this line
		if (_hasStoredAnyRequirementDefinition(current_line, current_req, isCurrentReqAcceptable)) continue ;

		// If the the «composed of several requirements» regex matches, then nothing more to do with this line
		if (isCurrentReqAcceptable && hasCmpRegex())
		{
			if (_hasStoredAnyExpectedCompositeRequirements(current_line, current_req)) continue ;
		}

		// If the the «covering several requirements» regex matches, then nothing more to do with this line
		if (isCurrentReqAcceptable && hasCovRegex())
		{
			if (_hasStoredAnyExpectedUpstreamRequirements(current_line, current_req)) continue ;
		}

		// if the stopAfter regex is reached, then stop parsing the file
		if (hasStopAfterRegex())
		{
			if (_mustStopParsing(current_line)) break ;
		}
	}


	delete(lst) ;
	delete(pdfDoc) ;
}

