/*!
 * \file RequirementFileAbstractZipped.cpp
 * \brief Implementation of the class RequirementFileAbstractZipped
 * \date 2016-08-26
 * \author f.souliers
 */

#include <QDebug>

#include "zip.h"

#include "ModelSngAnalysisErrors.h"
#include "RequirementFileAbstractZipped.h"


static const int ZIP_BUFFER_SIZE = 1048576;  //!< Size of a 1MB array of char
static char ZIP_BUFFER[ZIP_BUFFER_SIZE];  //!< 1MB char array, used to read the document.xml


RequirementFileAbstractZipped::RequirementFileAbstractZipped(const ModelConfiguration::CnfFileAttributesMap_t& p_cnfFile)
		: RequirementFileAbstract(p_cnfFile)
{
}

RequirementFileAbstractZipped::~RequirementFileAbstractZipped()
{
}

void RequirementFileAbstractZipped::_readTextDataFromZippedFormat(QString* p_textData)
{
	int err;
	struct zip* zipfile = zip_open(_cnfFile[ModelConfiguration::REQFILE_ATTR_PATH].toStdString().c_str(),
	                               0,
	                               &err);
	if (!zipfile)
	{
		qDebug() << "RequirementFileAbstractZipped::_readTextDataFromZippedFormat error unzipping file " << _cnfFile[ModelConfiguration::REQFILE_ATTR_PATH];

		AnalysisError e(AnalysisError::ERROR,
		                AnalysisError::PARSING,
		                _cnfFile[ModelConfiguration::REQFILE_ATTR_ID],
		                QObject::trUtf8("Erreur d'ouverture du fichier %1 (échec de zip_open, erreur %2)").arg(_cnfFile[ModelConfiguration::REQFILE_ATTR_PATH]).arg(err));

		ModelSngAnalysisErrors::instance().addError(e);

		p_textData->clear();
		return;
	}

	int fileindex = zip_name_locate(zipfile, _documentFileName.toStdString().c_str(), 0);
	if (fileindex < 0)
	{
		qDebug() << "RequirementFileAbstractZipped::_readTextDataFromZippedFormat error locating document " << _documentFileName;

		AnalysisError e(AnalysisError::ERROR,
		                AnalysisError::PARSING,
		                _cnfFile[ModelConfiguration::REQFILE_ATTR_ID],
		                QObject::trUtf8("Erreur de localisation de %1 (format de fichier invalide, échec de zip_name_locate)").arg(_documentFileName));

		ModelSngAnalysisErrors::instance().addError(e);

		p_textData->clear();
		return;
	}

	struct zip_file* zippeditem = zip_fopen_index(zipfile, fileindex, 0);
	if (!zippeditem)
	{
		qDebug() << "RequirementFileAbstractZipped::_readTextDataFromZippedFormat error opening index of zipfile";

		AnalysisError e(AnalysisError::ERROR,
		                AnalysisError::PARSING,
		                _cnfFile[ModelConfiguration::REQFILE_ATTR_ID],
		                QObject::trUtf8("Erreur de localisation de l'index (format de fichier invalide, échec de zip_fopen_index)"));

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

	qDebug() << "RequirementFileAbstractZipped::_readTextDataFromZippedFormat : File " << _cnfFile[ModelConfiguration::REQFILE_ATTR_PATH] << " properly read";
}

