/*!
 * \file RequirementFile_docx.cpp
 * \brief Implementation of the class RequirementFile_docx
 * \date 2016-07-21
 * \author f.souliers
 */

#include <QDebug>
#include <QDomElement>

#include "zip.h"

#include "ModelSngAnalysisErrors.h"
#include "RequirementFile_docx.h"
#include "Requirement.h"

static const QString DOCX_XML_BODY_NODE = "body";  //!< MS Word specific XML tag for body
static const QString DOCX_XML_PARAGRAPH_NODE = "p";  //!< MS Word specific XML tag for paragraph
static const QString DOCX_XML_DELETED_ELEMENT_NODE = "del";  //!< MS Word specific XML tag for deleted stuff

RequirementFile_docx::RequirementFile_docx(const ModelConfiguration::CnfFileAttributesMap_t& p_cnfFile)
		: RequirementFileAbstractZipped(p_cnfFile)
{
	_documentFileName = "word/document.xml" ;
}

RequirementFile_docx::~RequirementFile_docx()
{
}

void RequirementFile_docx::parseFile()
{
	// Read raw data from the document and store it into text_content
	QString* text_content = new QString();
	_readTextDataFromZippedFormat(text_content);
	if (text_content->isEmpty())
	{
		delete(text_content) ;
		return ; // errors have been logged --> nothing to do
	}

	// Now looking to the «text» tag containing the real data that must be analyzed
	QDomDocument* mainDoc = new QDomDocument();
	QString errMsg = "" ;
	int errLine = 0 ;
	int errCol = 0 ;
	if (!mainDoc->setContent(*text_content, true, &errMsg, &errLine, &errCol))
	{
		AnalysisError e(AnalysisError::ERROR,
		                AnalysisError::PARSING,
		                _cnfFile[ModelConfiguration::REQFILE_ATTR_ID],
		                QObject::trUtf8("Parser DOCX : erreur setContent ligne<%1> col<%2> msg<%3>").arg(errLine).arg(errCol).arg(errMsg));

		ModelSngAnalysisErrors::instance().addError(e);

		delete(text_content) ;
		delete(mainDoc) ;
		return ;
	}

	QDomElement* textRoot = new QDomElement() ;
	*textRoot = mainDoc->documentElement().firstChildElement(DOCX_XML_BODY_NODE);
	if (textRoot->isNull())
	{
		AnalysisError e(AnalysisError::ERROR,
		                AnalysisError::PARSING,
		                _cnfFile[ModelConfiguration::REQFILE_ATTR_ID],
		                QObject::trUtf8("Parser DOCX : Impossible de trouver le nœud %1").arg(DOCX_XML_BODY_NODE));

		ModelSngAnalysisErrors::instance().addError(e);

		delete (textRoot) ;
		delete (text_content) ;
		delete (mainDoc) ;
		return ;
	}

	// Now we can walk through every paragraph and check for requirement stuff in it
	QString current_req = "";
	bool isCurrentReqAcceptable = false;  // just to avoid parsing parts of file if the current requirement isn't correct (eg already defined)
	QDomElement elt = textRoot->firstChildElement(DOCX_XML_PARAGRAPH_NODE);
	for (; !elt.isNull(); elt = elt.nextSiblingElement(DOCX_XML_PARAGRAPH_NODE))
	{
		/*
		 * Data cannot be retrieved with a simple direct "QString data = elt.text() ;" as the paragraph node can
		 * contains deleted stuff that would be recognized as normal text. --> Text data must be retrieved ignoring
		 * deleted text (that means nodes for deleted stuff must be removed before calling the "text()" method on the element.
		 */
		for (QDomNode n = elt.firstChild() ; !n.isNull() ; n = n.nextSibling())
		{
			if (n.isElement())
			{
				QDomElement e = n.toElement();
				if (e.tagName() == DOCX_XML_DELETED_ELEMENT_NODE)
				{
					elt.removeChild(e) ;
				}
			}
		}
		QString data = elt.text() ;

		// If, after analysis, the data string is empty ... nothing to do ...
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

