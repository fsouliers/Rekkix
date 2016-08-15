/*!
 * \file ModelConfiguration.h
 * \brief Definition of the class ModelConfiguration
 * \date 2016-07-15
 * \author f.souliers
 */

#ifndef MODELCONFIGURATION_H_
#define MODELCONFIGURATION_H_

#include <QAbstractItemModel>
#include <QFile>

#include "ModelConfigurationErrors.h"

/*!
 * \class ModelConfiguration
 * \brief Abstraction layer used to read the configuration of a traceability matrix
 *
 * This class is used to access a configuration file setting up the parameters for one traceability matrix,
 * which is totally different of the application settings. The data read by ModelConfiguration is intended to
 * be displayed through a QTableView for the <i>files</i> part of the configuration file; the
 * outputs parts of it is accessed more directly as it is not displayed using a Qt data model.
 */
class ModelConfiguration : public QAbstractItemModel
{
Q_OBJECT

public:
	/*!
	 * \typedef XmlConfiguredFileAttributesMap_t
	 * \brief Map of the attributes composing one file configured in the XML.
	 *
	 * The map elements are the following:
	 * - key : attribute name that can be accessed (see XML_ATTRS and XML_OUTPUT_ATTRS)
	 * - value : value of the attribute
	 */

	typedef QMap<QString, QString> XmlConfiguredFileAttributesMap_t;

	/*!
	 * \typedef ConfiguredFileMapByFileId_t
	 * \brief Map of the configured files in the XML.
	 *
	 * The map elements are the following:
	 * - key : value of the attribute REQFILE_ATTR_ID (file id)
	 * - value : XmlConfiguredFileAttributesMap_t for this file
	 */
	typedef QMap<QString, XmlConfiguredFileAttributesMap_t> ConfiguredFileMapByFileId_t;

	/*!
	 * \typedef OutputFiles_t
	 * \brief Vector of the output files.
	 *
	 * Only the XML attributes differ between an output file and a requirement file.
	 */
	typedef QVector<XmlConfiguredFileAttributesMap_t> OutputFiles_t;

	/*!
	 * \brief Vector of XML attributes used to describe a requirement file
	 */
	static const QVector<QString> XML_ATTRS;

	/*!
	 * \brief Vector of headers to be displayed for XML attributes.
	 *
	 * This vector must be sorted in the same order as XML_ATTRS
	 */
	static const QVector<QString> XML_HEADER_ATTRS;


	static const QString REQFILE_ATTR_ID;  //!< XML Attribute name for the file ID
	static const QString REQFILE_ATTR_PATH;  //!< XML Attribute name for the file path
	static const QString REQFILE_ATTR_REQREGEX;  //!< XML Attribute name for the regex used to identify a requirement
	static const QString REQFILE_ATTR_CMPREGEX;  //!< XML Attribute name for tthe regex used to identify the composing requirements list
	static const QString REQFILE_ATTR_CMPSEPARATOR;  //!< XML Attribute name for the separator character in the composing requirements list
	static const QString REQFILE_ATTR_COVREGEX;  //!< XML Attribute name for tthe regex used to identify the upstream requirements list (the ones that are covered)
	static const QString REQFILE_ATTR_COVSEPARATOR;  //!< XML Attribute name for the separator character in the upstream requirements list
	static const QString REQFILE_ATTR_STOPAFTERREGEX;  //!< XML Attribute name for the regex used to stop the parsing of the file
	static const QString REQFILE_ATTR_HASDWN;  //!< XML Attribute name used to specify whether the file must have downstream documents
	static const QString REQFILE_ATTR_HASUP;  //!< XML Attribute name used to specify whether the file must have upstream documents

	static const QString REQFILE_ATTR_VALUE_YES;  //!< XML attribute value Yes for REQFILE_ATTR_HASDWN and REQFILE_ATTR_HASUP
	static const QString REQFILE_ATTR_VALUE_NO;  //!< XML attribute value No for REQFILE_ATTR_HASDWN and REQFILE_ATTR_HASUP

	/*!
	 * \brief Vector of XML attributes used to describe an output file
	 */
	static const QVector<QString> XML_OUTPUT_ATTRS;

	/*!
	 * \brief List of file formats supported by Rekkix  (ie acceptable values for the OUTPUT_ATTR_WRITER attribute)
	 */
	static const QVector<QString> OUTPUT_SUPPORTED_EXT;


	static const QString OUTPUT_ATTR_PATH;  //!< XML Attribute name for the file path
	static const QString OUTPUT_ATTR_WRITER;  //!< XML Attribute name for the file format
	static const QString OUTPUT_ATTR_DELIMITER;  //!< XML Attribute name for the field separator character, in case of csv-like format

	static const QString OUTPUT_ATTR_VALUE_HTML;  //!< XML Attribute value html for OUTPUT_ATTR_WRITER
	static const QString OUTPUT_ATTR_VALUE_CSV;  //!< XML Attribute value csv for OUTPUT_ATTR_WRITER
	static const QString OUTPUT_STR_TIMESTAMP;  //!< String in the path attribute value that shall be replaced by the timetamp


	/*!
	 * \brief Constructor of the model, does nothing special
	 * \param[in]  parent   Not used
	 */
	ModelConfiguration(QObject *parent = Q_NULLPTR);

	/*!
	 * \brief Destructor of the model, only ensures to close the configuration file if needed
	 */
	virtual ~ModelConfiguration();

