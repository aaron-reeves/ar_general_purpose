
#include <ar_general_purpose/qcout.h>
#include <ar_general_purpose/xlcsv.h>
#include <ar_general_purpose/cspreadsheetarray.h>

int main( int argc, char* argv[] ) {
  Q_UNUSED( argc );

  CXlCsv csv( CXlCsv::Format97_2003, argv[1], true );

  if( csv.open() ) {
    cout << csv.asTable() << endl << flush;
  }
  else {
    cout << csv.error() << ": " << csv.errorMsg() << endl << flush;
  }

  CSpreadsheetWorkBook wb( CSpreadsheetWorkBook::Format97_2003, argv[1] );
  wb.readSheet( 0 );

  QCsv csv2 = wb.sheet(0).asCsv( true );

  cout << endl << endl << "Now let's do it again:" << endl;

  if( QCsv::ERROR_NONE == csv2.error() )
    cout << csv2.asTable() << endl;
  else
    cout << csv2.error() << ": " << csv2.errorMsg() << endl << flush;


  cout << endl << endl << "Do the new version:" << endl;

  CXlCsv csv3( CXlCsv::Format2007, argv[2], true );

  if( csv3.open() ) {
    cout << csv3.asTable() << endl << flush;
  }
  else {
    cout << csv3.error() << ": " << csv3.errorMsg() << endl << flush;
  }

  return 0;
}



