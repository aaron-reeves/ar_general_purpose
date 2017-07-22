#ifndef CPDFS_H
#define CPDFS_H

#include <QtCore>

#include <naadsm/rng.h>

class CPdf {
  public:
    // Enumeration of the types of Probability Density Functions supported
    enum PdfType {
      PdfUndefined,

      // Continuous types
      //-----------------
      PdfBeta,
      PdfBetaPERT,
      PdfExponential,
      PdfGamma,
      PdfGaussian,
      PdfHistogram,
      PdfInverseGaussian,     // needs CDF, inverse CDF
      PdfLogistic,
      PdfLogLogistic,
      PdfLognormal,
      PdfPareto,
      PdfPearson5,
      PdfPoint,
      PdfPiecewise,
      PdfTriangular,
      PdfUniform,
      PdfWeibull,

      // Discrete types
      //---------------
      PdfDiscretePoint,
      PdfBernoulli,
      PdfBinomial,
      PdfDiscreteUniform,
      PdfHypergeometric,
      PdfNegativeBinomial,
      PdfPoisson,

      // Specialized types
      //------------------
      PdfSpGaniTalbert // A discrete type: see unit PdfGaniTalbert
    };

  public:
    CPdf();
    CPdf( const CPdf& other );
    virtual ~CPdf();

    virtual bool equals( const CPdf& other ) = 0;
    virtual QString descr() = 0;
    virtual bool validate( QString* errMsg = NULL );

    virtual double probDensity( const double x ) = 0;
    //virtual double cumulativeDistr( double x ) = 0;
    //virtual double invCumulative( double p ) = 0;
    virtual double rand( RAN_gen_t* rng ) = 0;
    int randInt( RAN_gen_t* rng );
//    double discreteProb( int x );
//    double discreteCumulativeProb( int x );

    PdfType pdfType() { return _pdfType; }
    virtual bool isDiscrete() = 0;
    bool isContinuous() { return !isDiscrete(); }

    bool infiniteLeftTail() { return _infLeftTail; }
    bool infinateRightTail() { return _infRightTail; }

    virtual bool hasMean() = 0;
    virtual double mean() = 0;

    bool hasMin() { return !_infLeftTail; }
    bool hasMax() { return !_infRightTail; }

    virtual double min() = 0;
    virtual double max() = 0;
//    virtual int minD();
//    virtual int maxD();

    QVector<double> variates( const int nVariates, RAN_gen_t* rng = NULL );

    void variatesToFile( const QString& fileName, const int nVariates, RAN_gen_t* rng /* = NULL */ );
    void debug() { qDebug() << descr(); }

    static QString pdfTypeDescr( PdfType type );

  protected:
    PdfType _pdfType;

    bool _infLeftTail;
    bool _infRightTail;

    virtual void initialize() = 0;

};

inline bool operator==( CPdf& lhs, const CPdf& rhs ){ return( lhs.equals( rhs ) ); }
inline bool operator!=( CPdf& lhs, const CPdf& rhs ){return !( lhs.equals( rhs ) ); }


class CPdfContinuous : public CPdf {
  public:
    virtual bool isDiscrete() { return false; }
};

class CPdfDiscrete : public CPdf {
  public:
    CPdfDiscrete();
    CPdfDiscrete( const CPdfDiscrete& other );
    virtual ~CPdfDiscrete();
    virtual bool isDiscrete() { return true; }

    int minD() { if( hasMin() ) return int( min() ); else return INT_MIN; }
    int maxD() { if( hasMax() ) return int( max() ); else return INT_MAX; }

//    virtual double invCumulative( double p );
//    int randInvCumulative( RAN_gen_t* rng );
};


class CPdfBinomial : public CPdfDiscrete {
  public:
    CPdfBinomial();
    CPdfBinomial( const int n, const double p );
    CPdfBinomial( const CPdfBinomial& other );
    virtual ~CPdfBinomial();

    virtual QString descr() { return( QString( "Binomial ( %1, %2 )" ).arg( _n ).arg( _p ) ); }
    virtual double mean() { return _n * _p; }
    virtual bool hasMean() { return true; }
    virtual double min() { return 0.0; }
    virtual double max() { return _n; }
    virtual bool validate( QString* errMsg = NULL );
    virtual bool equals( const CPdf& other );

