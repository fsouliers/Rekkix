/*
 * ModelReqs.cpp
 *
 *  Created on: 1 août 2016
 *      Author: f.souliers
 */

#include <QDebug>

#include "ModelReqs.h"

static const int REQS_COL_ID = 0 ;
static const int REQS_COL_FILE_ID = 1 ;
static const int REQS_COL_IS_COMPOSITE = 2 ;
static const int REQS_COL_COVERAGE = 3 ;

static const QVector<QString> COL_HEADERS { QObject::trUtf8 ("Exigence"),
                                            QObject::trUtf8 ("Définie dans"),
                                            QObject::trUtf8 ("Est composite ?"),
                                            QObject::trUtf8 ("Couverte à") };


ModelReqs::ModelReqs(QObject *parent)
: QAbstractItemModel(parent)
{

}

ModelReqs::~ModelReqs()
{

}

int
ModelReqs::columnCount(const QModelIndex & /*parent*/) const
{
	return(COL_HEADERS.count()) ;
}

int
ModelReqs::rowCount(const QModelIndex & /*parent*/) const
{
	return(__requirements.count()) ;
}


QVariant
ModelReqs::data(const QModelIndex &index, int role) const
{
	QVariant retVal ;

	// The framework is requiring the data to display
	if (role == Qt::DisplayRole)
	{
		Requirement* req = __requirements[index.row()] ;

		switch (index.column())
		{
			case REQS_COL_ID :
				retVal = req->getId() ;
				break;

			case REQS_COL_FILE_ID :
				retVal = req->getLocationId() ;
				break;

			case REQS_COL_IS_COMPOSITE :
				retVal = (req->getComposingRequirements().count()>0 ? QObject::trUtf8 ("Oui") : QObject::trUtf8 ("Non")) ;
				break;

			case REQS_COL_COVERAGE :
				retVal = QString("%1 %").arg(req->getCoverage() * 100) ;
				break;

			default :
				// invalid value
				qDebug() << "ModelReqs::data --> INVALID VALUE" ;
		}

	}

	return(retVal) ;
}


QVariant
ModelReqs::headerData(int section, Qt::Orientation orientation, int role) const
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
ModelReqs::index(int row, int column, const QModelIndex & /*parent*/) const
{
	return(createIndex(row, column)) ;
}


QModelIndex
ModelReqs::parent(const QModelIndex & /*index*/) const
{
	return(QModelIndex()) ;
}


void
ModelReqs::addRequirementsOfAFile(const QVector<Requirement*>& p_reqs)
{
	__requirements.append(p_reqs) ;
}


void
ModelReqs::refresh()
{
	QModelIndex topLeft = index(0, 0);
	QModelIndex bottomRight = index(rowCount() - 1, columnCount() - 1);

	emit dataChanged(topLeft, bottomRight);
	emit layoutChanged();
}
