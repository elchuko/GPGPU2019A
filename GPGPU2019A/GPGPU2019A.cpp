// GPGPU2019A.cpp : Define el punto de entrada de la aplicación.
//

#include "stdafx.h"

/*Calling conventions*/
/* Es la forma en la que la funcion invocadora, pasa los
parametros a la pila y recoje los resultados tras la ejecucion
de una funcion invocada*/

//__cdecl Caller Pushes / Caller Pops (Variale Args)
//__stdcall / __pascal Caller Pushes / Callee Pops ((es mas seguro))

HINSTANCE g_hInstance;

/*

	Paso 1: Registrar al menos una clase de ventana
			Nombre de Clase
			Un procedimiento ventana (puntero a una funcion)
			(otros datos)
			
			-> OS.
	Paso 2: Crear al menos una ventana a partir de una clase registrada.
			Nombre de CLase
			Titulo de Ventana
			Tamaño y posicion en pixeles
			Estilo Visual y comportamiento basico

			-> OS -> Ventana

	Paso 3: Implementar el bucle de mensajes. Cada hilo implementa una cola de mensajes
			donde los eventos del sistema y perifericos depositaran los sucesos.




*/

LRESULT CALLBACK WinProc(HWND, UINT, WPARAM, LPARAM);

int WINAPI WinMain(
	HINSTANCE hInstance,
	HINSTANCE hPrevInstance, //ya es obsoleto, siempre es 0
	char* pszCmdLine, //Linea de comandos
	int nCmdShow
)
{
	//Paso 1
	g_hInstance = hInstance;
	WNDCLASSEX wnc;
	memset(&wnc, 0, sizeof(WNDCLASSEX));
	wnc.cbSize = sizeof(WNDCLASSEX); // es un versionado, porque WNDCLASSEX es diferente en cada version de Windows
	wnc.hInstance = hInstance;
	wnc.lpszClassName = L"GPGPU2019A";
	wnc.lpfnWndProc = WinProc;
	wnc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	RegisterClassEx(&wnc);

	//Paso 2
	HWND hWnd = CreateWindowEx(WS_EX_OVERLAPPEDWINDOW, L"GPGPU2019A", L"Programacion de procesadores graficos 2019", WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, NULL,
		NULL, hInstance, NULL);
	ShowWindow(hWnd, nCmdShow);

	//Paso 3
	MSG msg;
	while (GetMessage(&msg, NULL, 0, 0))
	{
		DispatchMessage(&msg);
	}
	return 0;
}

LRESULT CALLBACK WinProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_CREATE:
		return 0;
	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;
	case WM_CLOSE:
		if (IDYES == MessageBox(hWnd, L"Desea salir?", L"Salir", MB_YESNO | MB_ICONQUESTION))
		{
			DestroyWindow(hWnd);
		}

	}
	return DefWindowProc(hWnd, message, wParam, lParam);
}