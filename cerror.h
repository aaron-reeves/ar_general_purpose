#include <QtCore>

namespace ErrorReturnValue {
  enum Result {
    NO_ERROR = 0x0000,                 //    0
    BAD_COMMAND = 0x0001,              //    1
    INPUT_FILE_PROBLEM = 0x0002,       //    2
    DATA_VALIDATION_PROBLEM = 0x0004,  //    4
    OUTPUT_FILE_PROBLEM = 0x0008,      //    8
    ERROR_LOG_PROBLEM = 0x0010,        //   16
    ERROR_VARIANT_CONVERSION = 0x0020, //   32
    PROCESSING_INTERRUPTED = 0x0040,   //   64
    FAILED_DB_QUERY = 0x0080,          //  128
    BAD_CONFIG = 0x0100,               //  256
    FILE_SYSTEM_PROBLEM = 0x0200,      //  512
    REQUIRED_FIELDS_MISSING = 0x0400,  // 1024
    BAD_DATABASE = 0x0800,             // 2048
    UNSPECIFIED_ERROR = 0x1000         // 4096
    //NEXT_PROBLEM = 0x2000
    //...
    // Don't forget to update CErrorHandler.errors() if new values are added.
  };
}


class CError {
  public:
    enum ErrorLevel {
      NoError,
      Unspecified,
      Information,
      Question,
      Warning,
      Critical,
      Fatal
    };

    CError();
    CError( const ErrorLevel level, const QString& msg );
    CError( const CError& other );
    CError& operator=( const CError& other );

    ErrorLevel level() const { return _level; }
    QString msg() const { return _msg; }

    QString levelAsString();
    static QString levelAsString( const ErrorLevel level );

  protected:
    ErrorLevel _level;
    QString _msg;
};


class CErrorList {
  public:
    CErrorList();
    void clear();
    int count();
    QString at( const int i );
    CError itemAt( const int i );
    void append( CError err );
    void append( CError::ErrorLevel level, const QString& msg );
    void append( CErrorList src );
    QString asText();
    
  protected:
    QList<CError> _list;
};


class CErrorHandler {
  public:
    CErrorHandler( const bool writeErrorLog, const QString& errorLogFilename, const bool autoWriteErrorLog = true );
    ~CErrorHandler();

    QString errors();
    int maxErrorLevel() { return _maxErrorLevel; }

  protected:
    void writeErrorLogFile();
    void handleError( CError::ErrorLevel type, const QString& msg, const ErrorReturnValue::Result result = ErrorReturnValue::UNSPECIFIED_ERROR );
    //void handleError( const CErrorList& msgs );

   int _maxErrorLevel;
   int _result;
   CErrorList _errMsgs;
   bool _writeErrorLog;
   bool _autoWriteErrorLog;
   QString _errorLogFilename;
};
