#ifndef CRANDOMIZER_H
#define CRANDOMIZER_H

#include <QtCore>

class CRandomizer {
  public:
    CRandomizer( const quint32 seed = 0 );
    ~CRandomizer();

    QString usCity() { return randListElement( _allUSCities ); }
    QString ukCity() { return randListElement( _allUKCities ); }
    QString color() { return randListElement( _allColors ); }
    QString country() { return randListElement( _allCountries ); }
    QString food() { return randListElement( _allFoods ); }
    QString planet() { return randListElement( _allPlanets ); }
    QString planetExt() { return randListElement( _allPlanetsExt ); }

    QString alphanumeric5();
    QString alphanumeric( const int length );
    int randInt( const int max );
    int rantInt( const int min, const int max );

    QStringList randUSCities( const int length ) { return randList( length, _allUSCities ); }
    QStringList randUKCities( const int length ) { return randList( length, _allUKCities ); }
    QStringList randColors( const int length ) { return randList( length, _allColors ); }
    QStringList randCountries( const int length ) { return randList( length, _allCountries ); }
    QStringList randFoods( const int length ) { return randList( length, _allFoods ); }
    QStringList randPlanets( const int length ) { return randList( length, _allPlanets ); }
    QStringList randPlanetsExt( const int length ) { return randList( length, _allPlanetsExt ); }

    QStringList randAlphanumeric5s( const int length );
    QList<int> randInts( const int length , const int min, const int max );

  protected:
    QStringList randList( const int length, const QStringList& sourceList );
    QString randListElement( const QStringList& sourceList ) { return( sourceList.at( _rng.bounded( sourceList.count() ) ) ); }

    QRandomGenerator _rng;
    quint32 _seed;

    void buildUSCityList();
    void buildUKCityList();
    void buildColorList();
    void buildCountryList();
    void buildFoodList();
    void buildPlanetList();
    void buildPlanetListExt();

    QStringList _allUSCities;
    QStringList _allUKCities;
    QStringList _allColors;
    QStringList _allCountries;
    QStringList _allFoods;
    QStringList _allPlanets;
    QStringList _allPlanetsExt;
};

#endif // CRANDOMIZER_H
