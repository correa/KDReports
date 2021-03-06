/****************************************************************************
** Copyright (C) 2007-2020 Klaralvdalens Datakonsult AB.  All rights reserved.
**
** This file is part of the KD Reports library.
**
** Licensees holding valid commercial KD Reports licenses may use this file in
** accordance with the KD Reports Commercial License Agreement provided with
** the Software.
**
**
** This file may be distributed and/or modified under the terms of the
** GNU Lesser General Public License version 2.1 and version 3 as published by the
** Free Software Foundation and appearing in the file LICENSE.LGPL.txt included.
**
** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
**
** Contact info@kdab.com if any conditions of this licensing are not
** clear to you.
**
**********************************************************************/

#include <QApplication>
#include <QAbstractTableModel>
#include <QDebug>

#include <KDReports>
#include <QPrintDialog>
#include <QMessageBox>
#include <QFile>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlTableModel>

int main( int argc, char** argv ) {
    QApplication app( argc, argv );

    // Create a report
    KDReports::Report report;

    // open a DB connection to an in-memory database
    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName(":memory:");
    if( !db.open() ) {
        QMessageBox::critical(0, QObject::tr("Cannot open database"),
                              QObject::tr("Cannot create connection to the requested database. Your Qt is probably lacking the QSQLITE driver. Please check your Qt installation." ), QMessageBox::Cancel );
        return -1;
    }

    // fill the DB with some test data
    QSqlQuery query;
    query.exec("create table airlines (id int primary key, "
               "name varchar(20), homecountry varchar(2))");
    query.exec("insert into airlines values(1, 'Lufthansa', 'DE')");
    query.exec("insert into airlines values(2, 'SAS', 'SE')");
    query.exec("insert into airlines values(3, 'United', 'US')");
    query.exec("insert into airlines values(4, 'KLM', 'NL')");
    query.exec("insert into airlines values(5, 'Aeroflot', 'RU')");

    // Create a QSqlTableModel, connect to the previously created database, fill
    // the db with some data.
    QSqlTableModel tableModel( 0, db );
    tableModel.setTable( "airlines" );
    tableModel.select();
    tableModel.removeColumn( 0 );
    tableModel.setHeaderData( 0, Qt::Horizontal, QObject::tr("Name") );
    tableModel.setHeaderData( 1, Qt::Horizontal, QObject::tr("Home country") );
    QFont font = app.font();
    font.setBold( true );
    tableModel.setHeaderData( 0, Qt::Horizontal, font, Qt::FontRole );
    tableModel.setHeaderData( 1, Qt::Horizontal, font, Qt::FontRole );

    // associate the model and load the XML file
    report.associateModel( "airlines", &tableModel );
    QFile reportFile( ":/Database.xml" );
    if( !reportFile.open( QIODevice::ReadOnly ) ) {
        QMessageBox::warning( 0, QObject::tr( "Warning" ), QObject::tr( "Could not open report description file 'Database.xml'. Please start this program from the DatabaseXML directory." ) );
        return -1;
    }

    KDReports::ErrorDetails details;
    if( !report.loadFromXML( &reportFile, &details ) ) {
        QMessageBox::warning( 0, QObject::tr( "Warning" ), QObject::tr( "Could not parse report description file:\n%1" ).arg(details.message()) );
        reportFile.close();
        return -2;
    }

    // To show a print preview:
    KDReports::PreviewDialog preview( &report );
    return preview.exec();

    //return app.exec();
}

