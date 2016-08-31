/*!
 * \file RequirementFile_odt.cpp
 * \brief Implementation of the class RequirementFile_odt
 * \date 2016-08-17
 * \author f.souliers
 */

#include <QDebug>

#include "zip.h"

#include "ModelSngAnalysisErrors.h"
#include "RequirementFile_odt.h"
#include "Requirement.h"


static const QString ODT_XML_BODY_NODE = "body";  //!< OpenDocument specific XML tag for body
static const QString ODT_XML_TEXT_NODE = "text";  //!< OpenDocument specific XML tag for text
static const QString ODT_XML_PARAGRAPH_NODE = "p";  //!< OpenDocument specific XML tag for paragraph
static const QString ODT_XML_ANNOTATION_NODE = "annotation";  //!< OpenDocument specific XML tag for annotation (side note inside a paragraph)


RequirementFile_odt::RequirementFile_odt(const ModelConfiguration::CnfFileAttributesMap_t& p_cnfFile)
		: RequirementFileAbstractZipped(p_cnfFile)
{
	_documentFileName = "content.xml" ;
}

RequirementFile_odt::~RequirementFile_odt()
{
}

void RequirementFile_odt::__lookForFirstTextNode(const QDomElement& p_firstNode, QDomElement& p_textNode)
{
	p_textNode = QDomElement() ; // by default, the element is null and the method fails

	QDomElement bodyNode = p_firstNode.firstChildElement(ODT_XML_BODY_NODE) ;
	if (bodyNode.isNull())
	{
		AnalysisError e(AnalysisError::ERROR,
		                AnalysisError::PARSING,
		                _cnfFile[ModelConfiguration::REQFILE_ATTR_ID],
		                QObject::trUtf8("Parser ODT : nœud %1 non trouvé").arg(ODT_XML_BODY_NODE));

		ModelSngAnalysisErrors::instance().addError(e);
	}
	else
	{
		QDomElement textNode = bodyNode.firstChildElement(ODT_XML_TEXT_NODE) ;
		if (textNode.isNull())
		{
			AnalysisError e(AnalysisError::ERROR,
			                AnalysisError::PARSING,
			                _cnfFile[ModelConfiguration::REQFILE_ATTR_ID],
			                QObject::trUtf8("Parser ODT : nœud %1 non trouvé").arg(ODT_XML_TEXT_NODE));

			ModelSngAnalysisErrors::instance().addError(e);
		}
		else
		{
			p_textNode = textNode ;
		}
	}

}

void RequirementFile_odt::parseFile()
{
	// Read raw data from the document and store it into text_content
	QString* text_content = new QString();
	_readTextDataFromZippedFormat(text_content);
	if (text_content->isEmpty())
	{
		delete(text_content) ;
		return ; // errors have been logged --> nothing to do
	}

	// Now open the XML doc and look for the «text» tag containing the real data that must be analyzed
	QDomDocument* mainDoc = new QDomDocument();
	QString errMsg = "" ;
	int errLine = 0 ;
	int errCol = 0 ;
	if (!mainDoc->setContent(*text_content, true, &errMsg, &errLine, &errCol))
	{
		AnalysisError e(AnalysisError::ERROR,
		                AnalysisError::PARSING,
		                _cnfFile[ModelConfiguration::REQFILE_ATTR_ID],
		                QObject::trUtf8("Parser ODT : erreur setContent ligne<%1> col<%2> msg<%3>").arg(errLine).arg(errCol).arg(errMsg));

		ModelSngAnalysisErrors::instance().addError(e);

		delete(text_content) ;
		delete(mainDoc) ;
		return ;
	}

	QDomElement* textRoot = new QDomElement();
	__lookForFirstTextNode(mainDoc->documentElement(), *textRoot) ;
	if (textRoot->isNull())
	{
		delete (textRoot) ;
		delete(text_content) ;
		delete(mainDoc) ;
		return ;
	}

	// Now we can walk through every paragraph and check for requirement stuff in it
	QString current_req = "";
	bool isCurrentReqAcceptable = false;  // just to avoid parsing parts of file if the current requirement isn't correct (eg already defined)
	QDomElement elt = textRoot->firstChildElement(ODT_XML_PARAGRAPH_NODE);
	for (; !elt.isNull(); elt = elt.nextSiblingElement(ODT_XML_PARAGRAPH_NODE))
	{
		QDomElement eltAnnotation = elt.firstChildElement(ODT_XML_ANNOTATION_NODE) ;
		QString data = elt.text() ;

		// if there is an annotation in the middle of the paragraph text, remove it so the paragraph
		// can be really used
		if (!eltAnnotation.isNull()) data.remove(eltAnnotation.text()) ;

		// If there is no data ... no need to look for anything
		if (data.isEmpty()) continue ;

		// If the «requirement definition» regex matches, then nothing more to do with this paragraph
		if (_hasStoredAnyRequirementDefinition(data, current_req, isCurrentReqAcceptable)) continue ;

		// If the the «composed of several requirements» regex matches, then nothing more to do with this paragraph
		if (isCurrentReqAcceptable && hasCmpRegex())
		{
			if (_hasStoredAnyExpectedCompositeRequirements(data, current_req)) continue ;
		}

		// If the the «covering several requirements» regex matches, then nothing more to do with this line
		if (isCurrentReqAcceptable && hasCovRegex())
		{
			if (_hasStoredAnyExpectedUpstreamRequirements(data, current_req)) continue ;
		}

		// if the stopAfter regex is reached, then stop parsing the file
		if (hasStopAfterRegex())
		{
			if (_mustStopParsing(data)) break ;
		}
	}

	delete (textRoot) ;
	delete(mainDoc) ;
	delete(text_content) ;
}

