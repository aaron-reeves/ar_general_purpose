
#include <qmath.h>

void inc( int& addTo ) {
  ++addTo;
}

void dec( int& subtractFrom ) {
  --subtractFrom;
}

void inc( int& addTo, const int toAdd ) {
  addTo = addTo + toAdd;
}

void dec( int& subtractFrom, const int toSubstract ) {
  subtractFrom = subtractFrom - toSubstract;
}


bool qBetterFuzzyCompare( const double p1, const double p2){
  if (qFuzzyIsNull(p1)) {
    return qFuzzyIsNull(p2);
  } else if (qFuzzyIsNull(p2)) {
    return false;
  } else {
    return (qAbs(p1 - p2) <= 0.000000000001 * qMin(qAbs(p1), qAbs(p2)));
  }
}