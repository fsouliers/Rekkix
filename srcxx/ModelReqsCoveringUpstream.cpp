/*
 * ModelReqsCoveringUpstream.cpp
 *
 *  Created on: 29 juil. 2016
 *      Author: f.souliers
 */

#include <QDebug>

#include "ModelReqsCoveringUpstream.h"

static const int REQSCOVUP_COL_ID = 0 ;
static const int REQSCOVUP_COL_FILE_ID = 1 ;
static const int REQSCOVUP_COL_COVERED_REQ_ID = 2 ;
static const int REQSCOVUP_COL_COVERED_REQ_FILE_ID = 3 ;

static const QVector<QString> COL_HEADERS { QObject::trUtf8 ("Exigence"),
                                            QObject::trUtf8 ("Définie dans"),
                                            QObject::trUtf8 ("Couvre"),
                                            QObject::trUtf8 ("Définie dans") };

ModelReqsCoveringUpstream::ModelReqsCoveringUpstream(QObject *parent)
: QAbstractItemModel(parent)
{

}

ModelReqsCoveringUpstream::~ModelReqsCoveringUpstream()
{
}

int
ModelReqsCoveringUpstream::columnCount(const QModelIndex & /*parent*/) const
{
	return(COL_HEADERS.count()) ;
}


int
ModelReqsCoveringUpstream::rowCount(const QModelIndex & /*parent*/) const
{
	return(__requirements.count()) ;
}

QVariant
ModelReqsCoveringUpstream::data(const QModelIndex &index, int role) const
{
	QVariant retVal ;

	// The framework is requiring the data to display
	if (role == Qt::DisplayRole)
	{
		Requirement* covered = __requirements.values().at(index.row()) ;

		switch (index.column())
		{
			case REQSCOVUP_COL_ID :
				retVal = covered->getDownstreamRequirement()->getId() ;
				break;

			case REQSCOVUP_COL_FILE_ID :
				retVal = covered->getDownstreamRequirement()->getLocationId() ;
				break;

			case REQSCOVUP_COL_COVERED_REQ_ID :
				retVal = covered->getId() ;
				break;

			case REQSCOVUP_COL_COVERED_REQ_FILE_ID :
				retVal = covered->getLocationId() ;
				break;

			default :
				// invalid value
				qDebug() << "ModelReqsCoveringUpstream::data --> INVALID VALUE" ;
		}
	}

	return(retVal) ;
}

QVariant
ModelReqsCoveringUpstream::headerData(int section, Qt::Orientation orientation, int role) const
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
ModelReqsCoveringUpstream::index(int row, int column, const QModelIndex & /*parent*/) const
{
	return(createIndex(row, column)) ;
}


QModelIndex
ModelReqsCoveringUpstream::parent(const QModelIndex & /*index*/) const
{
	return(QModelIndex()) ;
}


void
ModelReqsCoveringUpstream::addRequirementsOfAFile(const QVector<Requirement*>& p_reqs)
{
	// Each requirement may covers several other requirements, so the model must
	// display several lines with the same requirement_id
	foreach(Requirement* r, p_reqs)
	{
		QVector<Requirement*> upreqs = r->getUpstreamRequirements() ;
		if (upreqs.count() > 0)
		{
			foreach(Requirement* coveredreq, upreqs)
			{
				__requirements.insertMulti(r->getId(), coveredreq) ;
			}
		}
	}
}


void
ModelReqsCoveringUpstream::refresh()
{
	QModelIndex topLeft = index(0, 0);
	QModelIndex bottomRight = index(rowCount() - 1, columnCount() - 1);

	emit dataChanged(topLeft, bottomRight);
	emit layoutChanged();
}
