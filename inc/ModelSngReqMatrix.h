/*
 * ModelSngReqMatrix.h
 *
 *  Created on: 21 juil. 2016
 *      Author: f.souliers
 */

#ifndef MODELSNGREQMATRIX_H_
#define MODELSNGREQMATRIX_H_

#include <QAbstractItemModel>
#include <QString>
#include <QMap>

#include "Requirement.h"
#include "IRequirementFile.h"

/*!
 * ModelSngReqMatrix is the main matrix containing the information of all requirements; as a
 * consequence, it is also used as the main Model to display coverage computing once it
 * has been performed.
 */
class ModelSngReqMatrix : public QAbstractItemModel
{
	Q_OBJECT

	public:
		static ModelSngReqMatrix& instance()
		{
			static ModelSngReqMatrix r ;
			return(r) ;
		}

		/*!
		 * Add a defined requirement to the global requirement matrix. A defined requirement is a requirement
		 * identified with the req_regex ; on the opposite, an expected requirement is a requirement created as
		 * it has been identified as a reference of another requirement (and maybe not yet defined).
		 * - file_id : file id being parsed in for witch the method has been called
		 *
		 * \returns
		 * - true if the requirement has been properly added
		 * - false if not
		 */
		bool addDefinedRequirement(const QString& file_id, RequirementRef p_r) ;

		/*!
		 * Add an expected requirement as it has been defined as a subelement of another one
		 * - file_id : file id being parsed in for witch the method has been called
		 * - p_parentId : The Id of the requirement composed of at least p_r
		 * - p_r : the requirement which is a sublement of the requirement identified by p_parentId
		 */
		void addExpectedCompositeRequirement(const QString& file_id, QString& p_parentId, RequirementRef p_r) ;

		/*!
		 * Add an expected requirement as it has been defined as covered by the current one
		 * - file_id : file id being parsed in for witch the method has been called
		 * - p_reqId : current requirement that covers p_coveredReq
		 * - p_coveredReq : requirement covered by the current one
		 */
		void addExpectedCoveredRequirement(const QString& file_id, QString& p_reqId, RequirementRef p_coveredReq) ;

		/*!
		 * Add a file containing requirements and parse it
		 */
		void addRequirementFile(IRequirementFilePtr p) ;

		/*!
		 * Compute the coverage values of files once they all have been parsed. At the end of the
		 * computation, the method emits signals to freshen file coverage view
		 */
		void computeCoverage() ;

		int columnCount(const QModelIndex &parent = QModelIndex()) const ;
		int rowCount(const QModelIndex &parent = QModelIndex()) const ;

		QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const ;
		QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const ;

		QModelIndex index(int row, int column, const QModelIndex &parent = QModelIndex()) const ;

		QModelIndex parent(const QModelIndex &index) const ;

		IRequirementFilePtr getRequirementFile(const QString& p_fileID ) const ;

		void clear() ;

		const QString getReportFileSummaryTable(const QString& p_writer, const QString& p_delimiter) const ;
		const QString getReportFileDetails(const QString& p_writer, const QString& p_delimiter) const ;

	private:
		ModelSngReqMatrix(QObject *parent = Q_NULLPTR);
		virtual ~ModelSngReqMatrix();

		QMap<QString, Requirement> __reqsByName ;

		QMap<QString, QVector<Requirement*>> __reqsByFileId ;

		QMap<QString, IRequirementFilePtr> __filesByFileId ;

		const QString __getReportFileDetailsDocCoverage(const QString& file_id,const QString& p_writer, const QString& p_delimiter) const ;
		const QString __getReportFileDetailsReqCoverage(const QString& file_id,const QString& p_writer, const QString& p_delimiter) const ;
		const QString __getReportFileDetailsRevCov(const QString& file_id,const QString& p_writer, const QString& p_delimiter) const ;

};

#endif /* MODELSNGREQMATRIX_H_ */
