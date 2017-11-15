#include <Windows.h>
#include <stdint.h>
#include <math.h>
#include "DrawOntoDC.h"

#define global_variable static
global_variable BOOL running;

DrawOntoDC * drawOntoDC = NULL;

LRESULT CALLBACK WindowProc(HWND window, UINT message, WPARAM wParam, LPARAM lParam) {
	LRESULT result = 0;

	switch (message) {
		
		case WM_SIZE:
		{
			if( drawOntoDC != NULL ) {
				drawOntoDC->updateWindowDimensions();
			}
		}
		break;
		
		case WM_DESTROY:
		{
			if( drawOntoDC != NULL ) {
				delete drawOntoDC;
				drawOntoDC = NULL;
			}
			running = false;
			PostQuitMessage(0);
			return 0;
		}
		break;

		case WM_CLOSE: 
		{
			if( drawOntoDC != NULL ) {
				delete drawOntoDC;
				drawOntoDC = NULL;
			}
			running = false;
			DestroyWindow(window);
			return 0;
		}	
		break;
		
		case WM_PAINT:
		{
			if( drawOntoDC != NULL ) {
				drawOntoDC->draw();
			}
		}
		
		default:
		{
			result = DefWindowProc(window, message, wParam, lParam);
		}
		break;
	}
	
	return result;
}

int CALLBACK WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
	// Create the window class
	WNDCLASS windowClass = {};

	windowClass.style = CS_OWNDC|CS_HREDRAW|CS_VREDRAW;
	windowClass.lpfnWndProc = WindowProc;
	windowClass.hInstance = hInstance;
	windowClass.lpszClassName = "Windows Program Class";

	if(RegisterClass(&windowClass)) {
		// Create the window
		HWND windowHandle = CreateWindowEx(
								0,									// Extended style
								windowClass.lpszClassName,			// Class name
								"My Windows Program",				// Window name
								WS_OVERLAPPEDWINDOW | WS_VISIBLE,	// Style of the window
								CW_USEDEFAULT,						// Initial X position
								CW_USEDEFAULT,						// Initial Y position
								CW_USEDEFAULT,						// Initial width
								CW_USEDEFAULT,						// Initial height 
								0,									// Handle to the window parent
								0,									// Handle to a menu
								hInstance,							// Handle to an instance
								0);									// Pointer to a CLIENTCTREATESTRUCT
		if(windowHandle) {
			DrawOntoDC * drawOntoDC = new DrawOntoDC(windowHandle);

			running = true;

			// Message loop
			while(running) {
				MSG message;
				while(PeekMessage(&message, 0, 0, 0, PM_REMOVE)) {
					if(message.message == WM_QUIT) {
						running = false;
					}
					TranslateMessage(&message);
					DispatchMessage(&message);
				}
				if( drawOntoDC != NULL ) {
					drawOntoDC->draw();
				}
			}
		} else {
			// Logging
		}
	} else {
		// Logging
	}

	return 0;
}
