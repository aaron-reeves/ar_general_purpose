#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QtCore>
#include <QtGui>
#include <QMessageBox>
//#include <QTableWidgetItem>

#include <ar_general_purpose/csv.h>

#include "mainwindow.h"
#include "ui_mainwindow.h"


#ifdef UNDEFINED
// Some useful testing code.  Integrate this, somehow...

cout << "Basic table:" << endl;
QCsv allItems( "produce.csv", true, true, QCsv::EntireFile );
allItems.open(); // This isn't strictly necessary for mode EntireFile (it happens implicitly by this particular constructor), but it's a good habit to get into.
cout << allItems.asTable() << endl;

cout << "New column 'is fruit':" << endl;
allItems.appendField( "is fruit" );
allItems.setField( "is fruit", 0, "yes" );
allItems.setField( 3, 1, "yes" );
allItems.setField( "is fruit", 2, "yes" );
allItems.setField( 3, 3, "yes" );
allItems.setField( "is fruit", 4, "no" );
allItems.setField( 3, 5, "no" );
cout << allItems.asTable() << endl;

cout << "Drop column 'mostly round':" << endl;
allItems.removeField( "mostly round" );
cout << allItems.asTable() << endl;

cout << "Only green items:" << endl;
QCsv greenItems = allItems.filter( 1, "green" );
greenItems.debug();
cout << greenItems.asTable() << endl;

cout << "Only red items:" << endl;
QCsv redItems = allItems.filter( "color", "Red", Qt::CaseInsensitive );
cout << redItems.asTable() << endl;

cout << "A copy of red items:" << endl;
QCsv redFruitCopy = redItems;
cout << redFruitCopy.asTable() << endl;

cout << "Now with raspberries:" << endl;
QStringList raspberry;
raspberry << "raspberry" << "red" << "yes";
redFruitCopy.appendRow( raspberry );
redFruitCopy.renameField( "IS FRUIT", "fruit" );
cout << redFruitCopy.asTable() << endl;

QCsv sample( "sample.csv", true, true, QCsv::LineByLine );
// This will generate an error.  Remember that you have to explicitly open a file in line-by-line mode.
while( -1 != sample.moveNext() ) {
  cout << sample.field( "chico" ) << endl;
}
cout << sample.errorMsg() << endl << endl;

if( !sample.open() ) {
  qDebug() << "Couldn't open file.";
}
else {
  while( -1 != sample.moveNext() ) {
    cout << sample.field( "chico" ) << endl;
  }
  cout << sample.errorMsg() << endl;  // Look, Ma!  No error!
}

cout << endl << "All but the first two items (note that raspberries don't appear on this list: they weren't added here):" << endl;
allItems.toFront();
allItems.moveNext();
allItems.moveNext();
while( -1 != allItems.moveNext() ) {
  cout << allItems.field( 0 ) << endl;
}

cout << endl << "All items from the top again (still no raspberries):" << endl;
allItems.toFront();
while( -1 != allItems.moveNext() ) {
  cout << allItems.field( 0 ) << endl;
}

QCsv series;
series.setMode( QCsv::EntireFile );
series.setFilename( "series.csv" );
series.open();
cout << series.errorMsg() << endl;
cout << series.asTable() << endl;
cout << series.errorMsg() << endl;

#endif

MainWindow::MainWindow(QWidget *parent) :
  QMainWindow(parent),
  ui(new Ui::MainWindow)
{
  ui->setupUi(this);
}

MainWindow::~MainWindow(){
  delete ui;
}


void MainWindow::on_btnLoadFile_clicked(){
  ui->tblData->clearContents();
  ui->tblData->setRowCount( 0 );
  ui->tblData->setColumnCount( 0 );
  
  // Load the CSV file.
  //-------------------
  QCsv csvFile;
  csvFile.setFilename ( ui->leFileName->text() );
  csvFile.setMode( QCsv::EntireFile );
  csvFile.setContainsFieldList( true );
  csvFile.setStringsContainDelimiters( true );

  if( csvFile.open() ) {
    // Set up the header row.
    //-----------------------
    ui->tblData->setColumnCount( csvFile.fieldCount() );
    for ( int i = 0; i < csvFile.fieldCount(); i++ ){
      QTableWidgetItem* Item = new QTableWidgetItem ( csvFile.fieldName ( i ) );
      ui->tblData->setHorizontalHeaderItem ( i, Item );
    }

    qDebug() << csvFile.rowCount() << csvFile.fieldCount() << csvFile.currentRowNumber();

    // Set up data rows.
    //------------------
    int row = 0;
    while( 0 < csvFile.moveNext() ){
      //qDebug() << "Adding a row!" << csvFile.currentRow();
      ui->tblData->insertRow( row );
      for ( int i = 0; i < csvFile.fieldCount(); i++ ){
        QTableWidgetItem* Item = new QTableWidgetItem ( csvFile.field ( i ) );
        ui->tblData->setItem ( row, i, Item );
      }
      QApplication::processEvents();
      ++row;
    }
  }
  
  if ( csvFile.error() != QCsv::ERROR_NONE )
    QMessageBox::warning( this, csvFile.errorMsg(), "Error" );
}
