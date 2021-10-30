#include <mainwindow.h>
#include <info.h>
#include <profile.h>
#include <task/path.h>
#include <task/task.h>
#include <view2d/viewport.h>
#include <dialogs/settings/settings.h>
#include <dialogs/transform.h>

#include <QFileDialog>
#include <QMessageBox>
#include <QSplitter>
#include <QComboBox>
#include <QErrorMessage>
#include <QDebug>

namespace view
{

QWidget *MainWindow::setupLeftPanel()
{
	task::Task *task = new task::Task(m_app);
	task::Path *path = new task::Path(m_app);

	QSplitter *vertSplitter = new QSplitter(Qt::Vertical, this);
	vertSplitter->addWidget(task);
	vertSplitter->addWidget(path);
	vertSplitter->setStretchFactor(0, 1);
	vertSplitter->setStretchFactor(1, 0);

	return vertSplitter;
}

QWidget *MainWindow::setupCenterPanel()
{
	view2d::Viewport *viewport = new view2d::Viewport(m_app);
	Info *info = new Info(viewport);

	QWidget *container = new QWidget(this);
	QVBoxLayout *layout = new QVBoxLayout();

	layout->addWidget(viewport);
	layout->addWidget(info);

	container->setLayout(layout);

	return container;
}

void MainWindow::setupToolBar()
{
	Profile *profileBar = new Profile(m_app);
	toolBar->addWidget(profileBar);
}

void MainWindow::setupUi()
{
	Ui::MainWindow::setupUi(this);

	QSplitter *horiSplitter = new QSplitter(Qt::Horizontal, this);
	horiSplitter->addWidget(setupLeftPanel());
	horiSplitter->addWidget(setupCenterPanel());
	horiSplitter->setStretchFactor(0, 0);
	horiSplitter->setStretchFactor(1, 1);

	horizontalLayout->addWidget(horiSplitter);

	setupToolBar();
}

void MainWindow::setupMenuActions()
{
	// File actions
	connect(actionOpenFile, &QAction::triggered, this, &MainWindow::openFile);
	connect(actionSaveFile, &QAction::triggered, this, &MainWindow::saveFile);
	connect(actionSaveAsFile, &QAction::triggered, this, &MainWindow::saveAsFile);
	connect(actionExportFile, &QAction::triggered, this, &MainWindow::exportFile);
	connect(actionOpenSettings, &QAction::triggered, this, &MainWindow::openSettings);

	// Edit actions
	connect(actionLeftCutterCompensation, &QAction::triggered, &m_app, &model::Application::leftCutterCompensation);
	connect(actionRightCutterCompensation, &QAction::triggered, &m_app, &model::Application::rightCutterCompensation);
	connect(actionResetCutterCompensation, &QAction::triggered, &m_app, &model::Application::resetCutterCompensation);
	connect(actionHideSelection, &QAction::triggered, &m_app, &model::Application::hideSelection);
	connect(actionShowHidden, &QAction::triggered, &m_app, &model::Application::showHidden);
	connect(actionTransformSelection, &QAction::triggered, this, &MainWindow::transformSelection);
}

void MainWindow::setTaskToolsEnabled(bool enabled)
{
	actionExportFile->setEnabled(enabled);
	actionLeftCutterCompensation->setEnabled(enabled);
	actionRightCutterCompensation->setEnabled(enabled);
	actionResetCutterCompensation->setEnabled(enabled);
	actionHideSelection->setEnabled(enabled);
	actionShowHidden->setEnabled(enabled);
	actionTransformSelection->setEnabled(enabled);
}

MainWindow::MainWindow(model::Application &app)
	:m_app(app)
{
	setupUi();
	setupMenuActions();
	showMaximized();

	setTaskToolsEnabled(false);

	connect(&m_app, &model::Application::titleChanged, this, &MainWindow::setWindowTitle);
	connect(&m_app, &model::Application::documentChanged, this, &MainWindow::documentChanged);
	connect(&m_app, &model::Application::errorRaised, this, &MainWindow::displayError);
}

void MainWindow::openFile()
{
	const QString fileName = QFileDialog::getOpenFileName(this);
	if (!fileName.isEmpty() && !m_app.loadFile(fileName)) {
		QMessageBox::critical(this, "Error", "Invalid file type " + fileName);
	}
}

void MainWindow::saveFile()
{
	const QString fileName = m_app.lastHandledFileBaseName();
	if (fileName.isEmpty()) {
		saveAsFile();
	}
	else if (!m_app.saveToDxfplot(fileName)) {
		QMessageBox::critical(this, "Error", "Couldn't save " + fileName);
	}
}

void MainWindow::saveAsFile()
{
	const QString defaultPath = m_app.lastHandledFileBaseName() + ".dxfplot";
	const QString fileName = QFileDialog::getSaveFileName(this, "Save As File", defaultPath, "Text files (*.dxfplot)");

	if (!fileName.isEmpty() && !m_app.saveToDxfplot(fileName)) {
		QMessageBox::critical(this, "Error", "Couldn't save " + fileName);
	}
}

void MainWindow::exportFile()
{
	const QString defaultPath = m_app.lastHandledFileBaseName() + ".ngc";
	const QString fileName = QFileDialog::getSaveFileName(this, "Export File", defaultPath, "Text files (*.ngc *.txt)");

	if (!fileName.isEmpty() && !m_app.saveToGcode(fileName)) {
		QMessageBox::critical(this, "Error", "Couldn't save " + fileName);
	}
}

void MainWindow::openSettings()
{
	settings::Settings settings(m_app);
	if (settings.exec() == QDialog::Accepted) {
		m_app.setConfig(settings.newConfig());
	}
}

void MainWindow::transformSelection()
{
	Transform transform;
	if (transform.exec() == QDialog::Accepted) {
		m_app.transformSelection(transform.matrix());
	}
}

void MainWindow::documentChanged(model::Document *newDocument)
{
	setTaskToolsEnabled((newDocument != nullptr));
}

void MainWindow::displayError(const QString &message)
{
	QMessageBox messageBox;
	messageBox.critical(this, "Error", message);
}

}
