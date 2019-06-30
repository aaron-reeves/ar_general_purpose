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
    CConfigFile();
    CConfigFile( QStringList* args );
    CConfigFile( const QString& configFileName );
    CConfigFile( const CConfigFile& other );
    virtual ~CConfigFile();

    // FIXME: referencing individual blocks by index, rather than by name, may require some work in the code.

    // Does the file contain AT LEAST one block with the indicated name?
    bool contains( const QString& blockName ) const;
    
    // .. AT LEAST one block with the indicated key?
    bool contains( QString blockName, QString key ) const;
    
    // AT LEAST one block with the indicated key, set to the indicated value?
    bool contains( QString blockName, QString key, QString value ) const;

    // Return the FIRST block with the indicated name
    CConfigBlock* block( const QString& blockName ) const;
    
    // Return the value from the FIRST block with the indicated name
    QString value( QString blockName, QString key ) const;

    // How many blocks with the specified name?
    int multiContains( const QString& blockName ) const;
    
    // How many blocks with the specified name contain the key?
    int multiContains( const QString& blockName, QString key ) const;

    // Return a list of ALL blocks with the specified name
    QList<CConfigBlock*> blocks( const QString& blockName ) const;
    
    // Return the indicated key value from EVERY block with the specified name
    QStringList values( const QString& blockName, QString key ) const;

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

    QList<CConfigBlock*> _blockList;
    QMultiHash<QString, CConfigBlock*> _blockHash;

    QString _errorMessage;
    int _returnValue;
};


#endif // CCONFIGFILE_H
