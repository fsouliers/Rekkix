/*!
 * \file ModelConfiguration.cpp
 * \brief Implementation of the class ModelConfiguration
 * \date 2016-07-15
 * \author f.souliers
 */


#include <QDebug>
#include <QFile>
#include <QDir>
#include <QXmlStreamReader>
#include <QXmlStreamAttribute>
#include <QFileInfo>
#include <QRegularExpression>

#include "ModelConfiguration.h"

/*!
 * \brief XML node starting the definition of a requirement file
 */
static const QString XML_NODE_FILE("file");

/*!
 * \brief XML node starting the definition of an output file
 */
static const QString XML_NODE_OUTPUT("output");

const QString ModelConfiguration::REQFILE_ATTR_ID("id");
const QString ModelConfiguration::REQFILE_ATTR_PATH("path");
const QString ModelConfiguration::REQFILE_ATTR_REQREGEX("req_regex");
const QString ModelConfiguration::REQFILE_ATTR_CMPREGEX("cmp_regex");
const QString ModelConfiguration::REQFILE_ATTR_CMPSEPARATOR("cmp_separator");
const QString ModelConfiguration::REQFILE_ATTR_COVREGEX("cov_regex");
const QString ModelConfiguration::REQFILE_ATTR_COVSEPARATOR("cov_separator");
const QString ModelConfiguration::REQFILE_ATTR_STOPAFTERREGEX("stopafter_regex");
const QString ModelConfiguration::REQFILE_ATTR_HASDWN("has_downstream");
const QString ModelConfiguration::REQFILE_ATTR_HASUP("has_upstream");
const QString ModelConfiguration::REQFILE_ATTR_VALUE_YES("y");
const QString ModelConfiguration::REQFILE_ATTR_VALUE_NO("n");

const QString ModelConfiguration::OUTPUT_ATTR_PATH("path");
const QString ModelConfiguration::OUTPUT_ATTR_WRITER("writer");
const QString ModelConfiguration::OUTPUT_ATTR_DELIMITER("delimiter");
const QString ModelConfiguration::OUTPUT_ATTR_VALUE_HTML("html");
const QString ModelConfiguration::OUTPUT_ATTR_VALUE_CSV("csv");
const QString ModelConfiguration::OUTPUT_STR_TIMESTAMP("REKKIXTIMESTAMP");

/*!
 * \brief Vector of XML attributes that has to be visible in the gui
 *
 * The order is important as it must be the same as the order of ModelConfiguration::XML_HEADER_ATTRS
 */
const QVector<QString> ModelConfiguration::XML_ATTRS { REQFILE_ATTR_ID,
                                                       REQFILE_ATTR_PATH,
                                                       REQFILE_ATTR_HASDWN,
                                                       REQFILE_ATTR_HASUP,
                                                       REQFILE_ATTR_REQREGEX,
                                                       REQFILE_ATTR_CMPREGEX,
                                                       REQFILE_ATTR_CMPSEPARATOR,
                                                       REQFILE_ATTR_COVREGEX,
                                                       REQFILE_ATTR_COVSEPARATOR,
                                                       REQFILE_ATTR_STOPAFTERREGEX };

/*!
 * \brief Vector of headers for XML attributes that has to be visible in the gui
 *
 * The order is important as it must be the same as the order of ModelConfiguration::XML_ATTRS
 */
const QVector<QString> ModelConfiguration::XML_HEADER_ATTRS { QObject::trUtf8("ID"),
                                                              QObject::trUtf8("Fichier"),
                                                              QObject::trUtf8("Est couvert ?"),
                                                              QObject::trUtf8("Est couvrant ?"),
                                                              QObject::trUtf8("Regex Exigence"),
                                                              QObject::trUtf8("Regex Composition"),
                                                              QObject::trUtf8("Séparateur Composition"),
                                                              QObject::trUtf8("Regex Couverture"),
                                                              QObject::trUtf8("Séparateur Couverture"),
                                                              QObject::trUtf8("Regex de fin d'analyse") };

/*!
 * \brief Vector of XML attributes for the output documents, currently not displayed in the gui
 */
const QVector<QString> ModelConfiguration::XML_OUTPUT_ATTRS { OUTPUT_ATTR_PATH,
                                                              OUTPUT_ATTR_WRITER,
                                                              OUTPUT_ATTR_DELIMITER };

/*!
 * \brief Vector of the supported output file formats
 */
const QVector<QString> ModelConfiguration::OUTPUT_SUPPORTED_EXT { OUTPUT_ATTR_VALUE_HTML,
                                                                  OUTPUT_ATTR_VALUE_CSV };

