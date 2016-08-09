/*
 * ModelConfiguration.h
 *
 *  Created on: 15 juil. 2016
 *      Author: f.souliers
 */

#ifndef MODELCONFIGURATION_H_
#define MODELCONFIGURATION_H_

#include <QAbstractItemModel>
#include <QFile>

#include "ModelConfigurationErrors.h"

/**
 * This class is used to access a configuration file. it is intended to be
 * displayed through a QTableView for the files part of the configuration file ; the
 * outputs parts of it is accessed more directly as it is not displayed using data model.
 */
class ModelConfiguration : public QAbstractItemModel
{
	Q_OBJECT

public:
	/*!
	 * Map of the attributes composing a file configured in the XML :
	 * - key : attribute name that can be accessed (see XML_ATTRS and XML_OUTPUT_ATTRS)
	 * - value : value of the attribute
	 */

	typedef QMap<QString, QString> XmlAttributesMap_t ;

	/*!
	 * Map of the configured files in the XML :
	 * - key : value of the attribute REQFILE_ATTR_ID (file id)
	 * - value : ConfiguredFileMap of this file
	 */
	typedef QMap<QString,XmlAttributesMap_t> ConfiguredFileMapByFileId_t ;

	/*!
	 * Vector of the output files
	 */
	typedef QVector<XmlAttributesMap_t> OutputFiles_t ;


	static const QVector<QString> XML_ATTRS ; //<! Vector of XML attributes used to describe a requirement file
	static const QVector<QString> XML_HEADER_ATTRS ; //<! Vector of headers to be displayed for XML attributes --> must be sorted in the same order as XML_ATTRS
	static const QString REQFILE_ATTR_ID ;
	static const QString REQFILE_ATTR_PATH ;
	static const QString REQFILE_ATTR_REQREGEX ;
	static const QString REQFILE_ATTR_CMPREGEX ;
	static const QString REQFILE_ATTR_CMPSEPARATOR ;
	static const QString REQFILE_ATTR_COVREGEX ;
	static const QString REQFILE_ATTR_COVSEPARATOR ;
	static const QString REQFILE_ATTR_STOPAFTERREGEX ;
	static const QString REQFILE_ATTR_HASDWN ;
	static const QString REQFILE_ATTR_HASUP ;
	static const QString REQFILE_ATTR_VALUE_YES ;
	static const QString REQFILE_ATTR_VALUE_NO ;

	static const QVector<QString> XML_OUTPUT_ATTRS ; //<! Vector of XML attributes used to describe an output file
	static const QVector<QString> OUTPUT_SUPPORTED_EXT ; //<! Acceptable output file format
	static const QString OUTPUT_ATTR_PATH ;
	static const QString OUTPUT_ATTR_WRITER ;
	static const QString OUTPUT_ATTR_DELIMITER ;
	static const QString OUTPUT_ATTR_VALUE_HTML ;
	static const QString OUTPUT_ATTR_VALUE_CSV ;
	static const QString OUTPUT_STR_TIMESTAMP ;


	ModelConfiguration(QObject *parent = Q_NULLPTR) ;
	virtual ~ModelConfiguration();

	/**
	 * @brief Initialize the model with the data contained in a given configuration file
	 *
	 * Only the «files» part of the configuration file is accessible as a model. The reason is that
	 * this data must be displayed in the gui as a table.
	 * Other parts of the configuration file are accessed through classical getters and setters.
	 * Concerning the path of the files :
	 * - if they are absolute path, they are stored as-is
	 * - if they are relative path, they are considered to be relative to the configuration file
	 *
	 * the param errModel is used to store & display every configuration error
	 */
	void setFile(const char* p_filename, ModelConfigurationErrorsRef errModel) ;


	int columnCount(const QModelIndex &parent = QModelIndex()) const ;
	int rowCount(const QModelIndex &parent = QModelIndex()) const ;

	QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const ;
	QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const ;

	QModelIndex index(int row, int column, const QModelIndex &parent = QModelIndex()) const ;

	QModelIndex parent(const QModelIndex &index) const ;

	/*!
	 * Returns the map of the configured requirement files (key is the file id)
	 *
	 */
	ConfiguredFileMapByFileId_t getConfiguredRequirementFiles() const { return(__reqFiles) ;}


	OutputFiles_t getOutputFiles() const { return(__outputFiles) ;}

private:
	QFile __configFile ; //!< The configuration file itself

	/**
	 * Map of files containing requirements (see ConfiguredFileMapByFileId_t)
	 */
	ConfiguredFileMapByFileId_t __reqFiles ;

	/**
	 * Vector of the output files (each item of the vector is a map of XML attributes)
	 */
	OutputFiles_t __outputFiles ;

	/*!
	 * Checks the consistency of the xml attributes of a given file :
	 * - the ID is mandatory
	 * - the path is mandatory and must target an existing file
	 * - the req_regex is mandatory
	 * - if cmp_regex mentioned, then cmp_separator is mandatory
	 * - if cov_regex mentioned, then cov_separator is mandatory
	 *
	 * if the __errors attribute has been modified, then returns true ;
	 * else returns false.
	 */
	bool __hasAReqFileConsistecyErrors(const XmlAttributesMap_t& configured_file, ModelConfigurationErrorsRef errModel) ;

	bool __hasAnOutputFileConsistecyErrors(const XmlAttributesMap_t& output_file, ModelConfigurationErrorsRef errModel) ;

};

typedef ModelConfiguration& ModelConfigurationRef ;

typedef ModelConfiguration* ModelConfigurationPtr ;

#endif /* MODELCONFIGURATION_H_ */
