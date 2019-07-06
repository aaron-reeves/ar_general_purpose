#include "cquerytable.h"


//-----------------------------------------------------------------
// Construction/destruction
//-----------------------------------------------------------------
  CQueryTable::CQueryTable() {
    _ownsQuery = false;
    _query = nullptr;
    _db = nullptr;
  }


  CQueryTable::CQueryTable( QSqlDatabase* db, const QString& queryStr ) {
    _db = db;
    _ownsQuery = true;
    _query = new QSqlQuery( *db );
    _query->exec( queryStr );
  }


  CQueryTable::CQueryTable( QSqlQuery* query ) {
    _db = nullptr;
    _query = query;
    _ownsQuery = false;
  }


  CQueryTable::CQueryTable( const CQueryTable& other ) {
    _ownsQuery = true;
    _query = new QSqlQuery( *(other._query) );
  }


  CQueryTable& CQueryTable::operator=( const CQueryTable& other ) {
    _ownsQuery = true;
    _query = new QSqlQuery( *(other._query) );

    return *this;
  }


  CQueryTable::~CQueryTable() {
    if( _ownsQuery ) {
      delete _query;
    }
  }


  bool CQueryTable::exec( const QString& queryStr ) {
    return _query->exec( queryStr );
  }
//-----------------------------------------------------------------


//-----------------------------------------------------------------
// Helper functions
//-----------------------------------------------------------------
  QString CQueryTable::header( const QList<int>& arr ) {
    int i, j;
    int spaces;
    QString head = "";

    for( i = 0; arr.size() > i; ++i ) {
      head.append( "+" );
      spaces = arr.at( i );
      for( j = 0; (spaces+2) > j; ++j ) {
        head.append( "-" );
      }
    }

    head.append( "+" );

    return head;
  }


  QString CQueryTable::column( QString label, int len ) {
    QString col;
    int i;
    int lenDiff;

    if( label.length() <= len ) {
      // Prepend the leading space
      col = QString( " %1" ).arg( label ); // Note the leading space

      // Add spaces until desired length is reached
      lenDiff = len - label.length();
      for( i = 0; lenDiff > i; ++i ) {
        col.append( " " );
      }

      // Tack on the trailing space
      col.append( " " );
    }
    else {
      col = QString( " %1" ).arg( label ); // Note the leading space
      col = col.left( len - 2 );
      col = col + "... "; // Note the trailing space
    }

    return col;
  }


  QString CQueryTable::dbVariantToString( const QVariant& var ) {
    QString str;

    if( var.isNull() ) {
      str = "NULL";
    }
    else {
      //qDebug() << var;

      switch( var.type() ) {
        case QVariant::UInt:
        case QVariant::Int:
        case QVariant::LongLong:
        case QVariant::ULongLong:
        case QVariant::Double:
        case QVariant::String:
        case QVariant::Bool:
          str = var.toString();
          break;
        case QVariant::Date:
          str = var.toDate().toString( "yyyy-MM-dd" );
          break;
        case QVariant::DateTime:
          str = var.toDateTime().toString( "yyyy-MM-dd hh:mm:ss" );
          break;
        default:
          Q_UNREACHABLE();
      }
    }

    return str;
  }

  QString CQueryTable::listLabel( QString label, int desiredLen ) {
    int i;
    int diff = desiredLen - label.length();
    for( i = 0; diff > i; ++i ) label.prepend( " " );
    return label;
  }
//-----------------------------------------------------------------


//-----------------------------------------------------------------
// Print functions
//-----------------------------------------------------------------
  void CQueryTable::printTableFormat( QTextStream* stream ) {
    int i;
    QList<int> arr;
    QList<QStringList> rowsAsStr;

    if(( nullptr == stream) || ( nullptr == _query ) ) {
      //qDebug() << "No stream in printTableFormat()";
      return;
    }

    _query->first();
    QSqlRecord row = _query->record();
    // Create and clear the field size array
    for( i = 0; i < row.count(); ++i ) {
      arr.append( 0 );
    }

    // Loop through the result set once to determine max sizes
    QStringList rowStr;
    QString str;
    do {
      row = _query->record();
      for( i = 0; i < row.count(); ++i ) {
        str = dbVariantToString( row.value(i) );
        rowStr.append( str );

        if( str.length() > arr.at(i) )
          arr.replace( i, str.length() );
      }

      rowsAsStr.append( rowStr );
      rowStr.clear();
    } while( _query->next() );

    // Don't forget the field names in the header
    for( i = 0; i < row.count(); ++i ) {
      rowStr.append( row.fieldName(i) );
      if( row.fieldName(i).length() > arr.at(i) )
        arr.replace( i, row.fieldName(i).length() );
    }

    rowsAsStr.prepend( rowStr );

    // Start writing!
    //===============

    // Header row first
    *stream << header( arr ) << endl;
    rowStr = rowsAsStr.at(0);
    for( int j = 0; j < rowStr.count(); ++j ) {
       *stream << "|" << column( rowStr.at(j), arr.at(j) );
    }
    *stream << "|" << endl;
    *stream << header( arr ) << endl;

    // Then data
    for( int i = 1; i < rowsAsStr.count(); ++i ) {
      rowStr = rowsAsStr.at(i);
      for( int j = 0; j < rowStr.count(); ++j ) {
         *stream << "|" << column( rowStr.at(j), arr.at(j) );
      }
      *stream << "|" << endl;
    }

    *stream << header( arr ) << endl;
    *stream << flush;
  }


  void CQueryTable::printListFormat( QTextStream* stream ) {
    QSqlRecord row;
    int rowCount;
    int i;
    int maxNameLen = 0;
    QStringList labelList;

    if(( nullptr == stream) || ( nullptr == _query ) ) {
      //qDebug() << "No stream in printListFormat()";
      return;
    }

    _query->first();
    row = _query->record();
    // Determine the max field name length.
    for( i = 0; i < row.count(); ++i ) {
      if( row.fieldName(i).length() > maxNameLen ) maxNameLen = row.fieldName(i).length();
    }

    // Loop over fields again, creating and storing the field labels.
    for( i = 0; row.count() > i; ++i ) {
      labelList.append( listLabel( row.fieldName(i), maxNameLen ) );
    }

    // Loop over the records, writing the information to the console.
    rowCount = 1;
    do {
      *stream << "*************************** " << rowCount << ". row ***************************" << endl;
      for( i = 0; labelList.count() > i; ++i ) {
         *stream << labelList.at(i) <<  ": " << dbVariantToString( _query->value(i) ) << endl;
      }
      ++rowCount;
    } while( _query->next() );

    *stream << endl;
    *stream << flush;
  }
//-----------------------------------------------------------------
