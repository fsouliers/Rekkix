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

/*!
 * \brief XML file in the odt zip containing the document data (ie in witch the requirements can be found)
 */
static const char* DOCUMENT_FILE = "content.xml";

static const int ZIP_BUFFER_SIZE = 1048576;  //!< Size of a 1MB array of char
static char ZIP_BUFFER[ZIP_BUFFER_SIZE];  //!< 1MB char array, used to read the document.xml


static const QString ODT_XML_BODY_NODE = "body";  //!< OpenDocument specific XML tag for body
static const QString ODT_XML_TEXT_NODE = "text";  //!< OpenDocument specific XML tag for text
static const QString ODT_XML_PARAGRAPH_NODE = "p";  //!< OpenDocument specific XML tag for paragraph
static const QString ODT_XML_SPAN_NODE = "span";  //!< OpenDocument specific XML tag for text

RequirementFile_odt::RequirementFile_odt(const ModelConfiguration::CnfFileAttributesMap_t& p_cnfFile)
		: IRequirementFile(p_cnfFile)
{
}

RequirementFile_odt::~RequirementFile_odt()
{
}

void RequirementFile_odt::__readTextDataFromZippedFormat(QString& p_textData)
{
	int err;
	struct zip* zipfile = zip_open(_cnfFile[ModelConfiguration::REQFILE_ATTR_PATH].toStdString().c_str(),
	                               0,
	                               &err);
	if (!zipfile)
	{
		qDebug() << "RequirementFile_odt::__readTextDataFromZippedFormat error unzipping file " << _cnfFile[ModelConfiguration::REQFILE_ATTR_PATH];

		AnalysisError e(AnalysisError::ERROR,
		                AnalysisError::PARSING,
		                _cnfFile[ModelConfiguration::REQFILE_ATTR_ID],
		                QObject::trUtf8("Parser ODT : Erreur d'ouverture du fichier %1 (échec de zip_open, erreur %2)").arg(_cnfFile[ModelConfiguration::REQFILE_ATTR_PATH]).arg(err));

		ModelSngAnalysisErrors::instance().addError(e);

		p_textData.clear();
		return;
	}

	int fileindex = zip_name_locate(zipfile, DOCUMENT_FILE, 0);
	if (fileindex < 0)
	{
		qDebug() << "RequirementFile_odt::__readTextDataFromZippedFormat error locating document " << DOCUMENT_FILE;

		AnalysisError e(AnalysisError::ERROR,
		                AnalysisError::PARSING,
		                _cnfFile[ModelConfiguration::REQFILE_ATTR_ID],
		                QObject::trUtf8("Parser ODT : Erreur de localisation de %1 (format de fichier invalide, échec de zip_name_locate)").arg(DOCUMENT_FILE));

		ModelSngAnalysisErrors::instance().addError(e);

		p_textData.clear();
		return;
	}

	struct zip_file* zippeditem = zip_fopen_index(zipfile, fileindex, 0);
	if (!zippeditem)
	{
		qDebug() << "RequirementFile_odt::__readTextDataFromZippedFormat error opening index of zipfile";

		AnalysisError e(AnalysisError::ERROR,
		                AnalysisError::PARSING,
		                _cnfFile[ModelConfiguration::REQFILE_ATTR_ID],
		                QObject::trUtf8("Parser ODT : Erreur de localisation de l'index (format de fichier invalide, échec de zip_fopen_index)"));

		ModelSngAnalysisErrors::instance().addError(e);

		p_textData.clear();
		return;
	}

	// Now all zip stuff is done, the real content can finally be read
	p_textData.clear();
	int zipread;
	memset(ZIP_BUFFER, '\0', ZIP_BUFFER_SIZE);
	while ((zipread = zip_fread(zippeditem, ZIP_BUFFER, ZIP_BUFFER_SIZE), zipread) > 0)
	{
		p_textData.append(ZIP_BUFFER);
		memset(ZIP_BUFFER, '\0', ZIP_BUFFER_SIZE);
	}
	zip_fclose(zippeditem);
	zip_close(zipfile);

	qDebug() << "RequirementFile_odt::__readTextDataFromZippedFormat : File " << _cnfFile[ModelConfiguration::REQFILE_ATTR_PATH] << " properly read";
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

void RequirementFile_odt::__extractDataFromElement(const QDomElement& p_elt, QString& p_dataRead)
{
	// if the element has text or child elements ... it has child nodes
	if (p_elt.hasChildNodes())
	{
		QDomElement eltSpan = p_elt.firstChildElement(ODT_XML_SPAN_NODE) ;
		if (!eltSpan.isNull())
		{
			// Data is in separated «span» elements ... let's aggregate it
			for (; !eltSpan.isNull() ; eltSpan = eltSpan.nextSiblingElement(ODT_XML_SPAN_NODE))
			{
				p_dataRead += eltSpan.text() ;
			}
		}
		else
		{
			// Data is directly mentioned as text in the element
			p_dataRead = p_elt.text() ;
		}
	}
	else
	{
		p_dataRead = "" ;
	}
}


void RequirementFile_odt::parseFile()
{
	// Read raw data from the document and store it into text_content
	QString text_content;
	__readTextDataFromZippedFormat(text_content);
	if (text_content.isEmpty()) return ; // errors have been logged --> nothing to do

	// Now looking to the «text» tag containing the real data that must be analyzed
	QDomDocument mainDoc ;
	mainDoc.setContent(text_content, true);
	QDomElement textRoot ;
	__lookForFirstTextNode(mainDoc.documentElement(), textRoot) ;
	if (textRoot.isNull()) return ; // errors have been logged --> nothing to do

	// Now we can walk through every paragraph and check for requirement stuff in it
	QString current_req = "";
	bool isCurrentReqAcceptable = false;  // just to avoid parsing parts of file if the current requirement isn't correct (eg already defined)
	QDomElement elt = textRoot.firstChildElement(ODT_XML_PARAGRAPH_NODE);
	for (; !elt.isNull(); elt = elt.nextSiblingElement(ODT_XML_PARAGRAPH_NODE))
	{
		qDebug() << "RequirementFile_odt::parseFile looking for data ..." ;
		QString data = "";
		__extractDataFromElement(elt, data) ;
		if (data.isEmpty()) continue ;

		qDebug() << "RequirementFile_odt::parseFile DATA HAVE BEEN EXTRACTED :" << data ;

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
}

