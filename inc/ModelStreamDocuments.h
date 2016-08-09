/*
 * ModelStreamDocuments.h
 *
 *  Created on: 29 juil. 2016
 *      Author: f.souliers
 */

#ifndef MODELSTREAMDOCUMENTS_H_
#define MODELSTREAMDOCUMENTS_H_

#include <QAbstractItemModel>

#include "IRequirementFile.h"

/*!
 * This model is used to display upstream or downstream documents
 */
class ModelStreamDocuments : public QAbstractItemModel
{
	Q_OBJECT

	public:
		ModelStreamDocuments(QObject *parent = Q_NULLPTR);
		virtual ~ModelStreamDocuments();

		int columnCount(const QModelIndex &parent = QModelIndex()) const ;
		int rowCount(const QModelIndex &parent = QModelIndex()) const ;

		QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const ;
		QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const ;

		QModelIndex index(int row, int column, const QModelIndex &parent = QModelIndex()) const ;

		QModelIndex parent(const QModelIndex &index) const ;

		/*!
		 * add a document set (upstream or downstream) to the current one to be displayed
		 */
		void addDocumentSet(const QMap<QString, IRequirementFile*>& p_data) ;

		/*!
		 * empty __streamDocumentSet
		 */
		void clear() {__streamDocumentSet.clear() ;}


		/*!
		 * Ask for a refresh of the model so the UI is updated
		 */
		void refresh() ;

	private :

		/*!
		 * Map of the requirement files representing the upstream or the downstream of
		 * one or more files
		 */
		QMap<QString, IRequirementFile*> __streamDocumentSet ;
};

#endif /* MODELSTREAMDOCUMENTS_H_ */
