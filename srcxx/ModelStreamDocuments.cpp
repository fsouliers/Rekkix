/*
 * ModelStreamDocuments.cpp
 *
 *  Created on: 29 juil. 2016
 *      Author: f.souliers
 */

#include "ModelStreamDocuments.h"


static const int STREAMDOC_COL_ID = 0 ;

static const QVector<QString> COL_HEADERS { QObject::trUtf8 ("Fichier") };

ModelStreamDocuments::ModelStreamDocuments(QObject *parent)
: QAbstractItemModel(parent)
{

}

ModelStreamDocuments::~ModelStreamDocuments()
{
}

int
ModelStreamDocuments::columnCount(const QModelIndex & /*parent*/) const
{
	// There is only two columns in this model : the file id (key of the map __streamDocumentSet)
	return(COL_HEADERS.count()) ;
}


int
ModelStreamDocuments::rowCount(const QModelIndex & /*parent*/) const
{
	return(__streamDocumentSet.count()) ;
}

QVariant
ModelStreamDocuments::data(const QModelIndex &index, int role) const
{
	QVariant retVal ;

	// The framework is requiring the data to display
	if (role == Qt::DisplayRole && index.column() == STREAMDOC_COL_ID)
	{
		retVal = __streamDocumentSet.keys().at(index.row()) ;
	}

	return(retVal) ;
}

QVariant
ModelStreamDocuments::headerData(int section, Qt::Orientation orientation, int role) const
{
	if (role == Qt::DisplayRole)
	{
		if (orientation == Qt::Horizontal)
		{
			return(COL_HEADERS[section]);
		}
		else if (orientation == Qt::Vertical)
		{
			return(section+1);
		}
	}

	return(QAbstractItemModel::headerData(section, orientation, role)) ;
}


QModelIndex
ModelStreamDocuments::index(int row, int column, const QModelIndex & /*parent*/) const
{
	return(createIndex(row, column)) ;
}


QModelIndex
ModelStreamDocuments::parent(const QModelIndex & /*index*/) const
{
	return(QModelIndex()) ;
}


void
ModelStreamDocuments::addDocumentSet(const QMap<QString, IRequirementFile*>& p_data)
{
	QMap<QString, IRequirementFile*>::const_iterator it ;

	for (it = p_data.constBegin() ; it != p_data.constEnd() ; ++it)
	{
		if (! __streamDocumentSet.contains(it.key())) __streamDocumentSet.insert(it.key(), it.value()) ;
	}
}


void
ModelStreamDocuments::refresh()
{
	QModelIndex topLeft = index(0, 0);
	QModelIndex bottomRight = index(rowCount() - 1, columnCount() - 1);

	emit dataChanged(topLeft, bottomRight);
	emit layoutChanged();
}
