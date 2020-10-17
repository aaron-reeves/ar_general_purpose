/*
ccmdlinewithoptions.h/cpp
-------------------------
Begin: 2019-09-05
Author: Aaron Reeves <aaron.reeves@sruc.ac.uk>
---------------------------------------------------
Copyright (C) 2019 Scotland's Rural College (SRUC)

This program is free software; you can redistribute it and/or modify it under the terms of the GNU General
Public License as published by the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.
*/

#ifndef CCMDLINEWITHOPTIONS_H_
#define CCMDLINEWITHOPTIONS_H_

#include "ccmdline.h"
#include "qarcommandlineoption.h"

typedef void (*DisplayMessageFn)();

class CCmdLineWithQOptions : public CCmdLine {
  public:
    CCmdLineWithQOptions() : CCmdLine() { initialize(); }
    CCmdLineWithQOptions( int argc, char** argv, bool clearArgs = true ) : CCmdLine( argc, argv, clearArgs ) { initialize(); }
    CCmdLineWithQOptions( const QString& fileName ) : CCmdLine( fileName ) { initialize(); }
    CCmdLineWithQOptions( const CCmdLineWithQOptions& other ) : CCmdLine( other ) { assign( other ); }
    CCmdLineWithQOptions& operator=( const CCmdLineWithQOptions& other ) { assign( other ); return *this; }
    ~CCmdLineWithQOptions() { /* Nothing else to do here */ }

    void setSingleDashWordOptionMode( const int dummy ) const { Q_UNUSED( dummy ) }

    void addHelpOption( DisplayMessageFn helpFn = nullptr );
    void addVersionOption( DisplayMessageFn versionFn = nullptr );

    void addOption( const QARCommandLineOption& opt );

    bool process( const QCoreApplication& app );

    void showHelp() const;
    void showVersion() const;

  protected:
    void initialize();
    void assign( const CCmdLineWithQOptions& other );

    void generatePairs();

    QSet<QString> _acceptedArgNames;

    bool _hasHelpOption;
    DisplayMessageFn _helpFn;
    bool _hasVersionOption;
    DisplayMessageFn _versionFn;

    QList<QARCommandLineOption> _optionList;
};


#endif //CCMDLINEWITHOPTIONS_H_

