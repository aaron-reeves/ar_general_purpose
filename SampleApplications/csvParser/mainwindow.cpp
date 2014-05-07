#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QtCore>
#include <QtGui>
#include <QMessageBox>
//#include <QTableWidgetItem>

#include <ar_general_purpose/csv.h>

#include "mainwindow.h"
#include "ui_mainwindow.h"

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
  QTableWidgetItem *Item;
  qCSV csvFile;
  int row;
  int i;

  ui->tblData->clearContents();
  ui->tblData->setRowCount( 0 );
  ui->tblData->setColumnCount( 0 );
  
  // Load the CSV file.
  //-------------------
  csvFile.setFilename ( ui->leFileName->text() );
  csvFile.setContainsFieldList( true );
  //csvFile.setStringToken( '"' );
  csvFile.setStringsContainCommas( true );
  csvFile.open();

  // Set up the header row.
  //-----------------------
  row = 0;
  csvFile.moveNext();

  ui->tblData->setColumnCount( csvFile.columnCount() );
  for ( i = 0; i < csvFile.columnCount(); i++ ){
    Item = new QTableWidgetItem ( csvFile.fieldName ( i ) );
    ui->tblData->setHorizontalHeaderItem ( i, Item );
  }

  // Set up the first data row.  Note that moveNext is not needed,
  // as the first data row was loaded into csv by the previous call.
  //----------------------------------------------------------------
  ui->tblData->insertRow ( row );
  for ( i = 0; i < csvFile.columnCount(); i++ ){
    Item = new QTableWidgetItem ( csvFile.field ( i ) );
    ui->tblData->setItem ( row, i, Item );
  }
 
  // Set up remaining data rows.
  //----------------------------
  while ( 0 < csvFile.moveNext() ){
    ++row;
    ui->tblData->insertRow ( row );
    for ( i = 0; i < csvFile.columnCount(); i++ ){
      Item = new QTableWidgetItem ( csvFile.field ( i ) );
      ui->tblData->setItem ( row, i, Item );
    }
    QApplication::processEvents();
  }
  
  if ( csvFile.error() != qCSV::qCSV_ERROR_NONE )
    QMessageBox::warning( this, csvFile.errorMsg(), "Error" );
}
