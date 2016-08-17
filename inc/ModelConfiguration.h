/*!
 * \file ModelConfiguration.h
 * \brief Definition of the class ModelConfiguration
 * \date 2016-07-15
 * \author f.souliers
 */

#ifndef MODELCONFIGURATION_H_
#define MODELCONFIGURATION_H_

#include <QAbstractItemModel>

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
	 * \typedef CnfFileAttributesMap_t
	 * \brief Map of the values composing one file configured in a configuration file.
	 *
	 * The map elements are the following:
	 * - key : attribute name that can be accessed (see REQFILE_ATTRS and OUTPUT_ATTRS)
	 * - value : value of the attribute
	 */

	typedef QMap<QString, QString> CnfFileAttributesMap_t;

	/*!
	 * \typedef CnfFileAttributesMapsByFileId_t
	 * \brief Map of the requirement files in a configuration file.
	 *
	 * The map elements are the following:
	 * - key : value of the attribute REQFILE_ATTR_ID (file id)
	 * - value : CnfFileAttributesMap_t for this file
	 */
	typedef QMap<QString, CnfFileAttributesMap_t> CnfFileAttributesMapsByFileId_t;

	/*!
	 * \typedef OutputFiles_t
	 * \brief Vector of the output files.
	 *
	 * Only the attributes names and list differ between an output file and a requirement file.
	 */
	typedef QVector<CnfFileAttributesMap_t> OutputFiles_t;

	/*!
	 * \brief Vector of attributes used to describe a requirement file
	 */
	static const QVector<QString> REQFILE_ATTRS;

	/*!
	 * \brief Vector of headers to be displayed for requirement files attributes.
	 *
	 * This vector must be sorted in the same order as REQFILE_ATTRS
	 */
	static const QVector<QString> REQFILE_HEADER_ATTRS;


	static const QString REQFILE_ATTR_ID;  //!< Attribute name for the file ID (part of the setting name in the *.ini file)
	static const QString REQFILE_ATTR_PATH;  //!< Attribute name for the file path
	static const QString REQFILE_ATTR_PARSER;  //!< Attribute name for the parser that must be used to read the file, see FactoryRequirementFile for available parsers

	static const QString REQFILE_ATTR_REQREGEX;  //!< Attribute name for the regex used to identify a requirement
	static const QString REQFILE_GRPNAME_REQID;  //!< Name of the named group in the regex used to identify a requirement (req_name)
	static const QString REQFILE_GRPNAME_REQLST;  //!< Name of the named group in the regex REQFILE_ATTR_CMPREGEX and REQFILE_ATTR_COVREGEX used to identify requirements lists

	/*!
	 * \brief Attribute name for the regex used to identify the composing requirements list
	 *
	 * This regex must be built in such a way that only the composing requirement list is captured as a
	 * group. In other words, group 1 of the regex must be the list of composing requirements separated by
	 * REQFILE_ATTR_CMPSEPARATOR
	 */
	static const QString REQFILE_ATTR_CMPREGEX;
	static const QString REQFILE_ATTR_CMPSEPARATOR;  //!< Attribute name for the separator character in the composing requirements list

	/*!
	 * \brief Attribute name for the regex used to identify the upstream requirements list (the ones that are covered)
	 *
	 * This regex must be built in such a way that only the composing requirement list is captured as a
	 * group. In other words, group 1 of the regex must be the list of composing requirements separated by
	 * REQFILE_ATTR_COVSEPARATOR
	 */
	static const QString REQFILE_ATTR_COVREGEX;
	static const QString REQFILE_ATTR_COVSEPARATOR;  //!< Attribute name for the separator character in the upstream requirements list
	static const QString REQFILE_ATTR_STOPAFTERREGEX;  //!< Attribute name for the regex used to stop the parsing of the file
	static const QString REQFILE_ATTR_HASDWN;  //!< Attribute name used to specify whether the file must have downstream documents
	static const QString REQFILE_ATTR_HASUP;  //!< Attribute name used to specify whether the file must have upstream documents

	static const QString REQFILE_ATTR_VALUE_YES;  //!< Attribute value Yes for REQFILE_ATTR_HASDWN and REQFILE_ATTR_HASUP
	static const QString REQFILE_ATTR_VALUE_NO;  //!< Attribute value No for REQFILE_ATTR_HASDWN and REQFILE_ATTR_HASUP

	/*!
	 * \brief Vector of attributes used to describe an output file
	 */
	static const QVector<QString> OUTPUT_ATTRS;

	/*!
	 * \brief List of file formats supported by Rekkix  (ie acceptable values for the OUTPUT_ATTR_WRITER attribute)
	 */
	static const QVector<QString> OUTPUT_SUPPORTED_EXT;


	static const QString OUTPUT_ATTR_PATH;  //!< Attribute name for the file path
	static const QString OUTPUT_ATTR_WRITER;  //!< Attribute name for the file format
	static const QString OUTPUT_ATTR_DELIMITER;  //!< Attribute name for the field separator character, in case of csv-like format

	static const QString OUTPUT_ATTR_VALUE_HTML;  //!< Attribute value html for OUTPUT_ATTR_WRITER
	static const QString OUTPUT_ATTR_VALUE_CSV;  //!< Attribute value csv for OUTPUT_ATTR_WRITER
	static const QString OUTPUT_STR_TIMESTAMP;  //!< String in the path attribute value that shall be replaced by the timestamp


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
	 * \return The map of the configured requirement files (see CnfFileAttributesMapsByFileId_t)
	 */
	CnfFileAttributesMapsByFileId_t getConfiguredRequirementFiles() const
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
	/*!
	 * \brief Map of files containing requirements (see CnfFileAttributesMapsByFileId_t)
	 */
	CnfFileAttributesMapsByFileId_t __reqFiles;

	/*!
	 * \brief Vector of the output files (see OutputFiles_t)
	 */
	OutputFiles_t __outputFiles;

	/*!
	 * \brief Checks the consistency of the attributes of a given requirement file
	 *
	 * The following rules are checked:
	 * - the ID is mandatory
	 * - the path is mandatory and must target an existing file
	 * - the req_regex is mandatory
	 * - if cmp_regex mentioned, then cmp_separator is mandatory
	 * - if cov_regex mentioned, then cov_separator is mandatory
	 *
	 * \param[in] configured_file    attributes map of the file that must be verified
	 * \param[inout] errModel        used to store every configuration error, so it can be displayed
	 * \return
	 * If the errModel parameter has been modified, then returns true; else returns false.
	 */
	bool __hasAReqFileConsistecyError(const CnfFileAttributesMap_t& configured_file,
	                                  ModelConfigurationErrorsRef errModel);

	/*!
	 * \brief Checks the consistency of the attributes of a given output file
	 *
	 * The following rules are checked:
	 * - the path is mandatory
	 * - the writer is mandatory and must be a supported format
	 * - if the writer is csv, then the delimiter is mandatory
	 *
	 * \param[in] output_file  attributes map of the file that must be verified
	 * \param[inout] errModel    used to store every configuration error, so it can be displayed
	 * \return
	 * If the errModel parameter has been modified, then returns true; else returns false.
	 */
	bool __hasAnOutputFileConsistecyError(const CnfFileAttributesMap_t& output_file,
	                                      ModelConfigurationErrorsRef errModel);

	/*!
	 * \brief Reads the <i>files</i> section of the configuration file
	 * \warning The existence and the possibility to open the configuration file must have been
	 *          checked before calling this method
	 * \param[in] configFilePath  Path to the ini configuration file
	 * \param[inout] errModel     used to store every configuration error, so it can be displayed
	 */
	void __readSectionFiles(const QString& configFilePath, ModelConfigurationErrorsRef errModel);

	/*!
	 * \brief Reads the <i>outputs</i> section of the configuration file
	 * \warning The existence and the possibility to open the configuration file must have been
	 *          checked before calling this method
	 * \param[in] configFilePath  Path to the ini configuration file
	 * \param[inout] errModel     used to store every configuration error, so it can be displayed
	 */
	void __readSectionOutputs(const QString& configFilePath, ModelConfigurationErrorsRef errModel);
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
