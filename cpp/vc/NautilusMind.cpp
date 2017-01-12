#include <Windows.h>

LPCTSTR ClsName = "NautilusNet";
LPCTSTR WndName = "Nautilus Mind 1.0";

/* FOR DOUBLE BUFFERING  **/
HDC memDC;
HBITMAP bufferBmp;
HGDIOBJ oldMemBMP;
RECT clientRect;
int clientWidth;
int clientHeight;

LRESULT CALLBACK windProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
void onSizeChanged(HWND hwnd, WPARAM wParam, LPARAM lParam);

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
    
    HDC hDC;
    PAINTSTRUCT ps;
	HBRUSH brush;
    
	switch(msg) {
        
        case WM_PAINT:
            hDC = BeginPaint(hwnd, &ps);
			
			GetClientRect(hwnd, &clientRect);
	
			clientWidth = clientRect.right - clientRect.left;
			clientHeight = clientRect.bottom - clientRect.top;
			
			brush = CreateSolidBrush(RGB(25,25,25));
			SelectObject(memDC, bufferBmp);
			FillRect(memDC, &clientRect, brush);
			BitBlt(hDC, 0, 0, clientWidth, clientHeight, memDC, 0, 0, SRCCOPY);
			DeleteDC (hDC);
			DeleteObject(brush);
			
			
            EndPaint(hwnd, &ps);
        break;
        
        case WM_ERASEBKGND:
            return 1;
        break;
		
		case WM_SIZE:
			onSizeChanged(hwnd, wParam, lParam);
		break;
        
		case WM_DESTROY:
			SelectObject(memDC, oldMemBMP);
			DeleteObject(bufferBmp);
			DeleteDC (memDC);
			PostQuitMessage(WM_QUIT);
		break;
		
		default:
			return DefWindowProc(hwnd, msg, wParam, lParam);
	}
	
	return 0;
}

void onSizeChanged(HWND hwnd, WPARAM wParam, LPARAM lParam) {
	HDC hDC;
	PAINTSTRUCT ps;
	int width = LOWORD(lParam);
	int height = HIWORD(lParam);
	//HBRUSH brush = CreateSolidBrush(RGB(245,245,245));
	
	GetClientRect(hwnd, &clientRect);
	
	clientWidth = clientRect.right - clientRect.left;
	clientHeight = clientRect.bottom - clientRect.top;
	
	hDC = BeginPaint(hwnd, &ps);
	memDC = CreateCompatibleDC(hDC);
	bufferBmp = CreateCompatibleBitmap(hDC, clientWidth, clientHeight);
	oldMemBMP = SelectObject(memDC, bufferBmp);
	//FillRect(memDC, &clientRect, brush);
	DeleteDC(hDC);
	//DeleteObject(brush);
    EndPaint(hwnd, &ps);
}