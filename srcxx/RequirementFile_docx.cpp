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

/*!
 * \brief XML file in the docx zip containing the document data (ie in witch the requirements can be found)
 */
static const char* DOCUMENT_FILE = "word/document.xml";

static const int ZIP_BUFFER_SIZE = 1048576;  //!< Size of a 1MB array of char
static char ZIP_BUFFER[ZIP_BUFFER_SIZE];  //!< 1MB char array, used to read the document.xml

static const QString DOCX_XML_BODY_NODE = "body";  //!< MS Word specific XML tag for body
static const QString DOCX_XML_PARAGRAPH_NODE = "p";  //!< MS Word specific XML tag for paragraph

RequirementFile_docx::RequirementFile_docx(const ModelConfiguration::CnfFileAttributesMap_t& p_cnfFile)
		: IRequirementFile(p_cnfFile)
{
}

RequirementFile_docx::~RequirementFile_docx()
{
}

void RequirementFile_docx::__readTextDataFromZippedFormat(QString* p_textData)
{
	int err;
	struct zip* zipfile = zip_open(_cnfFile[ModelConfiguration::REQFILE_ATTR_PATH].toStdString().c_str(),
	                               0,
	                               &err);
	if (!zipfile)
	{
		qDebug() << "RequirementFile_docx::__readTextDataFromZippedFormat error unzipping file " << _cnfFile[ModelConfiguration::REQFILE_ATTR_PATH];

		AnalysisError e(AnalysisError::ERROR,
		                AnalysisError::PARSING,
		                _cnfFile[ModelConfiguration::REQFILE_ATTR_ID],
		                QObject::trUtf8("Parser DOCX : Erreur d'ouverture du fichier %1 (échec de zip_open, erreur %2)").arg(_cnfFile[ModelConfiguration::REQFILE_ATTR_PATH]).arg(err));

		ModelSngAnalysisErrors::instance().addError(e);

		p_textData->clear();
		return;
	}

	int fileindex = zip_name_locate(zipfile, DOCUMENT_FILE, 0);
	if (fileindex < 0)
	{
		qDebug() << "RequirementFile_docx::__readTextDataFromZippedFormat error locating document " << DOCUMENT_FILE;

		AnalysisError e(AnalysisError::ERROR,
		                AnalysisError::PARSING,
		                _cnfFile[ModelConfiguration::REQFILE_ATTR_ID],
		                QObject::trUtf8("Parser DOCX : Erreur de localisation de %1 (format de fichier invalide, échec de zip_name_locate)").arg(DOCUMENT_FILE));

		ModelSngAnalysisErrors::instance().addError(e);

		p_textData->clear();
		return;
	}

	struct zip_file* zippeditem = zip_fopen_index(zipfile, fileindex, 0);
	if (!zippeditem)
	{
		qDebug() << "RequirementFile_docx::__readTextDataFromZippedFormat error opening index of zipfile";

		AnalysisError e(AnalysisError::ERROR,
		                AnalysisError::PARSING,
		                _cnfFile[ModelConfiguration::REQFILE_ATTR_ID],
		                QObject::trUtf8("Parser DOCX : Erreur de localisation de l'index (format de fichier invalide, échec de zip_fopen_index)"));

		ModelSngAnalysisErrors::instance().addError(e);

		p_textData->clear();
		return;
	}

	// Now all zip stuff is done, the real content can finally be read
	p_textData->clear();
	int zipread;
	memset(ZIP_BUFFER, '\0', ZIP_BUFFER_SIZE);
	while ((zipread = zip_fread(zippeditem, ZIP_BUFFER, ZIP_BUFFER_SIZE), zipread) > 0)
	{
		p_textData->append(ZIP_BUFFER);
		memset(ZIP_BUFFER, '\0', ZIP_BUFFER_SIZE);
	}
	zip_fclose(zippeditem);
	zip_close(zipfile);

	qDebug() << "RequirementFile_docx::__readTextDataFromZippedFormat : File " << _cnfFile[ModelConfiguration::REQFILE_ATTR_PATH] << " properly read";
}

void RequirementFile_docx::parseFile()
{
	// Read raw data from the document and store it into text_content
	QString* text_content = new QString();
	__readTextDataFromZippedFormat(text_content);
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
		qDebug() << "RequirementFile_docx::parseFile looking for data ..." ;
		QString data = elt.text();
		if (data.isEmpty()) continue ;

		qDebug() << "RequirementFile_docx::parseFile DATA HAVE BEEN EXTRACTED :" << data ;

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

