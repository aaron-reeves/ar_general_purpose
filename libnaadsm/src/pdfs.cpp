#include <naadsm/pdfs.h>

#include <gsl/gsl_randist.h>

//-----------------------------------------------------------------------------
// CPdf: Base class for all Pdfs
//-----------------------------------------------------------------------------
CPdf::CPdf() {
  // Nothing to do here.
}


CPdf::CPdf( const CPdf& other ) {
  _pdfType = other._pdfType;
  _infLeftTail = other._infLeftTail;
  _infRightTail = other._infRightTail;
}


CPdf::~CPdf() {
  // Nothing to do here
}


bool CPdf::validate( QString* errMsg  /* = NULL */ ) {
  if( NULL != errMsg )
    errMsg->append( "Function type is unspecified.\n" );
  return false;
}


int CPdf::randInt( RAN_gen_t* rng ) {
  return int( round( rand( rng ) ) );
}

QVector<double> CPdf::variates( const int nVariates, RAN_gen_t* rng /* = NULL */ ) {
  QVector<double> result;
  result.resize( nVariates );

  bool createRng = ( NULL == rng );

  if( createRng )
    rng = RAN_new_generator( QDateTime::currentDateTime().toTime_t() );

  for( int i = 0; i < nVariates; ++i )
    result[i] = this->rand( rng );

  if( createRng )
    RAN_free_generator ( rng );

  return result;
}

void CPdf::variatesToFile( const QString& fileName, const int nVariates, RAN_gen_t* rng /* = NULL */ ) {
  QVector<double> variates = this->variates( nVariates, rng );

  QFile f( fileName );

  if( f.open( QFile::WriteOnly | QFile::Truncate ) ) {
    QTextStream ts( &f );
    ts << "\"" << this->descr() << "\"" << endl;
    for( int i = 0; i < nVariates; ++i )
      ts << variates[i] << endl;
    f.close();
  }
}


//int CPdf::minD() {
//  if( this->isContinuous() ) {
//    if( this->hasMin() )
//      return( int( floor( this->min() ) ) );
//    else
//      return int( floor( this->invCumulative( 0.00001 ) ) );
//  }
//  else {
//    if( this->hasMin() )
//      return( int( floor( this->min() ) ) );
//    else
//      return( int( this->invCumulative( 0.00001 ) ) );
//  }
//}


//int CPdf::maxD() {
//  if( this->isContinuous() ) {
//    if( this->hasMax() )
//      return( int( ceil( this->max() ) ) );
//    else
//      return int( ceil( this->invCumulative( 0.99999 ) ) );
//  }
//  else {
//    if( this->hasMax() )
//      return( int( floor( this->max() ) ) );
//    else
//      return( int( this->invCumulative( 0.99999 ) ) );
//  }
//}


//double CPdf::discreteProb( int x );
//double CPdf::discreteCumulativeProb( int x );


QString CPdf::pdfTypeDescr( PdfType type ) {
  QString result;

  switch( type ) {
    case PdfUndefined: result = "(Unspecified)"; break;
    case PdfBeta: result = "Beta"; break;
    case PdfBetaPERT: result = "BetaPERT"; break;
    case PdfExponential: result = "Exponential"; break;
    case PdfGamma: result = "Gamma"; break;
    case PdfGaussian: result = "Gaussian (normal)"; break;
    case PdfHistogram: result = "Histogram"; break;
    case PdfInverseGaussian: result = "Inverse Gaussian"; break;
    case PdfLogistic: result = "Logistic"; break;
    case PdfLogLogistic: result = "Loglogistic"; break;
    case PdfLognormal: result = "Lognormal"; break;
    case PdfPareto: result = "Pareto"; break;
    case PdfPearson5: result = "Pearson 5"; break;
    case PdfPiecewise: result = "Piecewise (general)"; break;
    case PdfPoint: result = "Fixed value"; break;
    case PdfTriangular: result = "Triangular"; break;
    case PdfUniform: result = "Uniform"; break;
    case PdfWeibull: result = "Weibull"; break;
    case PdfBernoulli: result = "Bernoulli"; break;
    case PdfBinomial: result = "Binomial"; break;
    case PdfDiscreteUniform: result = "Discrete uniform"; break;
    case PdfHypergeometric: result = "Hypergeometric"; break;
    case PdfNegativeBinomial: result = "Negative binomial"; break;
    case PdfPoisson: result = "Poisson"; break;
    default:
      result = "(Unspecified)";
      break;
  }

  return result;
}
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
// CPdfDiscrete: Base class for all discrete PDFs
//-----------------------------------------------------------------------------
CPdfDiscrete::CPdfDiscrete() : CPdf() {
  // Nothing to do here.
}


