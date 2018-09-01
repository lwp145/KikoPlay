#include "UI/mainwindow.h"
#include <QApplication>
#include <Windows.h>
#include <DbgHelp.h>
#include <QMessageBox>
#include "globalobjects.h"
#pragma comment (lib,"DbgHelp.lib")
LONG AppCrashHandler(EXCEPTION_POINTERS *pException) 
{			
	HANDLE hDumpFile = CreateFile((LPCWSTR)(QCoreApplication::applicationDirPath() + QDateTime::currentDateTime().toString("\\yyyy-MM-dd-hh-mm-ss")+".dmp").utf16(), GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	if (hDumpFile != INVALID_HANDLE_VALUE) 
	{
		MINIDUMP_EXCEPTION_INFORMATION dumpInfo;
		dumpInfo.ExceptionPointers = pException;
		dumpInfo.ThreadId = GetCurrentThreadId();
		dumpInfo.ClientPointers = TRUE;
		MiniDumpWriteDump(GetCurrentProcess(), GetCurrentProcessId(), hDumpFile, MiniDumpNormal, &dumpInfo, NULL, NULL);
	}
	EXCEPTION_RECORD* record = pException->ExceptionRecord;
	QString errCode(QString::number(record->ExceptionCode, 16)), errAdr(QString::number((uint)record->ExceptionAddress, 16)), errMod;
	QMessageBox::critical(nullptr, "Error", QString("Error Code: %1 Error Address: %2").arg(errCode).arg(errAdr),QMessageBox::Ok);
	return EXCEPTION_EXECUTE_HANDLER;
}
int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
	SetUnhandledExceptionFilter((LPTOP_LEVEL_EXCEPTION_FILTER)AppCrashHandler);
    QString qss;
    QFile qssFile(":/res/style.qss");
    qssFile.open(QFile::ReadOnly);

    if(qssFile.isOpen())
    {
        qss = QLatin1String(qssFile.readAll());
        qApp->setStyleSheet(qss);
        qssFile.close();
    }
    QString trans(QString(":/res/lang/%1.qm").arg(QLocale::system().name()));
    QTranslator translator;
    if(translator.load(trans))
        a.installTranslator(&translator);
    GlobalObjects::init();
    MainWindow w;
    w.show();

    return a.exec();
}
