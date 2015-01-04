/** This file is part of BullyCPP.
 **
 **     BullyCPP is free software: you can redistribute it and/or modify
 **     it under the terms of the GNU General Public License as published by
 **     the Free Software Foundation, either version 3 of the License, or
 **     (at your option) any later version.
 **
 **     BullyCPP is distributed in the hope that it will be useful,
 **     but WITHOUT ANY WARRANTY; without even the implied warranty of
 **     MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 **     GNU General Public License for more details.
 **
 **     You should have received a copy of the GNU General Public License
 **     along with BullyCPP.  If not, see <http://www.gnu.org/licenses/>.
 **/

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
	void onRequestFinished(QNetworkReply* reply);

private:
	void performCheck();
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
