#include <path.h>

namespace View::Task
{

void Path::setupModel()
{
	m_groupSettings.reset(new Model::PathGroupSettings(task()));

	hide();

	connect(task(), &Model::Task::selectionChanged, this, &Path::selectionChanged);

	connectOnFieldChanged<double>(planeFeedRate, [this](double value) { m_groupSettings->setPlaneFeedRate(value); });
	connectOnFieldChanged<double>(depthFeedRate, [this](double value) { m_groupSettings->setDepthFeedRate(value); });
	connectOnFieldChanged<double>(intensity, [this](double value) { m_groupSettings->setIntensity(value); });
	connectOnFieldChanged<double>(Ui::Path::depth, [this](double value) { m_groupSettings->setDepth(value); });
}

void Path::taskChanged()
{
	setupModel();
}

Path::Path(Model::Application &app)
	:TaskModelObserver(app)
{
	setupUi(this);
}

void Path::selectionChanged(int size)
{
	if (size > 0) {
		show();

		updateFieldValue(planeFeedRate, m_groupSettings->planeFeedRate());
		updateFieldValue(depthFeedRate, m_groupSettings->depthFeedRate());
		updateFieldValue(intensity, m_groupSettings->intensity());
		updateFieldValue(Ui::Path::depth, m_groupSettings->depth());
	}
	else {
		hide();
	}
}

}