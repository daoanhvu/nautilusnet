#include <Windows.h>

LPCTSTR ClsName = "NautilusNet";
LPCTSTR WndName = "Nautilus Mind 1.0";

LRESULT CALLBACK windProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

int CALLBACK WinMain(_In_ HINSTANCE hInstance, _In_ HINSTANCE hPreIns, _In_ LPSTR lpCmdLine, _In_ int nCmd) {
	WNDCLASSEX wndex;
	MSG msg;
	HWND hWnd;
	
	wndex.cbSize = sizeof(WNDCLASSEX);
	wndex.style = CS_HREDRAW | CS_VREDRAW;
	wndex.cbClsExtra = 0;
	wndex.cbWndExtra = 0;
	wndex.hInstance = hInstance;
	wndex.lpfnWndProc = windProc;
	wndex.hIcon       = LoadIcon(NULL, IDI_APPLICATION);
	wndex.hIconSm     = LoadIcon(NULL, IDI_APPLICATION);
	wndex.hCursor     = LoadCursor(NULL, IDC_ARROW);
	wndex.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
	wndex.lpszMenuName  = NULL;
	wndex.lpszClassName = ClsName;
	
	RegisterClassEx(&wndex);
	
	// Create the window object
	hWnd = CreateWindow(ClsName,
			  WndName,
			  WS_OVERLAPPEDWINDOW,
			  CW_USEDEFAULT,
			  CW_USEDEFAULT,
			  CW_USEDEFAULT,
			  CW_USEDEFAULT,
			  NULL,
			  NULL,
			  hInstance,
			  NULL);
	
	// Find out if the window was created
	if( !hWnd ) // If the window was not created,
		return 0; // stop the application

	// Display the window to the user
	ShowWindow(hWnd, SW_SHOWNORMAL);
	UpdateWindow(hWnd);

	// Decode and treat the messages
	// as long as the application is running
	while( GetMessage(&msg, NULL, 0, 0) ) {
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	return msg.wParam;
}

LRESULT CALLBACK windProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
	switch(msg) {
		case WM_DESTROY:
			PostQuitMessage(WM_QUIT);
		break;
		
		default:
			return DefWindowProc(hwnd, msg, wParam, lParam);
		
	}
	
	return 0;
}