ModelConfiguration::~ModelConfiguration()
{
	if (__configFile.isOpen()) __configFile.close();
}

ModelConfiguration::ModelConfiguration(QObject *parent)
		: QAbstractItemModel(parent)
{

}

int ModelConfiguration::columnCount(const QModelIndex & /*parent*/) const
{
	return (XML_ATTRS.count());
}

int ModelConfiguration::rowCount(const QModelIndex & /*parent*/) const
{
	return (__reqFiles.count());
}

QVariant ModelConfiguration::data(const QModelIndex &index, int role) const
{
	QVariant def_value;

	// The framework is requiring the data to display
	if (role == Qt::DisplayRole)
	{
		QString xml_id = XML_ATTRS[index.column()];
		QString file_id = __reqFiles.keys().at(index.row());
		return (__reqFiles[file_id][xml_id]);
	}

	return (def_value);
}

QVariant ModelConfiguration::headerData(int section, Qt::Orientation orientation, int role) const
{
	if (role == Qt::DisplayRole)
	{
		if (orientation == Qt::Horizontal)
		{
			return (XML_HEADER_ATTRS[section]);
		}
		else if (orientation == Qt::Vertical)
		{
			return (section + 1);
		}
	}

	return (QAbstractItemModel::headerData(section, orientation, role));
}

QModelIndex ModelConfiguration::index(int row, int column, const QModelIndex & /*parent*/) const
{
	return (createIndex(row, column));
}

QModelIndex ModelConfiguration::parent(const QModelIndex & /*index*/) const
{
	return (QModelIndex());
}

void ModelConfiguration::setFile(const char* p_filepath, ModelConfigurationErrorsRef errModel)
{
	// Clear possible previous data & errors
	__reqFiles.clear();
	__outputFiles.clear();
	errModel.clear();

	// then load the new file...
	qDebug() << "ModelConfiguration::setFile - Ouverture du fichier " << p_filepath;
	if (__configFile.isOpen()) __configFile.close();

	__configFile.setFileName(p_filepath);
	if (!__configFile.open(QIODevice::ReadOnly))
	{
		ModelConfigurationErrors::error_t e;
		e.severity = ModelConfigurationErrors::CRITICAL;
		e.category = ModelConfigurationErrors::FILESYSTEM;
		e.description = QObject::trUtf8("Impossible d'ouvrir le fichier %1").arg(p_filepath);

		errModel.addError(e);

		return;
	}

	// ... and parse it
	QXmlStreamReader xstr(&__configFile);
	while (!xstr.atEnd())
	{
		if (xstr.isStartElement())
		{
			if (xstr.name().toString() == XML_NODE_FILE)
			{
				// Parsing a configured file node
				qDebug() << "--------------------------------------------------";
				qDebug() << "ModelConfiguration::setFile : New configured file ";

				// Append a file to the list of configured files
				QMap<QString,QString> newItem;

				foreach(QString s, XML_ATTRS)
				{
					// If the attribute does not exist, the «value» method returns an empty string
					// Concerning the REQFILE_ATTR_PATH attribute, the basedir of the configuration file
					// must be added if the path is a relative one.

					newItem[s] = xstr.attributes().value(s).toString();// the correct value by default
					if (s == REQFILE_ATTR_PATH)
					{
						QFileInfo fi(xstr.attributes().value(s).toString());

						if (fi.isRelative())
						{
							// just override the previously stored value in this particular case
							QFileInfo ficnf(__configFile);
							newItem[s] = ficnf.absoluteDir().absolutePath() + "/" + xstr.attributes().value(s).toString();
						}
					}

					qDebug() << "ModelConfiguration::setFile : reading config[" << s << "] =" << newItem[s];
				}

				if (!__hasAReqFileConsistecyError(newItem, errModel))
				{
					qDebug() << "ModelConfiguration::setFile : File description is consistent --> adding the file to the list of configured files";
					__reqFiles.insert(newItem[REQFILE_ATTR_ID], newItem);
				}
			}
			else if (xstr.name().toString() == XML_NODE_OUTPUT)
			{
				// Parsing a configured output node
				qDebug() << "---------------------------------------------------";
				qDebug() << "ModelConfiguration::setFile : New configured output ";

				// Append a file to the list of configured outputs
				QMap<QString,QString> newItem;

				foreach(QString s, XML_OUTPUT_ATTRS)
				{
					// If the attribute does not exist, the «value» method returns an empty string
					// Concerning the OUTPUT_ATTR_PATH attribute, the basedir of the configuration file
					// must be added if the path is a relative one.

					newItem[s] = xstr.attributes().value(s).toString();// the correct value by default
					if (s == OUTPUT_ATTR_PATH)
					{
						QFileInfo fi(xstr.attributes().value(s).toString());

						if (fi.isRelative())
						{
							// just override the previously stored value in this particular case
							QFileInfo ficnf(__configFile);
							newItem[s] = ficnf.absoluteDir().absolutePath() + "/" + xstr.attributes().value(s).toString();
						}
					}

					qDebug() << "ModelConfiguration::setFile : reading output config[" << s << "] =" << newItem[s];
				}

				if (!__hasAnOutputFileConsistecyError(newItem, errModel))
				{
					qDebug() << "ModelConfiguration::setFile : Output File is consistent --> adding the file to the list of output files";
					__outputFiles.append(newItem);
				}
			}
		}

		xstr.readNext();
	}

	// Pure XML parsing error has been detected ?
	if (xstr.hasError())
	{
		ModelConfigurationErrors::error_t e;
		e.severity = ModelConfigurationErrors::WARNING;
		e.category = ModelConfigurationErrors::PARSING;
		e.description = QObject::trUtf8("Erreurs XML (la configuration peut être fausse) : %1").arg(xstr.errorString());

		errModel.addError(e);
	}

	// Request graphical update of the data
	QModelIndex topLeft = index(0, 0);
	QModelIndex bottomRight = index(rowCount() - 1, columnCount() - 1);

	emit dataChanged(topLeft, bottomRight);
	emit layoutChanged();

	errModel.refresh();
}

