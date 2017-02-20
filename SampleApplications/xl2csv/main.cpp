
#include <ar_general_purpose/qcout.h>
#include <ar_general_purpose/xlcsv.h>

int main( int argc, char* argv[] ) {
  Q_UNUSED( argc );

  CXlCsv csv( CXlCsv::Format97_2003, argv[1], true );

  if( csv.open() ) {
    cout << csv.asTable();
    csv.setFieldFormatXl( "DATE", CXlCsv::DateFormat );
    cout << endl;
    cout << csv.asTable();
  }
  else {
    cout << csv.error();
  }

  return 0;
}