CPdfDiscrete::CPdfDiscrete( const CPdfDiscrete& other ) : CPdf( other ) {
  // Nothing to do here.
}

CPdfDiscrete::~CPdfDiscrete() {
  // Nothing to do here.
}


//double CPdfDiscrete::invCumulative( double p );
//int CPdfDiscrete::randInvCumulative();
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
// CPdfBinomial
//-----------------------------------------------------------------------------
CPdfBinomial::CPdfBinomial() : CPdfDiscrete() {
  initialize();
}

CPdfBinomial::CPdfBinomial( const int n, const double p ) : CPdfDiscrete() {
  initialize();

  _n = n;
  _p = p;
}

CPdfBinomial::CPdfBinomial( const CPdfBinomial& other ) : CPdfDiscrete( other ) {
  initialize();

  _n = other._n;
  _p = other._p;
}

void CPdfBinomial::initialize() {
  _pdfType = PdfBinomial;

  setN( -1 );
  setP( NAN );

  _infLeftTail = false;
  _infRightTail = false;
}

CPdfBinomial::~CPdfBinomial() {

}


bool CPdfBinomial::validate( QString* errMsg /* = NULL */ ) {
  bool result = true; // Until shown otherwise;

  if( _n < 1) {
    if( NULL != errMsg )
      errMsg->append( "Number of trials must be greater than 0.\n" );
    result = false;
  }

  if( isnan( _p ) ) {
    if( NULL != errMsg )
      errMsg->append( "Probability of success must be specified.\n" );
    result = false;
  }
  else if( ( 0.0 > _p ) || ( 1.0 < _p ) ) {
    if( NULL != errMsg )
      errMsg->append( "Probability of success must be between 0 and 1, inclusive.\n" );
    result = false;
  }

  return result;
}


bool CPdfBinomial::equals( const CPdf& other ) {
  if( (const_cast<CPdf&>(other)).pdfType() != _pdfType )
    return false;
  else {
    CPdfBinomial o = dynamic_cast<CPdfBinomial&>( const_cast<CPdf&>(other) );

    return(
      ( this->n() == o.n() )
      && ( this->p() == o.p() )
    );
  }
}


double CPdfBinomial::probDensity( const double k ) {
  double result;

  if( 1.0 == _p ) {
    if( k == _n )
      result = 1.0;
    else
      result = 0.0;
  }
  else
    result = gsl_ran_binomial_pdf( k, _p, _n );

  return result;
}


double CPdfBinomial::rand( RAN_gen_t* rng ) {
  double result;

  if( 0 == _n )
    result = 0;
  else
    result = gsl_ran_binomial( RAN_generator_as_gsl( rng ), _p, _n );

  return result;
}


double CPdfBinomial::rand( RAN_gen_t* rng, const int n, const double p ) {
  return gsl_ran_binomial( RAN_generator_as_gsl( rng ), p, n );
}


void CPdfBinomial::multinomialRand( RAN_gen_t* rng, int n, const QVector<double>& p, QVector<int>& output ) {
  // This function is based on C++ code written by Agner Fog and released under the GPL.
  // See http://www.agner.org/random/

  double sum;
  double s;
  int x;
  int i;

  output.clear();
  output.resize( p.count() );

  // Compute sum of probabilities.  Do a little error checking along the way.
  sum = 0.0;
  for( i = 0; i < p.count(); ++i ) {
    Q_ASSERT( 0.0 <= p[i] );
    sum = sum + p[i];
  }

  // Generate output by calling binomial (c-1) times.
  for( i = 0; i < p.count() - 1; ++i ) {
    s = p.at( i );
    if( sum <= s ) {
      // this fixes two problems:
      // 1. prevent division by 0 when sum = 0.
      // 2. prevent s/sum getting bigger than 1 in case of rounding errors.
     x = n;
    }
    else
      x = CPdfBinomial::rand( rng, n, s/sum );

    n -= x;
    sum -= s;
    output[i] = x;
  }

  // Done forget the last one.
  output[i] = n;
}


