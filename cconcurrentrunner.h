#ifndef CCONCURRENTRUNNER_H
#define CCONCURRENTRUNNER_H

#include <QtCore>
#include <QtConcurrent>

#include <ar_general_purpose/returncodes.h>
#include <epic_general_purpose/cepicconfigfile.h>

class CConcurrencyManager {
  public:
    CConcurrencyManager();
    virtual ~CConcurrencyManager();

    void waitForOneToFinish();

    // Override this function to do anything useful.
    virtual QHash<QString, int> populateDatabase( const CConfigDatabase& cfdb, const int dataSourceID, const bool insertRecords );

  private:
    Q_DISABLE_COPY( CConcurrencyManager )
};


class CConcurrentRunner {
  public:
    CConcurrentRunner(CConcurrencyManager* list, const QFuture<QHash<QString, int> >& f );
    ~CConcurrentRunner();

    qint64 runtime() const; // in milliseconds. -1 if process hasn't yet ended.

    void waitForFinished();
    bool finished() const;
    bool isFinished();
    QHash<QString, int> result() const;

    static int adjustMaxListSize(
      const bool threadsFull,
      const QList<int>& maxListSize,
      const int idealThreadCount,
      const QList<int>& threadsInUse,
      const QList<bool>& backlog
    );

    static void writeUsage(
      const QList<int>& maxListSize,
      const QList<bool>& backlog,
      const QList<qint64>& waitTime,
      const QList<int>& threadsInUse
    );

  protected:
    void cleanup();

    QFuture< QHash<QString, int> > _future;
    CConcurrencyManager* _list;
    bool _hasFinished;
    QElapsedTimer _timer;
    qint64 _timeInMsec;

  private:
    Q_DISABLE_COPY( CConcurrentRunner )
};

#endif // CCONCURRENTRUNNER_H
