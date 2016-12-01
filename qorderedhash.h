#ifndef QORDEREDHASH_H
#define QORDEREDHASH_H

#include <QtCore>

template <typename Key, typename T>
class QOrderedHash {
  public:
    QOrderedHash();
    QOrderedHash( const QOrderedHash<Key, T>& other );
    ~QOrderedHash();

    const T at( int i ) const;
    void clear();
    bool containsKey( const Key& key ) const;
    bool containsValue( const T& value ) const;
    int count() const;
    int countByKey( const Key& key ) const;
    int countByValue( const T& value ) const;
    void insert( const Key& key, const T& value );
    bool isEmpty() const;
    QList<Key> keys() const;
    int removeKey( const Key& key );
    int removeAllValues( const T& value );
    const T value( const Key& key ) const;
    QList<T> values() const;

  protected:
    QList<Key> _keys;
    QList<T> _values;
    QHash<Key, T> _hash;
};

template <typename Key, typename T>
class QOrderedHashIterator {
  public:
    QOrderedHashIterator();

};

void testQOrderedHash();

#endif // QORDEREDHASH_H
