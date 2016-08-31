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


static const zip_uint64_t ZIP_BUFFER_SIZE = 262144;  //!< Size of a 256kB array of char

RequirementFileAbstractZipped::RequirementFileAbstractZipped(const ModelConfiguration::CnfFileAttributesMap_t& p_cnfFile)
		: RequirementFileAbstract(p_cnfFile)
{
	_zippedDataBuffer = new char[ZIP_BUFFER_SIZE] ;
}

RequirementFileAbstractZipped::~RequirementFileAbstractZipped()
{
	delete(_zippedDataBuffer) ;
}

void RequirementFileAbstractZipped::_readTextDataFromZippedFormat(QString* p_textData)
{
	int err;
	zip_t* zipfile = zip_open(_cnfFile[ModelConfiguration::REQFILE_ATTR_PATH].toStdString().c_str(),
	                               0,
	                               &err);
	if (!zipfile)
	{
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
		AnalysisError e(AnalysisError::ERROR,
		                AnalysisError::PARSING,
		                _cnfFile[ModelConfiguration::REQFILE_ATTR_ID],
		                QObject::trUtf8("Erreur de localisation de %1 (format de fichier invalide, échec de zip_name_locate)").arg(_documentFileName));

		ModelSngAnalysisErrors::instance().addError(e);

		p_textData->clear();
		return;
	}

	zip_file_t* zippeditem = zip_fopen_index(zipfile, fileindex, 0);
	if (!zippeditem)
	{
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
	int zipread = 1 ; // dumb init, just to enter the while
	memset(_zippedDataBuffer, '\0', ZIP_BUFFER_SIZE);
	int nbCycles = 0 ;
	while (zipread > 0)
	{
		nbCycles++ ;
		zipread = zip_fread(zippeditem, _zippedDataBuffer, ZIP_BUFFER_SIZE-1) ;
		if (zipread == -1)
		{
			zip_error_t* err = zip_get_error(zipfile) ;

			// real error --> nothing to append to the read string
			if (err->zip_err != 0)
			{
				AnalysisError e(AnalysisError::ERROR,
				                AnalysisError::PARSING,
				                _cnfFile[ModelConfiguration::REQFILE_ATTR_ID],
				                QObject::trUtf8("Erreur de lecture du fichier après %1 cycles (zip_fread retourne -1). zip_err %2 : %3").arg(nbCycles).arg(err->zip_err).arg(err->str));

				ModelSngAnalysisErrors::instance().addError(e);
			}
			else
			{
				// last buffer to be taken into account
				p_textData->append(_zippedDataBuffer);
				memset(_zippedDataBuffer, '\0', ZIP_BUFFER_SIZE);
			}

			// In any case, the file is finished
			break ;
		}
		else
		{
			p_textData->append(_zippedDataBuffer);
			memset(_zippedDataBuffer, '\0', ZIP_BUFFER_SIZE);
		}
	}
	zip_fclose(zippeditem);
	zip_close(zipfile);

	qDebug() << "RequirementFileAbstractZipped::_readTextDataFromZippedFormat : File " << _cnfFile[ModelConfiguration::REQFILE_ATTR_PATH] << " properly read";
}

