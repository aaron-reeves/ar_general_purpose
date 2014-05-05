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
  int row = 0;
  int i;

  ui->tblData->clearContents();
  ui->tblData->setRowCount( 0 );
  ui->tblData->setColumnCount( 0 );
  
  csvFile.setFilename ( ui->leFileName->text() );
  csvFile.setContainsFieldList( true );
  //csvFile.setStringToken( '"' );
  csvFile.setStringsContainCommas( true );
  csvFile.open();
  csvFile.moveNext();

  ui->tblData->setColumnCount( csvFile.setColumnCount() );
  for ( i = 0; i < csvFile.setColumnCount(); i++ ){
    Item = new QTableWidgetItem ( csvFile.fieldName ( i + 1 ) );
    ui->tblData->setHorizontalHeaderItem ( i, Item );
  }

  ui->tblData->insertRow ( row );
  for ( i = 0; i < csvFile.setColumnCount(); i++ ){
    Item = new QTableWidgetItem ( csvFile.field ( i + 1 ) );
    ui->tblData->setItem ( row, i, Item );
  }
 
  row++;
  while ( 0 < csvFile.moveNext() ){
    ui->tblData->insertRow ( row );
    for ( i = 0; i < csvFile.setColumnCount(); i++ ){
      Item = new QTableWidgetItem ( csvFile.field ( i + 1 ) );
      ui->tblData->setItem ( row, i, Item );
    }
    row++;
    QApplication::processEvents();
  }
  
  if ( csvFile.error() != qCSV::qCSV_ERROR_NONE )
    QMessageBox::warning( this, csvFile.errorMsg(), "Error" );
}
