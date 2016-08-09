/*
 * ModelReqsCoveredDownstream.cpp
 *
 *  Created on: 29 juil. 2016
 *      Author: f.souliers
 */

#include <QDebug>

#include "ModelReqsCoveredDownstream.h"

static const int REQSCOVDW_COL_ID = 0 ;
static const int REQSCOVDW_COL_FILE_ID = 1 ;
static const int REQSCOVDW_COL_COVERAGE = 2 ;
static const int REQSCOVDW_COL_COVERING_REQ_ID = 3 ;
static const int REQSCOVDW_COL_COVERING_REQ_FILE_ID = 4 ;

static const QVector<QString> COL_HEADERS { QObject::trUtf8 ("Exigence"),
                                            QObject::trUtf8 ("Définie dans"),
                                            QObject::trUtf8 ("Couverte à"),
                                            QObject::trUtf8 ("ou couverte par"),
                                            QObject::trUtf8 ("Définie dans") };


ModelReqsCoveredDownstream::ModelReqsCoveredDownstream(QObject *parent)
: QAbstractItemModel(parent)
{


}

ModelReqsCoveredDownstream::~ModelReqsCoveredDownstream()
{
}

int
ModelReqsCoveredDownstream::columnCount(const QModelIndex & /*parent*/) const
{
	return(COL_HEADERS.count()) ;
}

int
ModelReqsCoveredDownstream::rowCount(const QModelIndex & /*parent*/) const
{
	return(__requirements.count()) ;
}

QVariant
ModelReqsCoveredDownstream::data(const QModelIndex &index, int role) const
{
	QVariant retVal ;

	// The framework is requiring the data to display
	if (role == Qt::DisplayRole)
	{
		QString req_id = __requirements.keys().at(index.row()) ;
		Requirement* req = __requirements[req_id] ;
		Requirement* covering = req->getDownstreamRequirement() ;

		switch (index.column())
		{
			case REQSCOVDW_COL_ID :
				retVal = req_id ;
				break;

			case REQSCOVDW_COL_FILE_ID :
				retVal = req->getLocationId() ;
				break;

			case REQSCOVDW_COL_COVERAGE :
				retVal = QString("%1 %").arg(req->getCoverage() * 100) ;
				break;

			case REQSCOVDW_COL_COVERING_REQ_ID :
				if (covering) retVal = covering->getId() ;
				break;

			case REQSCOVDW_COL_COVERING_REQ_FILE_ID :
				if (covering) retVal = covering->getLocationId() ;
				break;

			default :
				// invalid value
				qDebug() << "ModelReqsCoveredDownstream::data --> INVALID VALUE" ;
		}

	}

	return(retVal) ;
}


QVariant
ModelReqsCoveredDownstream::headerData(int section, Qt::Orientation orientation, int role) const
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
ModelReqsCoveredDownstream::index(int row, int column, const QModelIndex & /*parent*/) const
{
	return(createIndex(row, column)) ;
}


QModelIndex
ModelReqsCoveredDownstream::parent(const QModelIndex & /*index*/) const
{
	return(QModelIndex()) ;
}


void
ModelReqsCoveredDownstream::addRequirementsOfAFile(const QVector<Requirement*>& p_reqs)
{
	// Each requirement can be covered by only one other requirement, but its coverage
	// may be != 0.0 in case of composite requirement
	foreach(Requirement* r, p_reqs)
	{
		Requirement* dwreq = r->getDownstreamRequirement() ;
		double coverage = r->getCoverage() ;
		if (dwreq || coverage > 0)
		{
			__requirements.insert(r->getId(), r) ;
		}
	}
}


void
ModelReqsCoveredDownstream::refresh()
{
	QModelIndex topLeft = index(0, 0);
	QModelIndex bottomRight = index(rowCount() - 1, columnCount() - 1);

	emit dataChanged(topLeft, bottomRight);
	emit layoutChanged();
}

