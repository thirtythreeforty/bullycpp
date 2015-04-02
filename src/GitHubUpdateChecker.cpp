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

#include "GitHubUpdateChecker.h"

#include <stdexcept>

#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonValue>
#include <QJsonParseError>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QRegularExpression>
#include <QRegularExpressionMatch>

GitHubUpdateChecker::GitHubUpdateChecker(const QString& maintainer, const QString& repo, const QString& thisVersion, QNetworkAccessManager* accessManager, QObject *parent)
	: QObject(parent)
	, networkAccessManager(accessManager ? accessManager : new QNetworkAccessManager(this))
	, softFailures(0)
	, checkInProgress(false)
{
	if(!parseSemanticVersionString(thisVersion, currentMajor, currentMinor, currentPatch))
		throw std::invalid_argument("Bad version string; must be in semantic versioning form");

	const QString& urlString = QStringLiteral("https://api.github.com/repos/%1/%2/releases");
	url.setUrl(urlString.arg(maintainer, repo));
	if(!url.isValid())
		throw std::invalid_argument("Bad maintainer or repo; must be valid url part");

	connect(networkAccessManager, &QNetworkAccessManager::finished, this, &GitHubUpdateChecker::onRequestFinished);
}

GitHubUpdateChecker::~GitHubUpdateChecker()
{}

void GitHubUpdateChecker::checkForUpdate()
{
	if(checkInProgress)
		return;

	softFailures = 0;
	checkInProgress = true;
	performCheck();
}

void GitHubUpdateChecker::onRequestFinished(QNetworkReply* reply)
{
	reply->deleteLater();

	if(reply->error() != QNetworkReply::NoError) {
		++softFailures;
		performCheck();
		return;
	}

	// Parse the reply and look for the latest release.
	QJsonParseError error;
	QJsonArray array = QJsonDocument::fromJson(reply->readAll(), &error).array();
	if(error.error != QJsonParseError::NoError) {
		++softFailures;
		performCheck();
		return;
	}
	if(!array.size()) {
		++softFailures;
		performCheck();
		return;
	}

	// Find the first release that isn't a pre-release
	// and that has a valid semantic version string in the name.
	for(const auto& val: array) {
		QJsonObject topRelease = val.toObject();
		QJsonValue name = topRelease["name"];
		QJsonValue html_url = topRelease["html_url"];
		QJsonValue prerelease = topRelease["prerelease"];

		if(name.type() != QJsonValue::String ||
		   html_url.type() != QJsonValue::String ||
		   prerelease.type() != QJsonValue::Bool
		) {
			++softFailures;
			performCheck();
			return;
		}

		if(prerelease.toBool())
			continue;

		unsigned int major, minor, patch;
		if(!parseSemanticVersionString(name.toString(), major, minor, patch))
			continue;

		if(major > currentMajor ||
		   (major == currentMajor && minor > currentMinor) ||
		   (major == currentMajor && minor == currentMinor && patch > currentPatch)
		) {
			// New release!
			emit updateAvailable(name.toString(), html_url.toString());
		}
		break;
	}

	checkInProgress = false;
}

bool GitHubUpdateChecker::parseSemanticVersionString(const QString &str, unsigned int &major, unsigned int &minor, unsigned int &patch) const
{
	QRegularExpression re(R"(v?(\d+)\.(\d+)(?:\.(\d+))?)");
	QRegularExpressionMatch match = re.match(str);
	if(match.hasMatch()) {
		major = match.captured(1).toUInt();
		minor = match.captured(2).toUInt();
		patch = match.captured(3).isNull() ? 0 : match.captured(3).toUInt();
		return true;
	}
	else
		return false;
}

void GitHubUpdateChecker::performCheck()
{
	if(softFailures >= maxTries) {
		emit checkFailed();
		checkInProgress = false;
		return;
	}

	if(networkAccessManager->networkAccessible())
		networkAccessManager->get(QNetworkRequest(url));
	else {
		emit checkFailed();
		checkInProgress = false;
		return;
	}
}
