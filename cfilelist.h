/*
cfilelist2.h/cpp
Begin: 2003/06/11
-----------------
Copyright (C) 2003 - 2017 by Aaron Reeves
aaron.reeves@naadsm.org

This program is free software; you can redistribute it and/or modify it under the terms of the GNU General
Public License as published by the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.
*/



#ifndef CFILELIST_H_DEFINED
#define CFILELIST_H_DEFINED

#include <QtCore>

void testFileList( const QString& directoryName, const QString& filter );


/**
This class provides specialized strings for dealing with file names and paths.  Functions for
manipulating file and path names exist for tasks such as removing path information, returning
the directory component of a file path, etc.

CPathStrings are closely related to and are used by CFileLists (see @ref CFileList).

@short Specialized strings for file names and paths
@author Aaron Reeves (aaron.reeves@naadsm.org)
@version 2.03
*/
class CPathString: public QString {
    public:
        /**
        Creates an empty CPathString
        */
        CPathString();

        /**
        Creates a new CPathString from a standard QString.

        @param str QString to make into a CPathString.
        */
        CPathString( const QString& str );

        /**
        Creates a new CQPathString from a char*.

        @param charstr char* used to create the CPathString.
        */
        CPathString( const char* charstr );

        /**
        Returns just the file name component of a path.  For example, if the path string is
        '/home/foo/bar.txt', shortFileName returns 'bar.txt'.

        @return QString containing the file name.
        */
        QString shortFileName() const;
        QString fileName() const { return shortFileName(); }

        /**
        Returns just the file name component of a path without a file type.  For example, if the path string is
        '/home/foo/bar.txt', baseName returns 'bar'.

        @return QString containing the file name. without the extension/type.
        */
        QString baseName() const;

        QString completeBaseName() const;
        
        /**
        Returns just the file name extension of a path.  For example, if the path string is
        '/home/foo/bar.txt', fileType returns 'txt'.

        @return QString containing the file name extension/type.
        */
        QString fileType() const;

        /**
        Returns the complete path.  For example, if the path string is '/home/foo/bar.txt',
        longFileName returns '/home/foo/bar.txt'.  This is exactly the same as returning
        '*this' for the CPathString, but is included for consistency among member function names.

        @return QString containing the complete path name.
        */
        QString longFileName() const;
        QString filePath() const { return longFileName(); }
        /**
        Returns just the directory path associated with a file.  For example, if the path string
        is '/home/foo/bar.txt', directory returns '/home/foo/'.  Note that the trailing slash is included.

        @return QString containing the directory path.
        */
        QString directory() const;
        QDir dir() const;
        
        /**
        Returns just the directory containing a file.  For example, if the path string is
        '/home/foo/bar.txt', shortDirectory returns 'foo'.  Note that slashes are not included.

        @return QString indicating the directory.
        */
        QString shortDirectory() const;

        QString removeRoot( QString oldRoot );

        QString replaceRoot( QString oldRoot, QString newRoot );

        /**
         * Returns a shorter path for the file.  If the path string is 'c:/foo/bar/bash/file.txt',
         * the following values are returned:
         * trimmedPath( 1 ): 'foo/bar/bash/file.txt'
         * trimmedPath( 2 ): 'bar/bash/file.txt'
         * trimmedPath( 3 ): 'bash/file.txt'
         * trimmedPath( 5 ): (empty string)
         * trimmedPath( 6 ): (raise exception)
         *
         * @param nPathsToTrim number of steps to trim off the path.
         * @return QString containing the file name, with nPathsToTrim fewer steps in the path
         */
        QString pathTrimmed( const int nPathsToTrim );

};

/**
This class provides a quick and convenient way to list all files or subdirectories within a given
directory.  Directory contents may be listed recursively, and file names may be matched to (right now)
very primitive file name filters.

The functionality of this class is similar to that provided by QDir::entryList(), except for the option
of recursive searches, the use of a true pointer list instead of a value list, and the enhanced
capabilities of CPathStrings over standard QStrings.

CFileList could be extended by including the ability to list files based on matches to some
regular expression, but this isn't yet an option.

@short Creates a list of files or subdirectories within a given directory
@author Aaron Reeves (aaron.reeves@naadsm.org)
@version 2.03
*/
class CFileList : public QList<CPathString> {
    public:
        /**
        Constructs an empty CQFileList.  By itself, this form of the constructor isn't very useful,
        although it is on occasion used internally by other class functions.

        see below for the more commonly used form of the constructor.
        */
        CFileList();

        /**
        Provides a list of all files that match 'filter' in the directory specified by 'path'.
        If 'recurse' is true, the directory contents will be recursively searched.

        @param path QString indicating the directory whose contents will be listed
        @param filter QString indicating the file name filter to match (e.g., "*.txt").  Use "*.*" to match all files.  More than one filter may be used, if semicolon-delimited (e.g., "*.cpp;*.h").
        @param recurse bool indicating whether to list directory contents recursively.
        */
        CFileList( const QString& path, const QString& filter,  const bool recurse );

        CFileList( const CFileList& other );
        CFileList& operator=( const CFileList& other );

        /**
        Destroys the file list.  Pointers to list elements (CPathStrings) are NOT deleted.

        Eventually, it might be nice to parameterize the destructor, so that the list may
        be emptied and all elements deleted as well, but it hasn't been an issue yet.
        */
        virtual ~CFileList();

        /**
        Returns a list of all of the directories that contain the files in 'this' list.  For example, if
        'this' list has the following elements:
        /home/foo/test1.txt
        /home/foo/test2.txt
        /home/bar/test3.txt

        this->directories() will return a list with the following elements:
        /home/foo/
        /home/bar/

        Not that 'this' list must exist first: there currently is no way to generate a list of only subdirectories
        directly, although this some day may be a nice addition.

        @return a CFileList containing only subdirectories
        */
        CFileList directories() const;

        CFileList files() const;

        /**
        Displays all of the items in 'this' list by printing them to the debug stream.
        */
        void debugList();
        void debug() { debugList(); }

        /**
        Converts the list to a QStringList.  See Qt documentation for
        more details.
        */
        QStringList qStringList() const;

        /**
        Combines the contents of this list and the contents of subList.
        */
        void merge( CFileList subList );

        void insert( const QString& file ) { insert( CPathString( file ) ); }

        /**
        This function does all of the actual work associated with generating list items and adding them to the list.
        Parameters are identical to the constructor.  File names that match 'filter' are appended to the list.
        If 'recurse' is true, this function is called recursively for each directory that it encounters.

        @param path QString indicating the directory whose contents will be listed
        @param filter QString indicating the file name filter to match (e.g., "*.txt").  Use "*.*" to match all files.  More than one filter may be used, if semicolon-delimited (e.g., "*.cpp;*.h").
        @param recurse bool indicating whether to list directory contents recursively
        */
        void getFileNames(const QString& path, const QString& filter, const bool recurse );

    private:
        CFileList( bool createDirList );

				QString _startingDir;
        QString _filter;

        CFileList* _fileList;
};

#endif //CFILELIST_H_DEFINED


