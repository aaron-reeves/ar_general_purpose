#include <QtCore>

#include <ar_general_purpose/qcout.h>
#include <ar_general_purpose/cspreadsheetarray.h>


int unmergeFiles( CSpreadsheetWorkBook* wb ) {
    QFileInfo fi( wb->sourcePathName() );

  QString timestamp = QDateTime::currentDateTime().toString( QStringLiteral("yyyyMMddhhmmss") );

  QString outputFileName;

  CSpreadsheet ssCopy1;

  if( true ) {
    // ssCopy will go out of scope and be destroyed.
    // ssCopy1, however, should still be a valid, functional copy.
    CSpreadsheet ssCopy = wb->sheet(0);
    ssCopy1 = ssCopy;

    ssCopy.debugVerbose();

    outputFileName = QStringLiteral( "%1/%2-copy.xlsx" ).arg( fi.path(), timestamp );
    if( !ssCopy.writeXlsx( outputFileName ) ) {
      qDebug() << "Failed to write copied file.";
      return -1;
    }
  }

  ssCopy1.unmergeRows( true );

  outputFileName = QStringLiteral( "%1/%2-unmergedRows.xlsx" ).arg( fi.path(), timestamp );
  if( !ssCopy1.writeXlsx( outputFileName ) ) {
    qDebug() << "Failed to write unmerged row file.";
    return -1;
  }

  ssCopy1 = wb->sheet(0);
  ssCopy1.unmergeColumns( true );

  outputFileName = QStringLiteral( "%1/%2-unmergedCols.xlsx" ).arg( fi.path(), timestamp );
  if( !ssCopy1.writeXlsx( outputFileName ) ) {
    qDebug() << "Failed to write unmerged col file.";
    return -1;
  }

  ssCopy1 = wb->sheet(0);
  ssCopy1.unmergeColumnsAndRows( true );

  outputFileName = QStringLiteral( "%1/%2-unmergedRowsAndCols.xlsx" ).arg( fi.path(), timestamp );
  if( !ssCopy1.writeXlsx( outputFileName ) ) {
    qDebug() << "Failed to write unmerged row and col file.";
    return -1;
  }

  return 0;
}


int testRows( CSpreadsheetWorkBook* wb ) {
  CSpreadsheet* ss = &(wb->sheet(0));

  qDebug() << endl;
  qDebug() << "Before unmerging:";

  for( int i = 0; i < 4; ++i ) {
    qDebug() << ss->rowAsVariantList( i );
    qDebug() << ss->rowAsStringList( i );
  }

  qDebug() << endl;
  qDebug() << "After unmerging and duplicating:";
  ss->unmergeRows( true );

  for( int i = 0; i < 4; ++i ) {
    qDebug() << ss->rowAsVariantList( i );
    qDebug() << ss->rowAsStringList( i );
  }

  qDebug() << endl;

  return 0;
}


int main( int argc, char* argv[] ) {
  if( argc < 2 ) {
    qDebug() << "Missing file name parameter.";
    return -1;
  }

  CSpreadsheetWorkBook wb( argv[1] );

  if( wb.error() ) {
    qDebug() << "File error:" << wb.errorMessage();
    return -1;
  }

  if( !wb.readSheet(0) ) {
    qDebug() << "Sheet read failed:" << wb.errorMessage();
    return -1;
  }

  return unmergeFiles( &wb );
  //return testRows( &wb );
}



