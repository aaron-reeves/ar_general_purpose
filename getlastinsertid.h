#ifndef GETLASTINSERTID_H
#define GETLASTINSERTID_H

#include <QtCore>
#include <QtSql>

/*
 * The QSqlQuery.lastInsertId() does not always play nicely.
 * The way that this function is implemented for versions of PostgreSQL > 8.1
 * is as follows (from https://bugreports.qt.io/browse/QTBUG-53807):
 *
 *   if (d->privDriver()->pro >= QPSQLDriver::Version81)
 *   {
 *      QSqlQuery qry(driver()->createResult()); // Most recent sequence value obtained from nextval
 *      if (qry.exec(QLatin1String("SELECT lastval();")) && qry.next())
 *        return qry.value(0);
 *   }
 *   ...
 *
 * However, lastval() doesn't always return what was expected
 * (from http://pear.php.net/bugs/bug.php?id=19918):
 *
 *   An example: I'm having the table 'foo' and a trigger configured on it,
 *   which is causing an INSERT to table 'bar' on any change to 'foo'.
 *   Now I'm doing sth. like:
 *     INSERT INTO 'foo';
 *     SELECT lastval();
 *   However in this case 'lastval()' doesn't return the ID of the INSERT
 *   my application caused, but the INSERT the trigger caused.
 *
 *   This isn't just a theoretical issue: I'm using slony for replication
 *   of my postgres tables and the application using lastInsertID()
 *   reproducible get back the the ID of the INSERT caused by the trigger,
 *   not my application.
 *
 * And from https://www.postgresql.org/message-id/87tz6byio0.fsf@dba2.int.libertyrms.com:
 *
 *   Someone recently reported this issue as a possible bug in Slony-I;
 *   they had written their application to use lastval() to capture
 *   sequence values, and then, when they introduced replication, they
 *   started capturing values of a sequence Slony-I uses to control *its*
 *   operations.
 *
 * The function getLastInsertId() determines the last inserted ID not from the
 * PostgreSQL function lastval(), but by querying the appropriate sequence,
 * which, at the moment, must be specified.
 */
int getLastInsertId( QSqlDatabase* db, const QString& sequenceName );

#endif // GETLASTINSERTID_H