void CPdfBinomial::multinomialRandDebug( RAN_gen_t* rng, int n, const QVector<double>& p, QVector<int>& output ) {
  // This function is based on C++ code written by Agner Fog and released under the GPL.
  // See http://www.agner.org/random/

  double sum;
  double s;
  int x;
  int i;

  output.clear();
  output.resize( p.count() );

  // Compute sum of probabilities.  Do a little error checking along the way.
  sum = 0.0;
  for( i = 0; i < p.count(); ++i ) {
    Q_ASSERT( 0.0 <= p[i] );
    sum = sum + p[i];
  }

  qDebug() << "n:" << n;
  qDebug() << "sum:" << sum;

  // Generate output by calling binomial (c-1) times.
  for( i = 0; i < p.count() - 1; ++i ) {
    s = p.at( i );
    if( sum <= s ) {
      // this fixes two problems:
      // 1. prevent division by 0 when sum = 0.
      // 2. prevent s/sum getting bigger than 1 in case of rounding errors.
     x = n;
    }
    else
      x = CPdfBinomial::rand( rng, n, s/sum );

    n -= x;
    sum -= s;

    qDebug() << "  Bin" << i << ": selected" << x << ", nRemaining" << n << ", pRemaining"  << sum;

    output[i] = x;
  }

  // Done forget the last one.
  output[i] = n;
}
//-----------------------------------------------------------------------------



//-----------------------------------------------------------------------------
// Discrete point PDF
//-----------------------------------------------------------------------------
bool CPdfDiscretePoint::equals( const CPdf& other ) {
  if( (const_cast<CPdf&>(other)).pdfType() != _pdfType )
    return false;
  else {
    CPdfDiscretePoint o = dynamic_cast<CPdfDiscretePoint&>( const_cast<CPdf&>(other) );

    return( this->value() == o.value() );
  }
}
//-----------------------------------------------------------------------------



//-----------------------------------------------------------------------------
// Discrete uniform PDF
//-----------------------------------------------------------------------------
CPdfDiscreteUniform::CPdfDiscreteUniform() : CPdfDiscrete() {
  initialize();
}

CPdfDiscreteUniform::CPdfDiscreteUniform( const int min, const int max ) : CPdfDiscrete() {
  initialize();

  _min = min;
  _max = max;
}

CPdfDiscreteUniform::CPdfDiscreteUniform( const CPdfDiscreteUniform& other ) : CPdfDiscrete( other ) {
  initialize();

  _min = other._min;
  _max = other._max;
}


void CPdfDiscreteUniform::initialize() {
  _pdfType = PdfDiscreteUniform;

  _min = -1;
  _max = -1;

  _infLeftTail = false;
  _infRightTail = false;
}


CPdfDiscreteUniform::~CPdfDiscreteUniform() {
  // Nothing to do here.
}


bool CPdfDiscreteUniform::validate( QString* errMsg /* = NULL */ ) {
  bool result = true;

  if( _min >= _max ) {
    if( NULL != errMsg )
      errMsg->append( "The maximum must be greater than the minimum.\n" );
      result = false;
  }

  return result;
}


bool CPdfDiscreteUniform::equals( const CPdf& other ) {
  if( (const_cast<CPdf&>(other)).pdfType() != _pdfType )
    return false;
  else {
    CPdfDiscreteUniform o = dynamic_cast<CPdfDiscreteUniform&>( const_cast<CPdf&>(other) );

    return(
      ( this->minParam() == o.minParam() )
      && ( this->maxParam() == o.maxParam() )
    );
  }
}


double CPdfDiscreteUniform::probDensity( double k ) {
  double result;
  int kd = int( k );

  if( ( kd > _max ) || ( kd < _min ) )
    result = 0.0;
  else {
    int range = abs( _max - _min ) + 1;
    result = 1.0/range;
  }

  return result;
}

double CPdfDiscreteUniform::rand( RAN_gen_t* rng ) {
  return rand( rng, _min, _max );
}

