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
  _errorMsg = QStringLiteral("Magic8Ball style is undefined.");

  QString numberStr;
  QString proportionStr;
  QString percentageStr;

  QStringList params = CSV::parseLine( parameters );

  for( const QString& param : params ) {
    QStringList indivParam = CSV::parseLine( param, '=' );
    if( 2 == indivParam.count() ) {
      if( 0 == indivParam.at(0).compare( QLatin1String("number"), Qt::CaseInsensitive ) )
        numberStr = indivParam.at(1);

      if( 0 == indivParam.at(0).compare( QLatin1String("proportion"), Qt::CaseInsensitive ) )
        proportionStr = indivParam.at(1);

      if( 0 == indivParam.at(0).compare( QLatin1String("percentage"), Qt::CaseInsensitive ) )
        percentageStr = indivParam.at(1);

      if( 0 == indivParam.at(0).compare( QLatin1String("percent"), Qt::CaseInsensitive ) )
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
    _errorMsg = QStringLiteral("Missing parameter: please specify number, proportion, or percentage.");
  }
  else if( 1 != nParams ) {
    _error = true;
    _errorMsg = QStringLiteral("Too many parameters: please specify one of number, proportion, or percentage.");
  }

  bool ok;

  if( !numberStr.isEmpty() ) {
    _number = numberStr.toInt( &ok );
    if( !ok ) {
      _error = true;
      _errorMsg = QStringLiteral( "Bad parameter: Number format (%1) is incorrect." ).arg( numberStr );
    }
    else {
      setNumberType( _number );
    }
  }

  else if( !proportionStr.isEmpty() ) {
    _proportion = proportionStr.toDouble( &ok );
    if( !ok ) {
      _error = true;
      _errorMsg = QStringLiteral( "Bad parameter: Proportion format (%1) is incorrect." ).arg( proportionStr );
    }
    else {
      setProportionType( _proportion );
    }
  }

  else if( !percentageStr.isEmpty() ) {
    percentageStr.replace( '%', QString() );
    _proportion = percentageStr.toDouble( &ok );
    if( !ok ) {
      _error = true;
      _errorMsg = QStringLiteral( "Bad parameter: Proportion format (%1) is incorrect." ).arg( proportionStr );
    }
    else {
      _proportion = ( _proportion / 100.0 );
      setProportionType( _proportion );
    }
  }
}


CMagic8Ball::~CMagic8Ball() {
  if( nullptr != _rng )
    RAN_free_generator( _rng );
}


void CMagic8Ball::setNumberType( const int number ) {
  _type = Number;
  _number = number;

  _rng = nullptr;

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
  bool result;
  double ran;

  switch( _type ) {
    case Number:
      result = ( _number >= number );
      break;

    case Proportion:
      ran = RAN_num( _rng );
      result = ( _proportion > ran );
      break;

    default:
      _error = true;
      _errorMsg = QStringLiteral("Magic8Ball type is unspecified.");
      result =  false;
      break;
  }

  return result;
}