bool ModelConfiguration::__hasAReqFileConsistecyError(const XmlConfiguredFileAttributesMap_t& configured_file,
                                                      ModelConfigurationErrorsRef errModel)
{
	bool retVal = true;  // errors by default
	QVector<ModelConfigurationErrors::error_t> errors;
	ModelConfigurationErrors::error_t e;
	QRegularExpression regex;

	QString file_id;

	// Checking the id & path attributes
	if (configured_file[REQFILE_ATTR_ID].isEmpty())
	{
		qDebug() << "ModelConfiguration::__hasAReqFileConsistecyErrors attribut id non trouvé ...";

		file_id = QObject::trUtf8("Non identifié");

		if (configured_file[REQFILE_ATTR_PATH].isEmpty())
		{
			qDebug() << "ModelConfiguration::__hasAReqFileConsistecyErrors ... et attribut path non trouvé non plus";
			e.severity = ModelConfigurationErrors::ERROR;
			e.category = ModelConfigurationErrors::CONTENT;
			e.description = QObject::trUtf8("Attributs %1 et %2 manquants").arg(REQFILE_ATTR_ID).arg(REQFILE_ATTR_PATH);
			errors.append(e);
		}
		else
		{
			qDebug() << "ModelConfiguration::__hasAReqFileConsistecyErrors ... mais attribut path trouvé";
			e.severity = ModelConfigurationErrors::ERROR;
			e.category = ModelConfigurationErrors::CONTENT;
			e.description = QObject::trUtf8("Attribut %1 manquant pour le fichier %2").arg(REQFILE_ATTR_ID).arg(configured_file[REQFILE_ATTR_PATH]);
			errors.append(e);

			QFileInfo fi(configured_file[REQFILE_ATTR_PATH]);
			if (!fi.exists())
			{
				qDebug() << "ModelConfiguration::__hasAReqFileConsistecyErrors ... et malgré tout le fichier n'existe pas" << configured_file[REQFILE_ATTR_PATH];
				e.severity = ModelConfigurationErrors::ERROR;
				e.category = ModelConfigurationErrors::FILESYSTEM;
				e.description = QObject::trUtf8("Le fichier %1 n'existe pas").arg(configured_file[REQFILE_ATTR_PATH]);
				errors.append(e);
			}
		}
	}
	else
	{
		file_id = configured_file[REQFILE_ATTR_ID];
		qDebug() << "ModelConfiguration::__hasAReqFileConsistecyErrors attribut id trouvé " << file_id;

		if (configured_file[REQFILE_ATTR_PATH].isEmpty())
		{
			qDebug() << "ModelConfiguration::__hasAReqFileConsistecyErrors ... mais pas path";
			e.severity = ModelConfigurationErrors::ERROR;
			e.category = ModelConfigurationErrors::CONTENT;
			e.description = QObject::trUtf8("Attribut %1 manquant pour le fichier %2").arg(REQFILE_ATTR_PATH).arg(file_id);

			errors.append(e);
		}
		else
		{
			// the path must be defined
			qDebug() << "ModelConfiguration::__hasAReqFileConsistecyErrors ... et path trouvé";
			QFileInfo fi(configured_file[REQFILE_ATTR_PATH]);
			if (!fi.exists())
			{
				qDebug() << "ModelConfiguration::__hasAReqFileConsistecyErrors ... mais le fichier n'existe pas " << configured_file[REQFILE_ATTR_PATH];
				e.severity = ModelConfigurationErrors::ERROR;
				e.category = ModelConfigurationErrors::FILESYSTEM;
				e.description = QObject::trUtf8("Le fichier %1 n'existe pas").arg(configured_file[REQFILE_ATTR_PATH]);
				errors.append(e);
			}

			// And in any case, the ID cannot exist more than once
			if (__reqFiles.contains(file_id))
			{
				e.severity = ModelConfigurationErrors::ERROR;
				e.category = ModelConfigurationErrors::CONTENT;
				e.description = QObject::trUtf8("Identifiant %1 défini plusieurs fois").arg(file_id);
				errors.append(e);
			}
		}
	}

	// Checking the req_regex attribute
	if (configured_file[REQFILE_ATTR_REQREGEX].isEmpty())
	{
		qDebug() << "ModelConfiguration::__hasAReqFileConsistecyErrors req_regex non trouvé";
		e.severity = ModelConfigurationErrors::ERROR;
		e.category = ModelConfigurationErrors::CONTENT;
		e.description = QObject::trUtf8("Attribut %1 manquant pour le fichier %2").arg(REQFILE_ATTR_REQREGEX).arg(file_id);
		errors.append(e);
	}
	else
	{
		regex.setPattern(configured_file[REQFILE_ATTR_REQREGEX]);
		if (!regex.isValid())
		{
			qDebug() << "ModelConfiguration::__hasAReqFileConsistecyErrors req_regex expression invalide";
			e.severity = ModelConfigurationErrors::ERROR;
			e.category = ModelConfigurationErrors::CONTENT;
			e.description = QObject::trUtf8("Fichier %1, regex pour %2 invalide").arg(file_id).arg(REQFILE_ATTR_REQREGEX);
			errors.append(e);
		}
	}

	// Checking the cmp_regex attributes
	if (!configured_file[REQFILE_ATTR_CMPREGEX].isEmpty())
	{
		if (configured_file[REQFILE_ATTR_CMPSEPARATOR].isEmpty())
		{
			e.severity = ModelConfigurationErrors::ERROR;
			e.category = ModelConfigurationErrors::CONTENT;
			e.description = QObject::trUtf8("Fichier %1 : si %2 est mentionné, alors %3 est obligatoire").arg(file_id).arg(REQFILE_ATTR_CMPREGEX).arg(REQFILE_ATTR_CMPSEPARATOR);
			errors.append(e);
		}

		// Moreover check the regex validity
		regex.setPattern(configured_file[REQFILE_ATTR_CMPREGEX]);
		if (!regex.isValid())
		{
			qDebug() << "ModelConfiguration::__hasAReqFileConsistecyErrors cmp_regex expression invalide";
			e.severity = ModelConfigurationErrors::ERROR;
			e.category = ModelConfigurationErrors::CONTENT;
			e.description = QObject::trUtf8("Fichier %1, regex pour %2 invalide").arg(file_id).arg(REQFILE_ATTR_CMPREGEX);
			errors.append(e);
		}
	}
	else
	{
		if (!configured_file[REQFILE_ATTR_CMPSEPARATOR].isEmpty())
		{
			e.severity = ModelConfigurationErrors::WARNING;
			e.category = ModelConfigurationErrors::CONTENT;
			e.description = QObject::trUtf8("Fichier %1 : %2 mentionné, alors que %3 ne l'a pas été").arg(file_id).arg(REQFILE_ATTR_CMPSEPARATOR).arg(REQFILE_ATTR_CMPREGEX);
			errors.append(e);
		}
	}

	// Checking the cov_regex attributes
	if (!configured_file[REQFILE_ATTR_COVREGEX].isEmpty())
	{
		if (configured_file[REQFILE_ATTR_COVSEPARATOR].isEmpty())
		{
			e.severity = ModelConfigurationErrors::ERROR;
			e.category = ModelConfigurationErrors::CONTENT;
			e.description = QObject::trUtf8("Fichier %1 : si %2 est mentionné, alors %3 est obligatoire").arg(file_id).arg(REQFILE_ATTR_COVREGEX).arg(REQFILE_ATTR_COVSEPARATOR);
			errors.append(e);
		}

		// Moreover check the regex validity
		regex.setPattern(configured_file[REQFILE_ATTR_COVREGEX]);
		if (!regex.isValid())
		{
			qDebug() << "ModelConfiguration::__hasAReqFileConsistecyErrors cov_regex expression invalide";
			e.severity = ModelConfigurationErrors::ERROR;
			e.category = ModelConfigurationErrors::CONTENT;
			e.description = QObject::trUtf8("Fichier %1, regex pour %2 invalide").arg(file_id).arg(REQFILE_ATTR_COVREGEX);
			errors.append(e);
		}
	}
	else
	{
		if (!configured_file[REQFILE_ATTR_COVSEPARATOR].isEmpty())
		{
			e.severity = ModelConfigurationErrors::WARNING;
			e.category = ModelConfigurationErrors::CONTENT;
			e.description = QObject::trUtf8("Fichier %1 : %2 mentionné, alors que %3 ne l'a pas été").arg(file_id).arg(REQFILE_ATTR_COVSEPARATOR).arg(REQFILE_ATTR_COVREGEX);
			errors.append(e);
		}
	}

	// checking the stopafter regex
	if (!configured_file[REQFILE_ATTR_STOPAFTERREGEX].isEmpty())
	{
		regex.setPattern(configured_file[REQFILE_ATTR_STOPAFTERREGEX]);
		if (!regex.isValid())
		{
			qDebug() << "ModelConfiguration::__hasAReqFileConsistecyErrors stopafter_regex expression invalide";
			e.severity = ModelConfigurationErrors::ERROR;
			e.category = ModelConfigurationErrors::CONTENT;
			e.description = QObject::trUtf8("Fichier %1, regex pour %2 invalide").arg(file_id).arg(REQFILE_ATTR_STOPAFTERREGEX);
			errors.append(e);
		}
	}

	// compute result
	if (errors.count() > 0)
	{
		errModel.addErrors(errors);
		retVal = true;
	}
	else
	{
		retVal = false;
	}

	return (retVal);
}

