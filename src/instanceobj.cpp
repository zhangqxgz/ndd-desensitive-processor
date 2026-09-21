#include "instanceobj.h"
#include "mainwindow.h"

InstanceObj::InstanceObj(QWidget *pNotepad)
    : QObject(pNotepad)
{
    m_pNotepad = pNotepad;
}

InstanceObj::~InstanceObj() = default;

void InstanceObj::doMainWork()
{
    if (m_pMainWindow.isNull())
    {
        m_pMainWindow = new MainWindow(m_pNotepad);
    }
    m_pMainWindow->show();
    m_pMainWindow->raise();
    m_pMainWindow->activateWindow();
}
