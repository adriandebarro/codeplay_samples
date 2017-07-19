#pragma once 
#include <tchar.h>
#include <Windows.h>
#include <Dbghelp.h>

namespace Engine
{
	namespace System
	{
		namespace DumpFile
		{
			///-------------------------------------------------------------------------------------------------
			/// <summary>
			/// 	Makes a minidump when the process craches. 
			/// </summary>
			///
			/// <remarks>	Adrian, 29/12/2015. </remarks>
			///
			/// <param name="e">	[in,out] If non-null, the EXCEPTION_POINTERS to process. </param>
			///-------------------------------------------------------------------------------------------------
			static void make_minidump(EXCEPTION_POINTERS* e)
			{
				const DWORD Flags = MiniDumpWithFullMemory |
					MiniDumpWithFullMemoryInfo |
					MiniDumpWithHandleData |
					MiniDumpWithUnloadedModules |
					MiniDumpWithProcessThreadData|
					MiniDumpWithIndirectlyReferencedMemory|
					MiniDumpWithThreadInfo;


				auto hDbgHelp = LoadLibraryA("dbghelp");
				if (hDbgHelp == nullptr)
					return;
				auto pMiniDumpWriteDump = (decltype(&MiniDumpWriteDump))GetProcAddress(hDbgHelp, "MiniDumpWriteDump");
				if (pMiniDumpWriteDump == nullptr)
					return;

				char name[MAX_PATH];
				{
					auto nameEnd = name + GetModuleFileNameA(GetModuleHandleA(0), name, MAX_PATH);
					SYSTEMTIME t;
					GetSystemTime(&t);
					wsprintfA(nameEnd - strlen(".exe"),
						"_%4d%02d%02d_%02d%02d%02d.dmp",
						t.wYear, t.wMonth, t.wDay, t.wHour, t.wMinute, t.wSecond);
				}

				auto hFile = CreateFileA(name, GENERIC_WRITE, FILE_SHARE_READ, 0, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, 0);
				if (hFile == INVALID_HANDLE_VALUE)
					return;

				MINIDUMP_EXCEPTION_INFORMATION exceptionInfo;
				exceptionInfo.ThreadId = GetCurrentThreadId();
				exceptionInfo.ExceptionPointers = e;
				exceptionInfo.ClientPointers = TRUE;

				auto dumped = pMiniDumpWriteDump(
					GetCurrentProcess(),
					GetCurrentProcessId(),
					hFile,
					(MINIDUMP_TYPE)Flags,
					e ? &exceptionInfo : nullptr,
					nullptr,
					nullptr);

				CloseHandle(hFile);

				return;
			}

			///-------------------------------------------------------------------------------------------------
			/// <summary>	Handler, called when the unhandled. </summary>
			///
			/// <remarks>	Adrian, 29/12/2015. </remarks>
			///
			/// <param name="e">	[in,out] If non-null, the EXCEPTION_POINTERS to process. </param>
			///
			/// <returns>	A CALLBACK. </returns>
			///-------------------------------------------------------------------------------------------------
			static LONG CALLBACK unhandled_handler(EXCEPTION_POINTERS* e)
			{
				make_minidump(e);
				return EXCEPTION_CONTINUE_SEARCH;
			}
		}

	}
}