bool ModelConfiguration::__hasAnOutputFileConsistecyError(const XmlConfiguredFileAttributesMap_t& configured_file,
                                                          ModelConfigurationErrorsRef errModel)
{
	bool retVal = true;  // errors by default
	ModelConfigurationErrors::error_t e;
	QVector<ModelConfigurationErrors::error_t> errors;

	if (configured_file[OUTPUT_ATTR_PATH].isEmpty())
	{
		qDebug() << "ModelConfiguration::__hasAnOutputFileConsistecyErrors attribut path non trouvé";
		e.severity = ModelConfigurationErrors::ERROR;
		e.category = ModelConfigurationErrors::CONTENT;
		e.description = QObject::trUtf8("Attributs %1 manquant sur un fichier de description de sortie").arg(OUTPUT_ATTR_PATH);
		errors.append(e);
	}

	if (configured_file[OUTPUT_ATTR_WRITER].isEmpty())
	{
		qDebug() << "ModelConfiguration::__hasAnOutputFileConsistecyErrors attribut writer non trouvé";
		e.severity = ModelConfigurationErrors::ERROR;
		e.category = ModelConfigurationErrors::CONTENT;
		e.description = QObject::trUtf8("Attributs %1 manquant sur un fichier de description de sortie").arg(OUTPUT_ATTR_WRITER);
		errors.append(e);
	}
	else
	{
		if (!OUTPUT_SUPPORTED_EXT.contains(configured_file[OUTPUT_ATTR_WRITER]))
		{
			qDebug() << "ModelConfiguration::__hasAnOutputFileConsistecyErrors writer non supporté";
			e.severity = ModelConfigurationErrors::ERROR;
			e.category = ModelConfigurationErrors::CONTENT;
			e.description = QObject::trUtf8("Le %1 spécifié suivant n'est pas supporté : %2").arg(OUTPUT_ATTR_WRITER).arg(configured_file[OUTPUT_ATTR_WRITER]);
			errors.append(e);
		}
	}

	if (configured_file[OUTPUT_ATTR_WRITER] == OUTPUT_ATTR_VALUE_CSV && configured_file[OUTPUT_ATTR_DELIMITER].isEmpty())
	{
		e.severity = ModelConfigurationErrors::WARNING;
		e.category = ModelConfigurationErrors::CONTENT;
		e.description = QObject::trUtf8("Le délimiteur n'est pas spécifié sur une sortie de type %1").arg(OUTPUT_ATTR_VALUE_CSV);
		errors.append(e);
	}

	// compute result
	if (errors.count() > 0)
	{
		errModel.addErrors(errors);
		retVal = true;
	}
	else
	{
		retVal = false;
	}

	return (retVal);
}