	/**
	 * \brief Initialize the model with the data contained in a given configuration file
	 *
	 * Only the <i>files</i> part of the configuration file is accessible as a model. The reason is that
	 * this data must be displayed in the gui as a table.
	 * Other parts of the configuration file are accessed through classical getters and setters.
	 * \warning Concerning the path of the files :
	 * - if they are absolute path, they are stored as-is
	 * - if they are relative path, they are considered to be relative to the configuration file
	 *
	 * \param[out] errModel used to store every configuration error, so it can be displayed
	 */
	void setFile(const char* p_filename, ModelConfigurationErrorsRef errModel);

	/*!
	 * \brief Column counter for the model
	 * \see Qt documentation about implementing subclasses of QAbstractItemModel
	 * \param[in] parent  Required by the framework, but not used here
	 * \return
	 * The number of columns the model has
	 */
	int columnCount(const QModelIndex &parent = QModelIndex()) const;

	/*!
	 * \brief Row counter for the model
	 * \see Qt documentation about implementing subclasses of QAbstractItemModel
	 * \param[in] parent  Required by the framework, but not used here
	 * \return
	 * The number of rows the model has
	 */
	int rowCount(const QModelIndex &parent = QModelIndex()) const;

	/*!
	 * \brief Getter for data in the model
	 * \param[in] index  Model index (that is row & column) for which the data is required
	 * \param[in] role   Role for which the data is required (see Qt documentation)
	 * \see ModelConfiguration.cpp for the details of the columns
	 * \return
	 * The required data
	 */
	QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;

	/*!
	 * \brief Getter for header data in the model
	 *
	 * The approach is totally the same as for the data method, but here only for columns & row headers
	 * \param[in] index       Model index (that is row & column) for which the header data is required
	 * \param[in] orientation Orientation for which the data is required
	 * \param[in] role        Role for which the data is required (see Qt documentation)
	 * \see ModelConfiguration.cpp for the details of the columns
	 * \return
	 * The required data
	 */
	QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;

	/*!
	 * \brief Index builder for a given set of row, column & parent
	 * \param[in] row    row number that must be used to create the index
	 * \param[in] column column number that must be used to create the index
	 * \param[in] parent not used in this implementation
	 * \return
	 * A new created index object
	 */
	QModelIndex index(int row, int column, const QModelIndex &parent = QModelIndex()) const;

	/*!
	 * \brief Parent getter for a given index
	 * \param[in] index  Not used in this implementation
	 * \return
	 * An empty index
	 */
	QModelIndex parent(const QModelIndex &index) const;

	/*!
	 * \brief Getter for the configured requirement files
	 * \return The map of the configured requirement files (see ConfiguredFileMapByFileId_t)
	 */
	ConfiguredFileMapByFileId_t getConfiguredRequirementFiles() const
	{
		return (__reqFiles);
	}

	/*!
	 * \brief Getter for the configured output files
	 * \return The vector of the configured output files (see OutputFiles_t)
	 */
	OutputFiles_t getOutputFiles() const
	{
		return (__outputFiles);
	}

private:
	QFile __configFile;  //!< The configuration file itself

	/*!
	 * \brief Map of files containing requirements (see ConfiguredFileMapByFileId_t)
	 */
	ConfiguredFileMapByFileId_t __reqFiles;

	/*!
	 * \brief Vector of the output files (see OutputFiles_t)
	 */
	OutputFiles_t __outputFiles;

	/*!
	 * \brief Checks the consistency of the xml attributes of a given requirement file
	 *
	 * The following rules are checked:
	 * - the ID is mandatory
	 * - the path is mandatory and must target an existing file
	 * - the req_regex is mandatory
	 * - if cmp_regex mentioned, then cmp_separator is mandatory
	 * - if cov_regex mentioned, then cov_separator is mandatory
	 *
	 * \param[in] configured_file  XML attributes map of the file that must be verified
	 * \param[out] errModel        used to store every configuration error, so it can be displayed
	 * \return
	 * If the errModel parameter has been modified, then returns true; else returns false.
	 */
	bool __hasAReqFileConsistecyError(const XmlConfiguredFileAttributesMap_t& configured_file,
	                                  ModelConfigurationErrorsRef errModel);

	/*!
	 * \brief Checks the consistency of the xml attributes of a given output file
	 *
	 * The following rules are checked:
	 * - the path is mandatory
	 * - the writer is mandatory and must be a supported format
	 * - if the writer is csv, then the delimiter is mandatory
	 *
	 * \param[in] output_file  XML attributes map of the file that must be verified
	 * \param[out] errModel    used to store every configuration error, so it can be displayed
	 * \return
	 * If the errModel parameter has been modified, then returns true; else returns false.
	 */
	bool __hasAnOutputFileConsistecyError(const XmlConfiguredFileAttributesMap_t& output_file,
	                                      ModelConfigurationErrorsRef errModel);

};

/*!
 * \typedef ModelConfigurationRef
 * \brief Just to make the code easier to type when a reference of an ModelConfiguration is required
 */
typedef ModelConfiguration& ModelConfigurationRef;

/*!
 * \typedef ModelConfigurationPtr
 * \brief Just to make the code easier to type when a pointer to ModelConfiguration is required
 */
typedef ModelConfiguration* ModelConfigurationPtr;

#endif /* MODELCONFIGURATION_H_ */
