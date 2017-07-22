#include "cmagic8ball.h"
#include <ar_general_purpose/csv.h>

CMagic8Ball::CMagic8Ball( const int number ) {
  setNumberType( number );
}


CMagic8Ball::CMagic8Ball( const double proportion ) {
  setProportionType( proportion );
}


CMagic8Ball::CMagic8Ball( const QString& parameters ) {
  _type = Undefined;
  _error = true;
  _errorMsg = "Magic8Ball style is undefined.";

  QString numberStr;
  QString proportionStr;
  QString percentageStr;

  QStringList params = CSV::parseLine( parameters );

  foreach( QString param, params ) {
    QStringList indivParam = CSV::parseLine( param, '=' );
    if( 2 == indivParam.count() ) {
      if( 0 == indivParam.at(0).compare( "number", Qt::CaseInsensitive ) )
        numberStr = indivParam.at(1);

      if( 0 == indivParam.at(0).compare( "proportion", Qt::CaseInsensitive ) )
        proportionStr = indivParam.at(1);

      if( 0 == indivParam.at(0).compare( "percentage", Qt::CaseInsensitive ) )
        percentageStr = indivParam.at(1);

      if( 0 == indivParam.at(0).compare( "percent", Qt::CaseInsensitive ) )
        percentageStr = indivParam.at(1);
    }
  }

  int nParams = 0;
  if( !numberStr.isEmpty() )
    ++nParams;
  if( !proportionStr.isEmpty() )
    ++nParams;
  if( !percentageStr.isEmpty() )
    ++nParams;

  if( 0 == nParams ) {
    _error = true;
    _errorMsg = "Missing parameter: please specify number, proportion, or percentage.";
  }
  else if( 1 != nParams ) {
    _error = true;
    _errorMsg = "Too many parameters: please specify one of number, proportion, or percentage.";
  }

  bool ok;

  if( !numberStr.isEmpty() ) {
    _number = numberStr.toInt( &ok );
    if( !ok ) {
      _error = true;
      _errorMsg = QString( "Bad parameter: Number format (%1) is incorrect." ).arg( numberStr );
    }
    else {
      setNumberType( _number );
    }
  }

  else if( !proportionStr.isEmpty() ) {
    _proportion = proportionStr.toDouble( &ok );
    if( !ok ) {
      _error = true;
      _errorMsg = QString( "Bad parameter: Proportion format (%1) is incorrect." ).arg( proportionStr );
    }
    else {
      setProportionType( _proportion );
    }
  }

  else if( !percentageStr.isEmpty() ) {
    percentageStr.replace( "%", "" );
    _proportion = percentageStr.toDouble( &ok );
    if( !ok ) {
      _error = true;
      _errorMsg = QString( "Bad parameter: Proportion format (%1) is incorrect." ).arg( proportionStr );
    }
    else {
      _proportion = ( _proportion / 100.0 );
      setProportionType( _proportion );
    }
  }
}


CMagic8Ball::~CMagic8Ball() {
  if( NULL != _rng )
    RAN_free_generator( _rng );
}


void CMagic8Ball::setNumberType( const int number ) {
  _type = Number;
  _number = number;

  _rng = NULL;

  _error = false;
  _errorMsg = QString();
}


void CMagic8Ball::setProportionType( const double proportion ) {
  _type = Proportion;
  _proportion = proportion;

  _rng = RAN_new_generator( -1 );

  _error = false;
  _errorMsg = QString();
}


bool CMagic8Ball::answer( const int number ) {
  double ran;

  switch( _type ) {
    case Number:
      return( _number >= number );
      break;

    case Proportion:
      ran = RAN_num( _rng );
      return( _proportion > ran );
      break;

    default:
      _error = true;
      _errorMsg = "Magic8Ball type is unspecified.";
      return false;
      break;
  }
}
