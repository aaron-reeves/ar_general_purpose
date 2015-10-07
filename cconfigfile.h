#ifndef CCONFIGFILE_H
#define CCONFIGFILE_H

#include <QtCore>

/*
 * Exit codes returned by the application.  Use --help for definitions.
 */
class ConfigReturnCode {
  public:
    enum returnCodes {
      Success,
      SuccessWithBadRows,  // Make sure that this is always the last "success" message.  Other bits of code rely on it!
      BadArguments,
      UnrecognizedFunction,
      MissingConfigFile,
      CannotOpenConfigFile,
      BadConfiguration,
      BadFunctionConfiguration,
      BadDatabaseConfiguration,
      BadDatabaseSchema,
      MissingInstructions,
      EmptyInputFile,
      CannotOpenInputFile,
      QueryDidNotExecute,
      CannotOpenOutputFile,
      BadFileFormat,
      BadDataField,

      // Insert any new codes here.

      LastReturnCode // Not a real return code.  Used to easily iterate over all return codes.
    };

    static QString resultString( const int& returnCode );
};


class CConfigFile {
  public:
    CConfigFile( QStringList* args );
    CConfigFile( const QString& configFileName );
    virtual ~CConfigFile();

    bool contains( const QString& blockName, const QString& key ) const;
    QString value( const QString& blockName, const QString& key ) const;

    virtual void debug();

    int result() { return _returnValue; }
    QString resultString() { return ConfigReturnCode::resultString( _returnValue ); }

    QString errorMessage() { return _errorMessage; }

  protected:
    void buildBasic( const QString& fn );
    int processBlock( QStringList strList );
    int processFile( QFile* file );
    int fillBlock( QHash<QString, QString>* block, QStringList strList );

    QHash< QString, QHash<QString, QString>* >* _blocks;

    QString _errorMessage;
    int _returnValue;
};


#endif // CCONFIGFILE_H
