#include <QAction>
#include <QObject>
#include <QString>
#include <QWidget>
#include <functional>
#include <pluginGl.h>
#include <qsciscintilla.h>

#include "instanceobj.h"

#ifdef WIN32
#include <Windows.h>
#endif

#define NDD_EXPORTDLL

#if defined(Q_OS_WIN)
#if defined(NDD_EXPORTDLL)
#define NDD_EXPORT __declspec(dllexport)
#else
#define NDD_EXPORT __declspec(dllimport)
#endif
#else
#define NDD_EXPORT __attribute__((visibility("default")))
#endif

#ifdef __cplusplus
extern "C" {
#endif

NDD_EXPORT bool NDD_PROC_IDENTIFY(NDD_PROC_DATA *pProcData);
NDD_EXPORT int NDD_PROC_MAIN(QWidget *pNotepad,
                             const QString &strFileName,
                             std::function<QsciScintilla *(QWidget *)> getCurEdit,
                             std::function<bool(QWidget *, int, void *)> pluginCallBack,
                             NDD_PROC_DATA *procData);

#ifdef __cplusplus
}
#endif

static NDD_PROC_DATA s_procData;
static InstanceObj *s_instance = nullptr;
QWidget *s_pMainNotepad = nullptr;
std::function<QsciScintilla *(QWidget *)> s_getCurEdit;
std::function<bool(QWidget *, int, void *)> s_invokeMainFun;

bool NDD_PROC_IDENTIFY(NDD_PROC_DATA *pProcData)
{
    if (pProcData == nullptr)
    {
        return false;
    }
    pProcData->m_strPlugName = QObject::tr("脱敏处理器");
    pProcData->m_strComment = QObject::tr("关键词映射脱敏与还原工具");
    pProcData->m_version = QStringLiteral("v1.0");
    pProcData->m_auther = QStringLiteral("qx");
    pProcData->m_menuType = 0;
    return true;
}

int NDD_PROC_MAIN(QWidget *pNotepad,
                  const QString &strFileName,
                  std::function<QsciScintilla *(QWidget *)> getCurEdit,
                  std::function<bool(QWidget *, int, void *)> pluginCallBack,
                  NDD_PROC_DATA *pProcData)
{
    Q_UNUSED(strFileName);

    if (pProcData == nullptr)
    {
        return -1;
    }

    if (s_instance == nullptr)
    {
        s_instance = new InstanceObj(pNotepad);
        s_instance->setObjectName(QStringLiteral("ndd_desensitive"));

        s_getCurEdit = getCurEdit;
        s_invokeMainFun = pluginCallBack;
        s_procData = *pProcData;
        s_pMainNotepad = pNotepad;

        QObject::connect(pProcData->m_pAction, &QAction::triggered, s_instance, &InstanceObj::doMainWork,
                         Qt::UniqueConnection);
    }

    return 0;
}

#ifdef WIN32
BOOL WINAPI DllMain(HINSTANCE hInst, DWORD fdwReason, LPVOID lpvReserved)
{
    Q_UNUSED(hInst);
    Q_UNUSED(fdwReason);
    Q_UNUSED(lpvReserved);
    switch (fdwReason)
    {
    case DLL_PROCESS_ATTACH:
    case DLL_THREAD_ATTACH:
        break;
    case DLL_THREAD_DETACH:
        break;
    case DLL_PROCESS_DETACH:
        if (nullptr == lpvReserved)
        {
        }
        break;
    }
    return TRUE;
}
#else
void onDllUnload(void)
{
}
#endif
