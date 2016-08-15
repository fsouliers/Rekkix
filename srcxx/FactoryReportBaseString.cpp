/*!
 * \file FactoryReportBaseString.cpp
 * \brief Implementation of the class FactoryReportBaseString
 * \date 2016-08-07
 * \author f.souliers
 */

#include <QFile>

#include "FactoryReportBaseString.h"

FactoryReportBaseString::FactoryReportBaseString()
{
}

FactoryReportBaseString::~FactoryReportBaseString()
{
}

QString FactoryReportBaseString::getBaseString(const char* p_templateName,
                                               const QString& p_writer,
                                               const QString& p_delimiter) const
{
	QString path;
	path = ":/template_files/" + p_writer + "/Report_" + QString(p_templateName) + "." + p_writer;

	QFile f_base(path);
	f_base.open(QIODevice::ReadOnly | QIODevice::Text);
	QString s_base(f_base.readAll());
	f_base.close();

	s_base.replace("<DELIMITER>", p_delimiter);

	return (s_base);
}

QString FactoryReportBaseString::getCSString() const
{
	QFile f_style(":/template_files/html/HtmlReportStyleSheet.css");
	f_style.open(QIODevice::ReadOnly | QIODevice::Text);
	QString s_style(f_style.readAll());
	f_style.close();

	return (s_style);
}