double CPdfDiscreteUniform::rand( RAN_gen_t* rng, int min, const int max ) {
  int range;
  int maxPlusOne;
  double d;
  int adj;
  int result;

  maxPlusOne = max + 1;

  if( 0 > min ) {
    adj = -1 * min;
    min = min + adj;
    maxPlusOne = maxPlusOne + adj;
  }
  else
    adj = 0;

  range = maxPlusOne - min;
  d = RAN_num( rng );

  result = int( trunc( d * range + min ) ) - adj;

  return result;
}
//-----------------------------------------------------------------------------



//-----------------------------------------------------------------------------
// Poisson PDF
//-----------------------------------------------------------------------------
CPdfPoisson::CPdfPoisson() : CPdfDiscrete() {
  initialize();
}

CPdfPoisson::CPdfPoisson( const double lambda ) : CPdfDiscrete() {
  initialize();
  _mean = lambda;
}

CPdfPoisson::CPdfPoisson( const CPdfPoisson& other ) : CPdfDiscrete( other ) {
  initialize();
  _mean = other._mean;
}

void CPdfPoisson::initialize() {
  _pdfType = PdfPoisson;

  _mean = NAN;

  _infLeftTail = false;
  _infRightTail = true;
}

CPdfPoisson::~CPdfPoisson() {
  // Nothing to do here.
}

bool CPdfPoisson::validate( QString* errMsg /* = NULL */ ) {
  bool result = true;

  if( isnan( _mean ) ) {
    if( NULL != errMsg )
      errMsg->append( "Mean must be specified.\n" );
    result = false;
  }
  else if( 0 >= _mean ) {
    if( NULL != errMsg )
      errMsg->append( "The mean must be greater than 0.\n" );
    result = false;
  }

  return result;
}


bool CPdfPoisson::equals( const CPdf& other ) {
  if( (const_cast<CPdf&>(other)).pdfType() != _pdfType )
    return false;
  else {
    CPdfPoisson o = dynamic_cast<CPdfPoisson&>( const_cast<CPdf&>(other) );

    return( this->lambda() == o.lambda() );
  }
}


double CPdfPoisson::min() {
  return 0;
}


double CPdfPoisson::max() {
  return INFINITY;
}


double CPdfPoisson::probDensity( const double k ) {
  return gsl_ran_poisson_pdf( int( k ), _mean );
}

double CPdfPoisson::rand( RAN_gen_t* rng ) {
  return gsl_ran_poisson( RAN_generator_as_gsl( rng ), _mean );
}

double CPdfPoisson::rand( RAN_gen_t* rng, const double mean ) {
  return gsl_ran_poisson( RAN_generator_as_gsl( rng ), mean );
}
//-----------------------------------------------------------------------------



void testPdfs( QString directoryForOutput /* = "" */ ) {
  QString path;

  if( directoryForOutput.isEmpty() )
    directoryForOutput = ".";

  RAN_gen_t* rng = RAN_new_generator( QDateTime::currentDateTime().toTime_t() );

//  CPdfBinomial binomial( 5, 0.15 );
//  path = QString( "%1/%2" ).arg( directoryForOutput ).arg( "binomial-test.csv" );
//  binomial.variatesToFile( path, 1000, rng );

//  CPdfPoisson poisson( 0.2 );
//  path = QString( "%1/%2" ).arg( directoryForOutput ).arg( "poisson-test.csv" );
//  poisson.variatesToFile( path, 1000, rng );

//  CPdfDiscreteUniform dunif( 1, 100 );
//  path = QString( "%1/%2" ).arg( directoryForOutput ).arg( "dunif-test.csv" );
//  dunif.variatesToFile( path, 1000, rng );

  // Multinomial
  QVector<double> p;
  p << 0.1 << 0.7 << 0.2;
  QVector<int> output;
  path = QString( "%1/%2" ).arg( directoryForOutput ).arg( "multinomial-test.csv" );
  QFile f( path );
  if( f.open( QFile::WriteOnly | QFile::Truncate ) ) {
    QTextStream ts( &f );
    for( int i = 0; i < 1000; ++i ) {
      CPdfBinomial::multinomialRand( rng, 5, p, output );
      ts << output[0] << "," << output[1] << "," << output[2] << endl;
    }
    f.close();
  }


  RAN_free_generator ( rng );
}
