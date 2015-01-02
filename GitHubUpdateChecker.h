#ifndef UPDATECHECKER_H
#define UPDATECHECKER_H

#include <QObject>
#include <QNetworkAccessManager>
#include <QString>
#include <QUrl>

class GitHubUpdateChecker : public QObject
{
	Q_OBJECT
public:
	explicit GitHubUpdateChecker(const QString& maintainer, const QString& repo, const QString& thisVersion, QNetworkAccessManager* accessManager = nullptr, QObject *parent = nullptr);
	virtual ~GitHubUpdateChecker();

signals:
	void updateAvailable(QString newVersion, QString url);
	void checkFailed();

public slots:
	void checkForUpdate();

private slots:
	void performCheck();
	void onRequestFinished(QNetworkReply* reply);

private:
	bool parseSemanticVersionString(const QString& str, unsigned int& major, unsigned int& minor, unsigned int& patch) const;

private:
	QNetworkAccessManager* networkAccessManager;
	unsigned int softFailures;
	bool checkInProgress;
	QUrl url;
	unsigned int currentMajor;
	unsigned int currentMinor;
	unsigned int currentPatch;

	// The maximum number of soft failures (bad JSON, etc.) before
	// we give up.
	const static unsigned int maxTries = 2;
};

#endif // UPDATECHECKER_H
