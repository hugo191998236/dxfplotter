#pragma once

#include <model/document.h>
#include <config/config.h>

#include <QObject>

#include <fstream>

namespace Importer::Dxf
{
	class Importer;
}

namespace Model
{

class Application : public QObject
{
	Q_OBJECT;

private:
	/// Global configuration
	Config::Config m_config;

	const Config::Tools::Tool *m_defaultToolConfig;
	const Config::Profiles::Profile *m_defaultProfileConfig;

	// Absolute file basename of current loaded file
	QString m_currentFileBaseName;

	Document::UPtr m_openedDocument;

	PathSettings defaultPathSettings() const;

	std::optional<const Config::Tools::Tool *> findTool(const std::string &name) const;
	std::optional<const Config::Profiles::Profile *> findProfile(const std::string &name) const;

	void cutterCompensation(float scale);

	Task::UPtr createTaskFromDxfImporter(const Importer::Dxf::Importer& importer);

	template <class Exporter>
	bool saveToFile(Exporter &exporter, const QString &fileName)
	{
		std::ofstream output(fileName.toStdString());
		if (output) {
			exporter(*m_openedDocument, output);
			return true;
		}

		return false;
	}

public:
	explicit Application();

	Config::Config &config();
	void setConfig(Config::Config &&config);

	/// Select tool used as configuration for further operations
	bool selectTool(const QString &toolName);
	void defaultToolFromCmd(const QString &toolName);

	/// Select profile used as configuration for further operations
	bool selectProfile(const QString &profileName);
	void defaultProfileFromCmd(const QString &profileName);

	QString currentFileBaseName() const;
	void loadFileFromCmd(const QString &fileName);
	bool loadFile(const QString &fileName);
	bool loadFromDxf(const QString &fileName);
	bool loadFromDxfplot(const QString &fileName);

	bool saveToGcode(const QString &fileName);
	bool saveToDxfplot(const QString &fileName);

	void leftCutterCompensation();
	void rightCutterCompensation();
	void resetCutterCompensation();

	void hideSelection();
	void showHidden();

Q_SIGNALS:
	void documentChanged(Document *newDocument);
	void titleChanged(QString title);
	void configChanged(Config::Config &config);
	void errorRaised(const QString& message);
};

}
