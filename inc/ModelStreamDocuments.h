/*!
 * \file ModelStreamDocuments.h
 * \brief Definition of the class ModelStreamDocuments
 * \date 2016-07-29
 * \author f.souliers
 */

#ifndef MODELSTREAMDOCUMENTS_H_
#define MODELSTREAMDOCUMENTS_H_

#include <QAbstractItemModel>

#include "IRequirementFile.h"

/*!
 * \class ModelStreamDocuments
 * \brief Model (see Qt Framework and MVC) used to display upstream or downstream documents of a selected one
 */
class ModelStreamDocuments : public QAbstractItemModel
{
Q_OBJECT

public:

	/*!
	 * \brief Constructor of the model, does nothing special
	 * \param[in]  parent   Not used
	 */
	ModelStreamDocuments(QObject *parent = Q_NULLPTR);

	/*!
	 * \brief Destructor of the model, only ensures to close the configuration file if needed
	 */
	virtual ~ModelStreamDocuments();

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
	 * \see ModelStreamDocuments.cpp for the details of the columns
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
	 * \see ModelStreamDocuments.cpp for the details of the columns
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
	 * \brief add a document set (upstream or downstream) to the current one to be displayed
	 * \param[in] p_data  document set to be displayed
	 */
	void addDocumentSet(const QMap<QString, IRequirementFile*>& p_data);

	/*!
	 * \brief empty __streamDocumentSet, mainly because new ones are going to be displayed
	 */
	void clear()
	{
		__streamDocumentSet.clear();
	}

	/*!
	 * \brief Ask for a refresh of the model so the UI is updated
	 *
	 * this method emits the dataChanged and layoutChanged signals so the Qt framework updates the graphical
	 * view associated to this model.
	 */
	void refresh();

private:

	/*!
	 * Map of the requirement files representing the upstream or the downstream of
	 * one or more files
	 */
	QMap<QString, IRequirementFile*> __streamDocumentSet;
};

#endif /* MODELSTREAMDOCUMENTS_H_ */
