#ifndef CMAGIC8BALL_H
#define CMAGIC8BALL_H

#include <QtCore>
#include <naadsm/rng.h>

class CMagic8Ball {
  public:
    enum Magic8BallType {
      Undefined,
      Number,
      Proportion
    };

    CMagic8Ball( const int number );
    CMagic8Ball( const double proportion );
    CMagic8Ball(const QString& parameters );
    ~CMagic8Ball();

    bool answer( const int number );

    QString errorMsg() const { return _errorMsg; }
    bool error() const { return _error; }

  protected:
    void setNumberType( const int number );
    void setProportionType( const double proportion );

    RAN_gen_t* _rng;

    int _type;
    int _number;
    double _proportion;

    bool _error;
    QString _errorMsg;

  private:
    Q_DISABLE_COPY( CMagic8Ball )
};

#endif // CMAGIC8BALL_H
