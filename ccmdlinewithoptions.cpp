
#include "ccmdlinewithoptions.h"

void CCmdLineWithQOptions::initialize() {
  _helpFn = nullptr;
  _versionFn = nullptr;
  _hasHelpOption = false;
  _hasVersionOption = false;
}


void CCmdLineWithQOptions::assign( const CCmdLineWithQOptions& other ) {
  CCmdLine::assign( other );

  _optionList = other._optionList;

  _helpFn = other._helpFn;
  _versionFn = other._versionFn;

  _hasHelpOption = other._hasHelpOption;
  _hasVersionOption = other._hasVersionOption;

  _acceptedArgNames = other._acceptedArgNames;
}


void CCmdLineWithQOptions::addHelpOption( DisplayMessageFn helpFn /* = nullptr */ ) {
  _helpFn = helpFn;
  _hasHelpOption = true;
  addOption(
    QARCommandLineOption(
      QStringList() << QStringLiteral("--help") << QStringLiteral("-h") << QStringLiteral("-?"),
      QStringLiteral("Display this help message.")
    )
  );
}


void CCmdLineWithQOptions::addVersionOption( DisplayMessageFn versionFn /* = nullptr */ ) {
  _versionFn = versionFn;
  _hasVersionOption = true;
  addOption(
    QARCommandLineOption(
      QStringList() << QStringLiteral("--version") << QStringLiteral("-v"),
      QStringLiteral("Display the application version.")
    )
  );
}


void CCmdLineWithQOptions::addOption( const QARCommandLineOption& opt ) {
  foreach( const QString& argName, opt.names() ) {
    _acceptedArgNames.insert( formatArg( argName ) );
  }

  _optionList.append( opt );
}


void CCmdLineWithQOptions::showVersion() const {
  if( nullptr != _versionFn ) {
    _versionFn();
    return;
  }

  cout << QCoreApplication::applicationName() << " " << QCoreApplication::applicationVersion() << endl;
}


void CCmdLineWithQOptions::showHelp() const {
  if( nullptr != _helpFn ) {
    _helpFn();
    return;
  }

  showVersion();

  CHelpItemList hList;

  for( int i = 0; i < _optionList.count(); ++i ) {
    QARCommandLineOption opt = _optionList.at(i);

    QStringList optNames;
    QString optNamesStr;
    for( int j = 0; j < opt.names().count(); ++j ) {
      QString optName =  opt.names().at(j);

      // I don't think this block is necessary any more.
      // FIXME: delete this the next time through.
      //if( 1 == optName.length() ) {
      //  optName.prepend( '-' );
      //}
      //else {
      //  optName.prepend( "--" );
      //}

      QStringList valueNames;
      QString valueNamesStr;
      for( int k = 0; k < opt.valueNames().count(); ++k ) {
        if( !opt.valueNames().at(k).trimmed().isEmpty() ) {
          valueNames.append( QStringLiteral( "<%1>" ).arg( opt.valueNames().at(k).trimmed() ) );
        }
      }
      valueNamesStr = valueNames.join( ' ' );
      if( !valueNamesStr.isEmpty() ) {
        optNames.append( QStringLiteral( "%1 %2" ).arg( optName, valueNamesStr ) );
      }
      else {
        optNames.append( optName );
      }
    }
    optNamesStr = optNames.join( QStringLiteral( ", ") );

    hList.append( optNamesStr, opt.description() );
  }

  hList.printHelpList();
}


void CCmdLineWithQOptions::generatePairs() {
  foreach( const QARCommandLineOption& opt, _optionList ) {
    QStringList args = opt.names();

    QList<QString>::const_iterator str1;
    QList<QString>::const_iterator str2;
    for( str1 = args.constBegin(); str1 != args.constEnd(); ++str1 ) {
      for( str2 = args.constBegin(); str2 != args.constEnd(); ++str2 ) {
        pair( *str1, *str2 );
      }
    }
  }
}


bool CCmdLineWithQOptions::process( const QCoreApplication& app ) {
  QStringList args = app.arguments();

  int argc = args.count();

  char** argv = new char* [ argc ];

  for( int i = 0; i < argc; ++i ) {
    argv[i] = new char[args.at(i).length()+1];
    strcpy( argv[i], args.at(i).toLatin1().data() );
  }

  splitLine( argc, argv, false );

  //this->debug();

  delete [] argv;

  bool argsOK = true;
  QStringList providedArgNames = _arguments.keys();

  // Check for any unrecognized arguments.  If found, return false.
  foreach( const QString& arg, providedArgNames ) {
    if( !_acceptedArgNames.contains( formatArg( arg ) ) ) {
      cout << "Unrecognized argument: " << formatArg( arg ) << endl;
      argsOK = false;
    }
  }

  // Check for any duplications.  If found, return false.
  int nMatches;
  foreach( const QARCommandLineOption& opt, _optionList ) {
    QStringList args = opt.names();

    nMatches = 0;
    foreach( const QString& arg, args ) {
      if( providedArgNames.contains( arg ) ) {
        ++nMatches;
      }
    }
    if( 1 < nMatches ) {
      cout << "Ambiguous or unnecessary argument duplication: " << opt.names().at(0) << endl;
      argsOK = false;
    }
  }

  if( !argsOK ) {
    return false;
  }
  else if( _hasHelpOption && hasHelp() ) {
    showHelp();
    exit( 0 );
  }
  else if( _hasVersionOption && hasVersion() ) {
    showVersion();
    exit( 0 );
  }

  // Generate pairs so that the application recognizes, e.g., "--version" and "-v" as the same thing.
  generatePairs();

  return true;
}

