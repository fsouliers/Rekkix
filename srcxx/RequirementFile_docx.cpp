/*!
 * \file RequirementFile_docx.cpp
 * \brief Implementation of the class RequirementFile_docx
 * \date 2016-07-21
 * \author f.souliers
 */

#include <QDebug>
#include <QXmlStreamReader>

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

static const QString DOCX_XML_PARAGRAPH_NODE = "p";  //!< MS Word specific XML tag for paragraph
static const QString DOCX_XML_TEXT_NODE = "t";  //!< MS Word specific XML tag for text

RequirementFile_docx::RequirementFile_docx(const ModelConfiguration::XmlConfiguredFileAttributesMap_t& p_cnfFile)
		: IRequirementFile(p_cnfFile)
{
}

RequirementFile_docx::~RequirementFile_docx()
{
}

void RequirementFile_docx::__readTextDataFromZippedFormat(QString& p_textData)
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

		p_textData.clear();
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

		p_textData.clear();
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

	qDebug() << "RequirementFile_docx::__readTextDataFromZippedFormat : File " << _cnfFile[ModelConfiguration::REQFILE_ATTR_PATH] << " properly read";
}

void RequirementFile_docx::parseFile()
{
	// Read raw data from the document and store it into text_content
	QString text_content;
	__readTextDataFromZippedFormat(text_content);

	// Parse XML stream
	QXmlStreamReader xstr(text_content);
	QString current_req = "";
	bool isCurrentReqAcceptable = false;  // just to avoid parsing parts of file if the current requirement isn't correct (eg already defined)
	while (!xstr.atEnd())
	{
		// just started a new paragraph, it has to be stored into a single string so the regexp can be matched on it
		// ... so just make the whole paragraph a single string
		QString data = "";
		if (xstr.isStartElement() && xstr.name().toString() == DOCX_XML_PARAGRAPH_NODE)
		{
			while (!(xstr.isEndElement() && xstr.name().toString() == DOCX_XML_PARAGRAPH_NODE))
			{
				if (xstr.isStartElement() && xstr.name().toString() == DOCX_XML_TEXT_NODE)
				{
					data += QString::fromUtf8(xstr.readElementText().toStdString().c_str());
				}

				xstr.readNext();
			}
		}

		// If the «requirement definition» regex matches, then nothing more to do with this line
		if (_hasRequirementDefinition(data, current_req, isCurrentReqAcceptable))
		{
			xstr.readNext();
			continue ;
		}

		// If the the «composed of several requirements» regex matches, then nothing more to do with this line
		if (isCurrentReqAcceptable && hasCmpRegex())
		{
			if (_hasExpectedCompositeRequirements(data, current_req))
			{
				xstr.readNext();
				continue ;
			}
		}

		// If the the «covering several requirements» regex matches, then nothing more to do with this line
		if (isCurrentReqAcceptable && hasCovRegex())
		{
			if (_hasExpectedUpstreamRequirements(data, current_req))
			{
				xstr.readNext();
				continue ;
			}
		}

		// if the stopAfter regex is reached, then stop parsing the file
		if (hasStopAfterRegex())
		{
			if (_mustStopParsing(data)) break ;
		}

		xstr.readNext();
	}
}

