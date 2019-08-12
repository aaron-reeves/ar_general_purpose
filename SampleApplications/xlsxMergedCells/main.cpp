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


int xlInfo( CSpreadsheetWorkBook* wb ) {
  cout << endl;
  cout << "File name                  : " << QFileInfo( wb->sourcePathName() ).fileName() << endl;
  cout << "File format                : " << wb->fileFormatAsString() << endl;

  QString msg = wb->magicFileTypeDescr();
  if( 60 > msg.length() ) {
    cout << "Magic file description     : " << msg << endl;
  }
  else {
    QStringList msgList = msg.split( ',' );
     cout << "Magic file description     : " << endl;
    for( int j = 0; j < msgList.count(); ++j ) {
      cout << "  " << msgList.at(j).trimmed() << endl;
    }

    //msg = prettyPrint( msg, 50, false, false, 2 );
    //cout << "Magic file description   : " << endl;
    //cout << msg;
  }

  cout << "1904 date system            : " << boolToText( wb->isXls1904DateSystem() ) << endl;
  cout << "Number of sheets            : " << wb->sheetCount() << endl;
  cout << "----------------------------- " << endl;

  for( int i = 0; i < wb->sheetCount(); ++i ) {
    cout << "  Sheet name                : " << wb->sheetName(i) << endl;
    cout << "    Sheet is empty          : " << boolToText( wb->sheet(i).isEmpty() ) << endl;

    if( !wb->sheet(i).isEmpty() ) {
      cout << "    Sheet dimensions        : " << wb->sheet(i).nCols() << " cols x " << wb->sheet(i).nRows() << " rows" << endl;
      cout << "    Has empty rows          : " << boolToText( wb->sheet(i).hasEmptyRows() ) << endl;
      cout << "    Has empty columns       : " << boolToText( wb->sheet(i).hasEmptyColumns() ) << endl;
      cout << "    Sheet is tidy           : " << boolToText( wb->sheet(i).isTidy( true ) ) << endl;
      cout << "    Number of merged ranges : " << wb->sheet(i).mergedRangeCount() << endl;

      QStringList firstRow = wb->sheet(i).rowAsStringList(0);
      QString msg;
      for( int j = 0; j < firstRow.count(); ++j ) {
        msg.append( QStringLiteral( "'%1', " ).arg( firstRow.at(j) ) );
      }
      if( !msg.isEmpty() ) {
        msg = msg.left( msg.length() - 2 );
      }

      if( 60 > msg.length() ) {
        cout << "    First row               : " << msg << endl;
      }
      else {
        msg = prettyPrint( msg, 50, true, true, 4 );
        cout << "    First row               : " << endl;
        cout << msg;
      }
    }

    cout << endl;
  }

  cout << endl;

  return 0;
}


int main( int argc, char* argv[] ) {
  if( argc < 2 ) {
    cout << "Missing file name parameter." << endl;
    return -1;
  }

  CSpreadsheetWorkBook wb( argv[1] );

  if( wb.error() ) {
    qDebug() << "File error:" << wb.errorMessage();
    return -1;
  }

  if( !wb.readAllSheets() ) {
    cout << "Sheet read failed:" << wb.errorMessage() << endl;
    return -1;
  }

  //return unmergeFiles( &wb );
  //return testRows( &wb );

  return xlInfo( &wb );
}