    virtual double probDensity( const double k );
    virtual double rand( RAN_gen_t* rng );

    void setParams( const int n, const double p ) { _n = n; _p = p; }
    void setN( const int n ) { _n = n; }
    void setP( const double p ) { _p = p; }
    int n() { return _n; }
    double p() { return _p; }

    static double rand( RAN_gen_t* rng, const int n, const double p );
    static void multinomialRand( RAN_gen_t* rng, int n, const QVector<double>& p, QVector<int>& output );
    static void multinomialRandDebug( RAN_gen_t* rng, int n, const QVector<double>& p, QVector<int>& output );

  protected:
    virtual void initialize();

    int _n;
    double _p;
};


class CPdfDiscretePoint : public CPdfDiscrete {
  public:
    CPdfDiscretePoint() : CPdfDiscrete() { initialize(); }
    CPdfDiscretePoint( const int value ) : CPdfDiscrete() { initialize(); setValue( value ); }
    CPdfDiscretePoint( const CPdfDiscretePoint& other ) : CPdfDiscrete( other ) { initialize(); setValue( other._value ); }
    virtual ~CPdfDiscretePoint() {}

    virtual QString descr() { return QString( "Discrete point ( %1 )" ).arg( _value ); }
    virtual double mean() { return _value; }
    virtual bool hasMean() { return true; }
    virtual double min() { return _value; }
    virtual double max() { return _value; }
    virtual bool validate( QString* errMsg = NULL ) { Q_UNUSED( errMsg ); return true; } // FIXME: For this and other integer types, implement "value is set" flags.
    virtual bool equals( const CPdf& other );

    void setValue( const int value ) { _value = value; }
    int value() { return _value; }

    virtual double probDensity( const double k ) {  Q_UNUSED( k ); return NAN; }
    virtual double rand( RAN_gen_t* rng )  { Q_UNUSED( rng ); return _value; }
    static double rand( RAN_gen_t* rng, const int value ) { Q_UNUSED( rng ); return value; }

  protected:
    virtual void initialize() {   _pdfType = PdfDiscretePoint; setValue( -1 ); }

    int _value;
};


class CPdfDiscreteUniform : public CPdfDiscrete {
  public:
    CPdfDiscreteUniform();
    CPdfDiscreteUniform( const int min, const int max );
    CPdfDiscreteUniform( const CPdfDiscreteUniform& other );
    virtual ~CPdfDiscreteUniform();

    virtual QString descr() { return QString( "Discrete uniform ( %1, %2 )" ).arg( _min ).arg( _max ); }
    virtual double mean() { return( (double(_max) - double( _min ))/2.0 ); }
    virtual bool hasMean() { return true; }
    virtual double min() { return _min; }
    virtual double max() { return _max; }
    virtual bool validate( QString* errMsg = NULL );
    virtual bool equals( const CPdf& other );

    void setParams( const int min, const int max ) { _min = min; _max = max; }
    void setMin( const int min ) { _min = min; }
    void setMax( const int max ) { _max = max; }
    int minParam() { return _min; }
    int maxParam() { return _max; }

    virtual double probDensity( const double k );
    virtual double rand( RAN_gen_t* rng );
    static double rand( RAN_gen_t* rng, int min, const int max );

  protected:
    virtual void initialize();

    int _min;
    int _max;
};


class CPdfPoisson : public CPdfDiscrete {
  public:
    CPdfPoisson();
    CPdfPoisson( const double lambda );
    CPdfPoisson( const CPdfPoisson& other );
    virtual ~CPdfPoisson();

    virtual QString descr() { return QString( "Poisson ( %1 )" ).arg( _mean ); }
    virtual double mean() { return _mean; }
    virtual bool hasMean() { return true; }
    virtual double min();
    virtual double max();
    virtual bool validate( QString* errMsg = NULL );
    virtual bool equals( const CPdf& other );

    double lambda() { return _mean; }
    void setLambda( const double mean ) { _mean = mean; }

    virtual double probDensity( const double k );
    virtual double rand( RAN_gen_t* rng );
    static double rand( RAN_gen_t* rng, const double mean );

  protected:
    virtual void initialize();

    double _mean;
};


void testPdfs( QString directoryForOutput = "" );



#endif // CPDFS_H
