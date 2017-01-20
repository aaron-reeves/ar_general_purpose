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


class CConfigBlock : public QMap<QString, QString> {
  public:
    CConfigBlock( const QString& name );
    CConfigBlock( const CConfigBlock& other );
    ~CConfigBlock();

    QString name() const { return _name; }
    bool removed() const { return _removed; }
    void setRemoved( const bool val ) { _removed = val; }

    void writeToStream( QTextStream* stream );
    void debug();

  protected:
    QString _name;
    bool _removed;
};

typedef QMapIterator<QString, QString> CConfigBlockIterator;

class CConfigFile {
  public:
    CConfigFile( QStringList* args );
    CConfigFile( const QString& configFileName );
    virtual ~CConfigFile();

    // Check only the FIRST REMAINING block with the indicated name.
    //--------------------------------------------------------------
    bool contains( QString blockName, QString key ) const;
    QString value( QString blockName, QString key ) const;

    // Check ALL blocks with the indicated name.
    //------------------------------------------
    // How many blocks with the specified name contain the key?
    int multiContains( QString blockName, QString key ) const;
    QStringList multiValues( QString blockName, QString key ) const;

    bool contains( QString blockName ) const;
    int multiContains( QString blockName ) const;

    virtual void debug( const bool showRemovedBlocks = true );

    virtual void writeToStream( QTextStream* stream );

    int result() { return _returnValue; }
    QString resultString() { return ConfigReturnCode::resultString( _returnValue ); }

    QString errorMessage() { return _errorMessage; }

    QString fileName() { return _fileName; }

    bool setWorkingDirectory();

  protected:
    void buildBasic( const QString& fn );
    int processBlock( QStringList strList );
    int processFile( QFile* file );
    int fillBlock( CConfigBlock* block, QStringList strList );

    QString _fileName;

    QList<CConfigBlock*>* _blockList;
    QMultiHash<QString, CConfigBlock*>* _blockHash;

    QString _errorMessage;
    int _returnValue;
};


#endif // CCONFIGFILE_H
