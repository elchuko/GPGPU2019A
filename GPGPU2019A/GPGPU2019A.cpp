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

int WINAPI WinMain(
	HINSTANCE hinstance,
	HINSTANCE hPrevInstance, //ya es obsoleto, siempre es 0
	char* pszCmdLine, //Linea de comandos
	int nCmdShow
)
{

}

