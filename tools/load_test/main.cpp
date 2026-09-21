#include <QApplication>
#include <QLibrary>
#include <QTextStream>
#include <pluginGl.h>

typedef bool (*IdentifyFn)(NDD_PROC_DATA *);

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    QTextStream out(stdout);

    const QString dll = QStringLiteral("desensitive_processor.dll");
    QLibrary lib(dll);
    if (!lib.load())
    {
        out << "LOAD FAILED: " << lib.errorString() << Qt::endl;
        return 1;
    }

    IdentifyFn identify = reinterpret_cast<IdentifyFn>(lib.resolve("NDD_PROC_IDENTIFY"));
    if (identify == nullptr)
    {
        out << "RESOLVE FAILED: NDD_PROC_IDENTIFY" << Qt::endl;
        return 1;
    }

    NDD_PROC_DATA data;
    if (!identify(&data))
    {
        out << "IDENTIFY RETURNED FALSE" << Qt::endl;
        return 1;
    }

    out << "PLUGIN_NAME=" << data.m_strPlugName << Qt::endl;
    out << "MENU_TYPE=" << data.m_menuType << Qt::endl;
    out << "VERSION=" << data.m_version << Qt::endl;
    out << "AUTHOR=" << data.m_auther << Qt::endl;
    out << "SMOKE_TEST_PASSED" << Qt::endl;
    return 0;
}
